#include "frame_allocator.h"

#include <shared/memory.h>
#include <shared/log.h>

#include <new>

#include "../job_queue.h"
#include "../profiler.h"
#include "page_pool.h"
#include "arena_allocator.h"

void FrameAllocator::init(Allocator& persistent_allocator, PagePool& page_pool)
{
	arenas = persistent_allocator.alloc<ArenaAllocator>(job_queue_get_worker_count());

	for (ArenaAllocator& arena : arenas)
	{
		arena.init(&page_pool, "Frame Allocator"_str);
	}
}

void FrameAllocator::reset()
{
	PAW_PROFILER_FUNC();
	for (ArenaAllocator& arena : arenas)
	{
		arena.reset();
	}
}

Slice<byte> FrameAllocator::alloc_bytes(usize size, usize alignment)
{
	return arenas[job_queue_get_worker_index()].alloc_bytes(size, alignment);
}

void FrameAllocator::free_bytes(Slice<byte> mem, usize alignment)
{
	// #TODO: get job worer index
	arenas[job_queue_get_worker_index()].free_bytes(mem, alignment);
}