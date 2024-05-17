#pragma once

#include <shared/std.h>
#include <shared/string.h>

struct ArenaAllocator;
struct Platform;
struct PlatformWaitHandle;
struct UTF8StringView;
template <typename T>
struct Slice;

typedef void JobQueueFunc(void* data);

struct JobQueueDecl
{
	UTF8StringView name;
	void* data;
	JobQueueFunc* func;
};

void job_queue_init(Platform& platform, ArenaAllocator& persistent_allocator);
void job_queue_start();
void job_queue_deinit();
void job_queue_schedule(const Slice<const JobQueueDecl>&& jobs);
u32 job_queue_push_wait_job(const PlatformWaitHandle& wait_handle, const JobQueueDecl&& decl);
// Returns true if switch from idle to load happened, returns false if triggered
bool job_queue_load_wait_if_idle(u32 wait_index);
void job_queue_set_wait_to_idle(u32 wait_index);
usize job_queue_get_worker_count();
u32 job_queue_get_worker_index();