#pragma once

#include <shared/std.h>
#include <shared/string.h>
#include <shared/slice.h>
#include <shared/string_id.h>

#include <source_location>
#include <utility>

struct Platform;
struct PlatformWaitHandle;
struct ArenaAllocator;
struct FrameAllocator;
struct PagePool;

typedef std::source_location JobSrc;
#define JOB_SRC JobSrc::current()

struct JobGraph;

struct JobHandle
{
	usize handle;
};

inline bool operator==(JobHandle a, JobHandle b)
{
	return a.handle == b.handle;
}

enum class ResourceAccessType
{
	ReadOnly,
	ReadWrite,
	Count,
};

struct JobResource
{
	u64 range_start;
	u64 range_end;
	ResourceAccessType access;
	u32 arg_index;
};

template <typename T>
struct JobResourceTypeInfo
{
	static constexpr bool value = false;
};

static inline constexpr JobHandle g_null_job = JobHandle{(usize)-1};

struct JobBase
{
	virtual void execute(JobGraph* job_graph, JobHandle job) = 0;
};

struct JobWaitable
{
	u32 index;
};

JobGraph* job_graph_init(Platform* platform, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent, PagePool& page_pool, PagePool& debug_page_pool, char const* profiler_frame_name);
void job_graph_deinit(JobGraph* self);

// Don't use this function unless absolutely necessary, you most likely want to use job_graph_set_initial_job in job_graph_add_helper.inl
void job_graph_set_initial_job_internal(JobGraph* self, JobSrc&& src, UTF8StringView const& name, JobBase* callable, usize callable_size_bytes, usize callable_alignment, Slice<JobResource const> const& resources, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator);

// Don't use this function unless absolutely necessary, you most likely want to use job_graph_add_job in job_graph_add_helper.inl
JobHandle job_graph_add_internal(JobGraph* self, JobSrc&& src, UTF8StringView const& name, JobBase* callable, usize callable_size_bytes, usize callable_alignment, Slice<JobHandle const> const& dependencies, Slice<JobResource const> const& resources);

JobHandle job_graph_add_wait(JobGraph* self, JobSrc&& src, UTF8StringView const& name, Slice<JobHandle const> const& dependencies, JobWaitable waitable);

JobHandle job_graph_add_frame_end(JobGraph* self, JobSrc&& src, UTF8StringView const& name, Slice<JobHandle const> const& dependencies);

// JobHandle job_graph_add_frame_end(JobGraph* self);

void job_graph_start(JobGraph* self);

void job_graph_verify_active_policy(u64 range_start, u64 range_end, Slice<ResourceAccessType const>&& access_types);

template <typename T>
void job_graph_verify_active_policy(T const* ptr, Slice<ResourceAccessType const>&& access_types)
{
	u64 const range_start = reinterpret_cast<u64>(ptr);
	job_graph_verify_active_policy(range_start, range_start + sizeof(T), std::move(access_types));
}

FrameAllocator& job_graph_get_frame_allocator(JobGraph* self);

JobWaitable job_graph_create_waitable(JobGraph* self, PlatformWaitHandle const& waitable);