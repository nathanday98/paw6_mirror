#pragma once

#include <shared/allocator.h>
#include <shared/string.h>

typedef u64 ArenaMarker;

struct PagePool;
struct ArenaPage;
struct UTF8StringView;

struct ArenaAllocator final : public Allocator
{
	void init(PagePool* page_pool, const UTF8StringView& name);
	void reset();
	ArenaMarker get_marker() const;
	void reset_to_marker(ArenaMarker marker);

	usize get_page_size();

	virtual Slice<byte> alloc_bytes(usize size, usize alignment) override;
	virtual void free_bytes(Slice<byte> mem, usize alignment) override;

#if PAW_TESTS
	static void tests();
#endif

	struct PageHeader;

	PagePool* page_pool;
	PageHeader* current_page;
	usize page_size;
	UTF8StringView name;
};