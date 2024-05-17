#include "arena_allocator.h"

#include <shared/memory.h>
#include <shared/log.h>

#include "page_pool.h"

struct ArenaAllocator::PageHeader
{
	PageHeader* prev;
	usize used;

	byte* getCurrentPtr() const
	{
		return ((byte*)this) + used;
	}
};

void ArenaAllocator::init(PagePool* in_page_pool, UTF8StringView const& in_name)
{
	PAW_ASSERT(in_page_pool->page_size > sizeof(PageHeader));
	page_pool = in_page_pool;
	page_size = in_page_pool->page_size;
	current_page = nullptr;
	name = in_name;
}

void ArenaAllocator::reset()
{
	PageHeader* page = current_page;
	while (page)
	{
		PageHeader* prev_page = page->prev;
		page_pool->free(reinterpret_cast<byte*>(page));
		page = prev_page;
	}

	current_page = nullptr;
}

ArenaMarker ArenaAllocator::get_marker() const
{

	return current_page != NULL ? (ArenaMarker)(current_page->getCurrentPtr()) : 0;
}

void ArenaAllocator::reset_to_marker(ArenaMarker marker)
{

	if (marker == 0)
	{
		reset();
		return;
	}

	while (current_page)
	{
		const u64 page_start = reinterpret_cast<u64>(current_page);
		const u64 page_end = page_start + current_page->used;
		if (marker >= page_start && marker <= page_end)
		{
			current_page->used = marker - page_start;
			return;
		}

		PageHeader* old_page = current_page;
		current_page = old_page->prev;
		page_pool->free(reinterpret_cast<byte*>(old_page));
	}

	PAW_UNREACHABLE;
}

usize ArenaAllocator::get_page_size()
{
	return page_size - sizeof(PageHeader);
}

Slice<byte> ArenaAllocator::alloc_bytes(usize size, usize alignment)
{
	PAW_ASSERT(size < page_size - sizeof(PageHeader));

	bool space_available = false;

	byte* start_ptr = nullptr;
	usize alignment_offset = 0;

	if (current_page)
	{
		const usize remaining = page_size - current_page->used - sizeof(PageHeader);
		start_ptr = current_page->getCurrentPtr();
		alignment_offset = calcAlignmentOffset(start_ptr, alignment);

		space_available = size + alignment_offset <= remaining;
	}

	if (!space_available || current_page == nullptr)
	{
		byte* mem = page_pool->alloc();
		PageHeader* new_page = reinterpret_cast<PageHeader*>(mem);
		new_page->prev = current_page;
		current_page = new_page;
		current_page->used = sizeof(PageHeader);

		start_ptr = current_page->getCurrentPtr();
		alignment_offset = calcAlignmentOffset(start_ptr, alignment);
	}

	PAW_ASSERT(start_ptr);

	current_page->used += size + alignment_offset;

	return {start_ptr + alignment_offset, size};
}

void ArenaAllocator::free_bytes(Slice<byte> /*mem*/, usize /*alignment*/)
{
	PAW_UNREACHABLE;
}

#if PAW_TESTS
#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME ArenaAllocator

// PAW_TEST(ArenaAlloc)
//{
//	 ArenaAllocator allocator{};
//	 allocator.init(&testing::getAllocator(), sizeof(ArenaAllocator::PageHeader) + 4);
//	 Slice<byte> a0 = allocator.alloc<byte>(3);
//	 testing::expect(a0.ptr != nullptr);
//	 testing::expect(a0.size = 3);
//	 Slice<byte> a1 = allocator.alloc<byte>(1);
//	 testing::expect(a1.size = 1);
//	 testing::expect(reinterpret_cast<usize>(a1.ptr) - reinterpret_cast<usize>(a0.ptr) == 3);
//	 Slice<byte> a2 = allocator.alloc<byte>(3);
//	 testing::expect(a2.size = 3);
//	 testing::expectNot(reinterpret_cast<usize>(a2.ptr) - reinterpret_cast<usize>(a1.ptr) == 3);
//
//	 allocator.reset();
// }
//
// PAW_TEST(ArenaFree)
//{
//	 ArenaAllocator allocator{};
//	 allocator.init(&testing::getAllocator(), sizeof(ArenaAllocator::PageHeader) + 4);
//	 Slice<byte> a0 = allocator.alloc<byte>(3);
//	(void)a0;
//	 Slice<byte> a1 = allocator.alloc<byte>(1);
//	 Slice<byte> a2 = allocator.alloc<byte>(3);
//
//	 allocator.free(a2);
//	 allocator.free(a1);
//	// allocator.free(a0);
//
//	 allocator.reset();
// }

#endif
