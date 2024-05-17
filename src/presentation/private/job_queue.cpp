#include "job_queue.h"

#include <shared/assert.h>

#include <platform/platform.h>

#include <atomic>

#include "allocators/arena_allocator.h"
#include "profiler.h"

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

struct Job
{
	UTF8StringView name;
	JobQueueFunc* func;
	void* data;
};

struct JobQueue;

struct WorkerData
{
	PlatformThread thread;
	u32 index;
};

static void worker_func(void*);

enum class WaitState
{
	Idle,
	Loaded,
	Triggered,
};

struct JobQueue
{
	MPMCBoundedQueue<Job> job_queue;
	Platform* platform;
	PlatformSemaphore worker_wake_semaphore;
	Slice<WorkerData> workers;
	std::atomic<bool> running;

	static constexpr u32 max_waits = 32;
	PlatformWaitHandle wait_handles[max_waits];
	Job wait_jobs[max_waits];
	std::atomic<WaitState> wait_states[max_waits];
	u32 wait_count;

	bool can_add_waitables;
};

static thread_local u32 g_thread_index;
static JobQueue* g_self;

void job_queue_init(Platform& platform, ArenaAllocator& persistent_allocator)
{
	g_self = &persistent_allocator.alloc_and_construct_single<JobQueue>();

	g_self->job_queue.init(4096, &persistent_allocator);
	g_self->platform = &platform;

	g_self->running = true;

	const u32 thread_count = 16;

	g_self->worker_wake_semaphore = platform.create_semaphore(0, thread_count);

	g_self->wait_handles[0] = platform.get_wait_handle(g_self->worker_wake_semaphore);
	g_self->wait_count++;

	g_self->workers = persistent_allocator.alloc<WorkerData>(thread_count);

	for (u32 i = 0; i < thread_count; ++i)
	{
		WorkerData& worker = g_self->workers[i];
		worker.index = i;
		worker.thread = platform.create_thread("Worker"_str, &worker_func, &worker);
	}

	g_self->can_add_waitables = true;

	PAW_ASSERT(g_self->wait_states[0].is_lock_free());
}

void job_queue_start()
{
	g_self->can_add_waitables = false;

	for (WorkerData& worker : g_self->workers)
	{
		g_self->platform->start_thread(worker.thread);
	}
}

void job_queue_deinit()
{
	g_self->running = false;

	for (usize i = 0; i < g_self->workers.size; ++i)
	{
		g_self->platform->signal_semaphore(g_self->worker_wake_semaphore);
	}

	for (WorkerData& worker : g_self->workers)
	{
		g_self->platform->wait_for_thread(worker.thread);
		g_self->platform->destroy_thread(worker.thread);
	}

	g_self->platform->destroy_semaphore(g_self->worker_wake_semaphore);
}

void job_queue_schedule(Slice<JobQueueDecl const> const&& jobs)
{
	for (JobQueueDecl const& decl : jobs)
	{
		g_self->job_queue.force_enqueue(Job{.name = decl.name, .func = decl.func, .data = decl.data});
		g_self->platform->signal_semaphore(g_self->worker_wake_semaphore);
	}
}

u32 job_queue_push_wait_job(PlatformWaitHandle const& wait_handle, JobQueueDecl const&& decl)
{
	PAW_ASSERT(g_self->can_add_waitables);
	const u32 write_index = g_self->wait_count++;
	PAW_ASSERT(write_index < JobQueue::max_waits);
	g_self->wait_handles[write_index] = wait_handle;
	g_self->wait_jobs[write_index] = Job{.name = decl.name, .func = decl.func, .data = decl.data};
	return write_index;
}

bool job_queue_load_wait_if_idle(u32 wait_index)
{
	PAW_ASSERT(wait_index < g_self->wait_count);

	WaitState expected = WaitState::Idle;
	if (g_self->wait_states[wait_index].compare_exchange_weak(expected, WaitState::Loaded))
	{
		return true;
	}

	PAW_ASSERT(expected == WaitState::Triggered);

	return false;
}

void job_queue_set_wait_to_idle(u32 wait_index)
{
	PAW_ASSERT(wait_index < g_self->wait_count);

	WaitState expected = WaitState::Triggered;
	if (g_self->wait_states[wait_index].compare_exchange_weak(expected, WaitState::Idle))
	{
		return;
	}

	PAW_UNREACHABLE;
}

usize job_queue_get_worker_count()
{
	return g_self->workers.size;
}

u32 job_queue_get_worker_index()
{
	return g_thread_index;
}

static void worker_func(void* in_ptr)
{
	WorkerData* data = (WorkerData*)in_ptr;
	g_thread_index = data->index;

	byte name_buffer[32];
	const UTF8StringView thread_name =
		string_formatNullTerminated(name_buffer, PAW_ARRAY_COUNT(name_buffer), "Worker %u", data->index);
	PAW_PROFILER_THREAD(thread_name.ptr);

	while (g_self->running)
	{
		Job job;
		while (g_self->job_queue.try_dequeue(job))
		{

			PAW_ASSERT(job.name.null_terminated);
			{
				PAW_PROFILER_SCOPE_DYNAMIC(job.name);
				job.func(job.data);
			}
		}

		/*usize completed_indices_arr[JobQueue::max_waits];
		Slice<usize> completed_indices{completed_indices_arr, JobQueue::max_waits};*/
		const usize completed_index = g_self->platform->wait_for_any({g_self->wait_handles, g_self->wait_count});

		if (completed_index > 0)
		{
			while (true)
			{
				WaitState expected = WaitState::Loaded;
				if (g_self->wait_states[completed_index].compare_exchange_weak(expected, WaitState::Triggered))
				{
					g_self->job_queue.force_enqueue(g_self->wait_jobs[completed_index]);
					g_self->platform->signal_semaphore(g_self->worker_wake_semaphore);
					break;
				}

				PAW_ASSERT(expected != WaitState::Triggered);

				// Wait has not been loaded with response, so just set it to triggered so the loader can handle it
				if (expected == WaitState::Idle)
				{
					if (g_self->wait_states[completed_index].compare_exchange_weak(expected, WaitState::Triggered))
					{
						break;
					}
				}
			}
		}

		// g_self->platform->wait_for_semaphore(g_self->worker_wake_semaphore);
	}
}
