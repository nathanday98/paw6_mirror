#include "job_graph.h"

#include <atomic>
#include <new>

#include <shared/memory.h>
#include <shared/log.h>

#include <platform/platform.h>

#include "profiler.h"
#include "allocators/arena_allocator.h"
#include "allocators/frame_allocator.h"
#include "job_queue.h"

// Ported from https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
template <typename T>
struct MPMCBoundedQueue
{

	void init(usize buffer_size, Allocator* in_allocator)
	{
		allocator = in_allocator;
		this->buffer_mask = buffer_size - 1;

		PAW_ASSERT((buffer_size >= 2) && ((buffer_size & (buffer_size - 1)) == 0));

		buffer = allocator->alloc<Cell>(buffer_size);

		for (usize i = 0; i != buffer_size; i += 1)
			buffer[i].sequence.store(i, std::memory_order_relaxed);

		enqueue_pos.store(0, std::memory_order_relaxed);
		dequeue_pos.store(0, std::memory_order_relaxed);
	}

	void deinit()
	{
		allocator->free(buffer);
	}

	bool try_enqueue(T const& data)
	{
		Cell* cell;
		usize pos = enqueue_pos.load(std::memory_order_relaxed);

		for (;;)

		{
			cell = &buffer[pos & buffer_mask];
			usize seq = cell->sequence.load(std::memory_order_acquire);
			intptr_t dif = (intptr_t)seq - (intptr_t)pos;
			if (dif == 0)
			{
				if (enqueue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
					break;
			}
			else if (dif < 0)
				return false;
			else
				pos = enqueue_pos.load(std::memory_order_relaxed);
		}

		cell->data = data;
		cell->sequence.store(pos + 1, std::memory_order_release);

		return true;
	}

	void force_enqueue(T const& data)
	{
		int attempts = 0;
		while (!try_enqueue(data))
		{
			if (++attempts > 10000)
			{
				PAW_UNREACHABLE;
			}
		}
	}

	bool try_dequeue(T& data)
	{
		Cell* cell;
		usize pos = dequeue_pos.load(std::memory_order_relaxed);

		for (;;)
		{
			cell = &buffer[pos & buffer_mask];
			usize seq = cell->sequence.load(std::memory_order_acquire);
			intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);

			if (dif == 0)
			{
				if (dequeue_pos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
					break;
			}
			else if (dif < 0)
				return false;
			else
				pos = dequeue_pos.load(std::memory_order_relaxed);
		}

		data = cell->data;
		cell->sequence.store(pos + buffer_mask + 1, std::memory_order_release);

		return true;
	}

	void force_dequeue(T& data)
	{
		int attempts = 0;
		while (!try_dequeue(data))
		{
			if (++attempts > 10000)
			{
				PAW_UNREACHABLE;
			}
		}
	}

private:
	static usize const cacheline_size = 64;
	typedef char CachelinePad[cacheline_size];

	struct Cell
	{
		std::atomic<usize> sequence;
		T data;
	};

	Allocator* allocator;
	CachelinePad pad0;
	Slice<Cell> buffer;
	usize buffer_mask;
	CachelinePad pad1;
	std::atomic<usize> enqueue_pos;
	CachelinePad pad2;
	std::atomic<usize> dequeue_pos;
	CachelinePad pad3;
};

struct Job;

struct JobRelationship
{
	Job* job;
	JobRelationship* next_relationship;
};

//#define PAW_JOB_GRAPH_VALIDATION

enum class JobType
{
	Normal,
	Wait,
	FrameEnd,
	Count,
};

static constexpr char const* g_job_type_names[(int)JobType::Count]{
	"Normal",
	"Wait",
	"FrameEnd",
};

struct Job
{
	UTF8StringView name;
	JobBase* callable;

	JobGraph* graph;

	JobType type;
	// #TODO: This shouldn't need to be in every job
	JobWaitable waitable;

	JobSrc src;

	std::atomic<usize> parents_left_to_complete;
	std::atomic<JobRelationship*> first_child;

#ifdef PAW_JOB_GRAPH_VALIDATION
	usize validation_job_index;
#endif

	std::atomic<bool> attachable;
};

#ifdef PAW_JOB_GRAPH_VALIDATION
struct ValidationJob;

enum class RelationshipType
{
	Parent,
	Child,
};

struct ValidationJobRelationship
{
	ValidationJob* job;
	ValidationJobRelationship* next_relationship;
	RelationshipType type;
};

struct ValidationJob
{
	UTF8StringView name;
	Slice<JobResource> resources;
	JobType type;
	std::atomic<ValidationJobRelationship*> first_parent;
	std::atomic<ValidationJobRelationship*> first_child;
};

static constexpr UTF8StringView g_access_type_strings[(usize)ResourceAccessType::Count]{
	"ReadOnly"_str,
	"ReadWrite"_str,
};

struct Validator
{
	Slice<ValidationJob> jobs;
	std::atomic<usize> job_count;
	Slice<ValidationJobRelationship> job_relationships;
	std::atomic<usize> job_relationship_count;
	// #TODO: Replace with a critical section
	PlatformMutex mutex;
	Slice<s32> parents_buffer;

	void init(Platform& platform, ArenaAllocator& debug_persistent_allocator, usize in_job_count, usize in_relationship_count)
	{
		jobs = debug_persistent_allocator.alloc<ValidationJob>(in_job_count);
		job_relationships = debug_persistent_allocator.alloc<ValidationJobRelationship>(in_relationship_count);
		parents_buffer = debug_persistent_allocator.alloc<s32>(in_job_count);
		job_count = 0;
		job_relationship_count = 0;
		mutex = platform.create_mutex();
	}

	void deinit(Platform& platform)
	{
		platform.destroy_mutex(mutex);
	}

	struct ResourceUsageCount
	{
		usize usages[(usize)ResourceAccessType::Count];
	};

	void process_parent_usages(ValidationJob const& job)
	{
		for (ValidationJobRelationship* relationship = job.first_parent; relationship != nullptr; relationship = relationship->next_relationship)
		{
			ValidationJob* parent = relationship->job;
			PAW_ASSERT(relationship->type == RelationshipType::Parent);

			process_parent_usages(*parent);
		}

		const usize index = ((usize)&job - (usize)jobs.ptr) / sizeof(ValidationJob);
		parents_buffer[index] = true;
	}

	usize push_job(Platform& platform, UTF8StringView const& name, Slice<JobHandle const> const dependencies, Slice<JobResource> const& in_resources, JobType type)
	{
		PAW_PROFILER_FUNC();
		platform.lock_mutex(mutex);

		{
			memset(parents_buffer.ptr, 0, parents_buffer.calc_total_size_in_bytes());
			usize conflict_count = 0;
			for (JobHandle parent_handle : dependencies)
			{
				Job& parent_job = *(Job*)parent_handle.handle;
				ValidationJob& parent = jobs[parent_job.validation_job_index];

				process_parent_usages(parent);
			}

			for (usize job_index = 0; job_index < job_count; ++job_index)
			{
				if (parents_buffer[job_index])
				{
					continue;
				}

				ValidationJob const& job = jobs[job_index];

				if (type == JobType::Wait || job.type == JobType::Wait || job.type == JobType::FrameEnd || type == JobType::FrameEnd)
				{
					PAW_LOG_ERROR("Job Graph Validation Error: Type Conflict\n\t%p{str} type: %s\n\t%p{str} type: %s", &name, g_job_type_names[int(type)], &job.name, g_job_type_names[int(job.type)]);
					conflict_count++;
				}

				if (job.type == JobType::Normal && type == JobType::Normal)
				{

					for (JobResource const& decl_resource : in_resources)
					{
						for (JobResource const& job_resource : job.resources)
						{
							// Pretty sure these need to just be > so that we can have memory regions that are directly next to each other not conflicting
							if (job_resource.range_end > decl_resource.range_start && decl_resource.range_end > job_resource.range_start)
							{
								switch (decl_resource.access)
								{
									case ResourceAccessType::ReadOnly:
									{
										// Allow multiple readers
										if (job_resource.access == ResourceAccessType::ReadWrite)
										{
											PAW_LOG_ERROR("Job Graph Validation Error: Resource Conflict\n\t%p{str}: param %d (%p{str})\n\t%p{str}: param %d (%p{str})", &name, decl_resource.arg_index, &g_access_type_strings[(int)decl_resource.access], &job.name, job_resource.arg_index, &g_access_type_strings[(int)job_resource.access]);
											conflict_count++;
										}
									}
									break;

									case ResourceAccessType::ReadWrite:
									{
										// No other readers or writers can be active when a single writer is active
										PAW_LOG_ERROR("Job Graph Validation Error: Resource Conflict\n\t%p{str}: param %d (%p{str})\n\t%p{str}: param %d (%p{str})", &name, decl_resource.arg_index, &g_access_type_strings[(int)decl_resource.access], &job.name, job_resource.arg_index, &g_access_type_strings[(int)job_resource.access]);
										conflict_count++;
									}
									break;

									default:
									{
										PAW_UNREACHABLE;
									}
									break;
								}
							}
						}
					}
				}
			}

			if (conflict_count > 0)
			{
				PAW_LOG_ERROR("Found %llu conflicts", conflict_count);
			}
			PAW_ASSERT(conflict_count == 0);
		}

		PAW_ASSERT(job_count < jobs.size);
		const usize free_job_index = job_count.fetch_add(1, std::memory_order_seq_cst);
		ValidationJob& free_job = jobs[free_job_index];
		free_job.name = name;
		free_job.first_child = nullptr;
		free_job.first_parent = nullptr;
		free_job.resources = in_resources;
		free_job.type = type;

		for (JobHandle parent_handle : dependencies)
		{
			Job& parent_job = *(Job*)parent_handle.handle;
			ValidationJob& parent = jobs[parent_job.validation_job_index];

			PAW_ASSERT(job_relationship_count < job_relationships.size);
			ValidationJobRelationship& free_child_relationship = job_relationships[job_relationship_count.fetch_add(1, std::memory_order_seq_cst)];
			free_child_relationship.job = &free_job;
			free_child_relationship.type = RelationshipType::Child;

			// This needs to be lock free because we could be adding to an existing job
			while (true)
			{
				ValidationJobRelationship* old_first = parent.first_child;
				free_child_relationship.next_relationship = old_first;
				if (parent.first_child.compare_exchange_weak(old_first, &free_child_relationship))
				{
					break;
				}
			}

			PAW_ASSERT(job_relationship_count < job_relationships.size);
			ValidationJobRelationship& free_parent_relationship = job_relationships[job_relationship_count.fetch_add(1, std::memory_order_seq_cst)];
			free_parent_relationship.job = &parent;
			free_parent_relationship.type = RelationshipType::Parent;

			// This doesn't need to be lock-free because it's a new job
			free_parent_relationship.next_relationship = free_job.first_parent;
			free_job.first_parent = &free_parent_relationship;
		}

		platform.unlock_mutex(mutex);
		return free_job_index;
	}

	void reset()
	{
		job_count = 0;
		job_relationship_count = 0;
	}
};
#endif

enum class FrameState
{
	Initial, // Initialized but not inside a frame yet
	Active,	 // Inside of a frame currently
	Waiting,
	Ended, // A job has requested that the frame should end after it's finished
};

struct JobGraph
{
	Slice<JobRelationship> job_relationships;
	MPMCBoundedQueue<JobRelationship*> free_job_relationships;
	Platform* platform;
	FrameAllocator frame_allocator;
#ifdef PAW_JOB_GRAPH_VALIDATION
	FrameAllocator debug_frame_allocator;
	Validator validator;
#endif

	std::atomic<FrameState> frame_state;

	Job initial_job;
#ifdef PAW_JOB_GRAPH_VALIDATION
	Slice<JobResource> initial_job_resources;
#endif
	Job* wait_job;

	usize wait_return_count = 0;

	char const* profiler_frame_name;
};

struct Policy
{
	Slice<JobResource const> resources;
};

static thread_local Policy const* g_current_policy = nullptr;

JobGraph* job_graph_init(Platform* platform, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent, PagePool& page_pool, PagePool& debug_page_pool, char const* profiler_frame_name)
{
	JobGraph& self = persistent_allocator.alloc_and_construct_single<JobGraph>();
	self.platform = platform;
	self.frame_allocator.init(persistent_allocator, page_pool);

	self.job_relationships = persistent_allocator.alloc<JobRelationship>(8192);
	self.free_job_relationships.init(self.job_relationships.size, &persistent_allocator);

	for (JobRelationship& job : self.job_relationships)
	{
		self.free_job_relationships.force_enqueue(&job);
	}

#ifdef PAW_JOB_GRAPH_VALIDATION
	self.debug_frame_allocator.init(debug_persistent, debug_page_pool);
	self.validator.init(*platform, debug_persistent, 4096, self.job_relationships.size);
#else
	PAW_UNUSED_ARG(debug_persistent);
	PAW_UNUSED_ARG(debug_page_pool);
#endif

	self.frame_state = FrameState::Initial;

	self.profiler_frame_name = profiler_frame_name;

	return &self;
}

void job_graph_deinit(JobGraph* self)
{

#ifdef PAW_JOB_GRAPH_VALIDATION
	self->validator.deinit(*self->platform);
#else
	PAW_UNUSED_ARG(self);
#endif
}

void job_graph_set_initial_job_internal(JobGraph* self, JobSrc&& src, UTF8StringView const& name, JobBase* callable, usize callable_size_bytes, usize callable_alignment, Slice<JobResource const> const& resources, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator)
{
	Slice<byte> callable_memory = persistent_allocator.alloc_bytes(callable_size_bytes, callable_alignment);
	memCopy(callable, callable_memory.ptr, callable_size_bytes);

#ifdef PAW_JOB_GRAPH_VALIDATION
	if (resources.size > 0)
	{
		self->initial_job_resources = debug_persistent_allocator.alloc<JobResource>(resources.size);
		memCopy(resources.ptr, self->initial_job_resources.ptr, resources.calc_total_size_in_bytes());
	}
	else
	{
		self->initial_job_resources = {};
	}
#else
	PAW_UNUSED_ARG(resources);
	PAW_UNUSED_ARG(debug_persistent_allocator);
#endif

	self->initial_job.name = name;
	self->initial_job.callable = (JobBase*)callable_memory.ptr;
	self->initial_job.graph = self;
	self->initial_job.type = JobType::Normal;
	self->initial_job.first_child = nullptr;
	self->initial_job.parents_left_to_complete = 0;
	self->initial_job.attachable = true;
	self->initial_job.src = src;
}

JobHandle job_graph_add_internal(JobGraph* self, JobSrc&& src, UTF8StringView const& name, JobBase* callable, usize callable_size_bytes, usize callable_alignment, Slice<JobHandle const> const& dependencies, Slice<JobResource const> const& resources)
{
	PAW_PROFILER_FUNC();
	Slice<byte> callable_memory = self->frame_allocator.alloc_bytes(callable_size_bytes, callable_alignment);
	memCopy(callable, callable_memory.ptr, callable_size_bytes);

	Job& job = self->frame_allocator.alloc_and_construct_single<Job>();
	job.name = name;
	job.callable = (JobBase*)callable_memory.ptr;
	job.graph = self;
	job.type = JobType::Normal;
	job.first_child = nullptr;
	job.parents_left_to_complete = dependencies.size;
	job.attachable = true;
	job.src = src;
#ifdef PAW_JOB_GRAPH_VALIDATION
	Slice<JobResource> resources_copy{};
	if (resources.size > 0)
	{
		resources_copy = self->debug_frame_allocator.alloc<JobResource>(resources.size);
		memCopy(resources.ptr, resources_copy.ptr, resources.calc_total_size_in_bytes());
	}
	job.validation_job_index = self->validator.push_job(*self->platform, name, dependencies, resources_copy, JobType::Normal);
#else
	PAW_UNUSED_ARG(resources);
#endif

	for (JobHandle parent_handle : dependencies)
	{
		Job& parent = *(Job*)parent_handle.handle;

		PAW_ASSERT(parent.attachable);

		JobRelationship* free_child_relationship;
		self->free_job_relationships.force_dequeue(free_child_relationship);
		free_child_relationship->job = &job;

		// This needs to be lock free because we could be adding to an existing job
		while (true)
		{
			JobRelationship* old_first = parent.first_child;
			free_child_relationship->next_relationship = old_first;
			if (parent.first_child.compare_exchange_weak(old_first, free_child_relationship))
			{
				break;
			}
		}
	}

	return JobHandle{usize(&job)};
}

JobHandle job_graph_add_wait(JobGraph* self, JobSrc&& src, UTF8StringView const& name, Slice<JobHandle const> const& dependencies, JobWaitable waitable)
{
	Job& job = self->frame_allocator.alloc_and_construct_single<Job>();
	job.name = name;
	job.callable = nullptr;
	job.graph = self;
	job.type = JobType::Wait;
	job.first_child = nullptr;
	job.parents_left_to_complete = dependencies.size;
	job.attachable = true;
	job.waitable = waitable;
	job.src = src;
#ifdef PAW_JOB_GRAPH_VALIDATION
	job.validation_job_index = self->validator.push_job(*self->platform, name, dependencies, {}, JobType::Wait);
#endif

	for (JobHandle parent_handle : dependencies)
	{
		Job& parent = *(Job*)parent_handle.handle;

		PAW_ASSERT(parent.attachable);

		JobRelationship* free_child_relationship;
		self->free_job_relationships.force_dequeue(free_child_relationship);
		free_child_relationship->job = &job;

		// This needs to be lock free because we could be adding to an existing job
		while (true)
		{
			JobRelationship* old_first = parent.first_child;
			free_child_relationship->next_relationship = old_first;
			if (parent.first_child.compare_exchange_weak(old_first, free_child_relationship))
			{
				break;
			}
		}
	}

	return JobHandle{usize(&job)};
}

JobHandle job_graph_add_frame_end(JobGraph* self, JobSrc&& src, UTF8StringView const& name, Slice<JobHandle const> const& dependencies)
{
	Job& job = self->frame_allocator.alloc_and_construct_single<Job>();
	job.name = name;
	job.callable = nullptr;
	job.graph = self;
	job.type = JobType::FrameEnd;
	job.first_child = nullptr;
	job.parents_left_to_complete = dependencies.size;
	job.attachable = false;
	job.src = src;
#ifdef PAW_JOB_GRAPH_VALIDATION
	job.validation_job_index = self->validator.push_job(*self->platform, name, dependencies, {}, JobType::FrameEnd);
#endif

	for (JobHandle parent_handle : dependencies)
	{
		Job& parent = *(Job*)parent_handle.handle;

		PAW_ASSERT(parent.attachable);

		JobRelationship* free_child_relationship;
		self->free_job_relationships.force_dequeue(free_child_relationship);
		free_child_relationship->job = &job;

		// This needs to be lock free because we could be adding to an existing job
		while (true)
		{
			JobRelationship* old_first = parent.first_child;
			free_child_relationship->next_relationship = old_first;
			if (parent.first_child.compare_exchange_weak(old_first, free_child_relationship))
			{
				break;
			}
		}
	}

	return JobHandle{usize(&job)};
}

static void graph_func(void* data);

static void frame_end_graph_func(void* data)
{
	JobGraph* graph = (JobGraph*)data;
	// for (usize job_index = 0; job_index < self->validator.job_count; ++job_index)
	//{
	//	const ValidationJob& job = self->validator.jobs[job_index];
	//	printf("    %.*s;\n", (int)job.decl.name.size_bytes, job.decl.name.ptr);

	//	for (ValidationJobRelationship* relationship = job.first_child; relationship != nullptr; relationship = relationship->next_relationship)
	//	{
	//		const ValidationJob* child = relationship->job;
	//		printf("    %.*s-->%.*s;\n", (int)job.decl.name.size_bytes, job.decl.name.ptr, (int)child->decl.name.size_bytes, child->decl.name.ptr);
	//	}
	//}
	graph->frame_state = FrameState::Ended;
#ifdef PAW_JOB_GRAPH_VALIDATION
	graph->validator.reset();
	graph->debug_frame_allocator.reset();
#endif
	graph->frame_allocator.reset();

	PAW_PROFILER_FRAME_END(graph->profiler_frame_name);
	graph->wait_return_count = 0;
	job_graph_start(graph);
}

static void wait_graph_func(void* data);

static void process_job_end(JobGraph* graph, Job* job)
{
	PAW_ASSERT(graph->frame_state == FrameState::Active);
	usize child_count = 0;
	bool switching_to_wait_job = false;
	for (JobRelationship* relationship = job->first_child; relationship != nullptr;)
	{
		Job* child = relationship->job;

		// Cache next pointer in-case it's wiped when relationship is freed
		JobRelationship* next_relationship = relationship->next_relationship;

		if (child->parents_left_to_complete.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			switch (child->type)
			{
				case JobType::Normal:
				{
					job_queue_schedule({{.name = child->name, .data = child, .func = &graph_func}});
				}
				break;

				case JobType::Wait:
				{
					graph->wait_job = child;
					graph->frame_state = FrameState::Waiting;
					switching_to_wait_job = true;
				}
				break;

				case JobType::FrameEnd:
				{
					// #TODO: Might be pointless scheduling this, since we can't have overlapping jobs with the frame end
					job_queue_schedule({{.name = "FrameEnd"_str, .data = graph, .func = &frame_end_graph_func}});
				}
				break;
			}
		}

		graph->free_job_relationships.force_enqueue(relationship);

		relationship = next_relationship;

		child_count++;
	}

	job->first_child = nullptr;

	if (graph->frame_state == FrameState::Waiting)
	{
		PAW_ASSERT(child_count == 1);
		// Safe to do work in here because no jobs can overlap with waiting
		if (switching_to_wait_job)
		{
			if (!job_queue_load_wait_if_idle(graph->wait_job->waitable.index))
			{
				wait_graph_func(graph);
			}
		}
	}
}

static void graph_func(void* data)
{
	Job* job = (Job*)data;
	JobGraph* graph = job->graph;


	PAW_ASSERT(graph->frame_state == FrameState::Active);

	PAW_ASSERT(job->parents_left_to_complete == 0);

	PAW_ASSERT(job->name.null_terminated);
	{
		#ifdef PAW_JOB_GRAPH_VALIDATION
		const Policy policy{
			.resources = graph->validator.jobs[job->validation_job_index].resources.to_const_slice(),
		};

		g_current_policy = &policy;
		#endif

		job->callable->execute(graph, JobHandle{(usize)job});

		#ifdef PAW_JOB_GRAPH_VALIDATION
		g_current_policy = nullptr;
		#endif
	}

	job->attachable = false;

	process_job_end(graph, job);
}

static void wait_graph_func(void* data)
{
	JobGraph* graph = (JobGraph*)data;

	Job* job = graph->wait_job;
	// PAW_ASSERT(graph->wait_return_count == 0);
	PAW_ASSERT(graph->frame_state == FrameState::Waiting);
	job_queue_set_wait_to_idle(job->waitable.index);
	graph->wait_return_count++;
	graph->frame_state = FrameState::Active;
	process_job_end(graph, job);


}

void job_graph_start(JobGraph* self)
{
	PAW_ASSERT(self->frame_state == FrameState::Ended || self->frame_state == FrameState::Initial);
	self->frame_state = FrameState::Active;
	self->initial_job.attachable = true;

#ifdef PAW_JOB_GRAPH_VALIDATION
	self->initial_job.validation_job_index = self->validator.push_job(*self->platform, self->initial_job.name, {}, self->initial_job_resources, JobType::Normal);
#endif
	job_queue_schedule({{.name = self->initial_job.name, .data = &self->initial_job, .func = &graph_func}});
}

void job_graph_verify_active_policy(u64 range_start, u64 range_end, Slice<ResourceAccessType const>&& access_types)
{
#ifdef PAW_JOB_GRAPH_VALIDATION
	Policy const* policy = g_current_policy;
	PAW_ASSERT(policy);

	bool found = false;

	for (JobResource const& resource : policy->resources)
	{
		if (resource.range_end > range_start && range_end > resource.range_start)
		{
			for (ResourceAccessType const& access : access_types)
			{
				if (resource.access == access)
				{
					found = true;
					break;
				}
			}
		}
		if (found)
		{
			break;
		}
	}

	PAW_ASSERT(found);
#else
	PAW_UNUSED_ARG(range_start);
	PAW_UNUSED_ARG(range_end);
	PAW_UNUSED_ARG(access_types);
#endif
}

FrameAllocator& job_graph_get_frame_allocator(JobGraph* self)
{
	return self->frame_allocator;
}

JobWaitable job_graph_create_waitable(JobGraph* self, PlatformWaitHandle const& waitable)
{
	return {job_queue_push_wait_job(waitable, JobQueueDecl{.name = "JobGraphWait"_str, .data = self, .func = &wait_graph_func})};
}
