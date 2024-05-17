#pragma once

#include <shared/allocator.h>
#include <shared/slice.h>

struct ArenaAllocator;
struct PagePool;
struct StringID;

struct FrameAllocator : public Allocator
{
	void init(Allocator& persistent_allocator, PagePool& page_pool);
	void reset();

	virtual Slice<byte> alloc_bytes(usize size, usize) override;
	virtual void free_bytes(Slice<byte> mem, usize alignment) override;

private:
	Slice<ArenaAllocator> arenas;
};