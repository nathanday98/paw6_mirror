#pragma once

#include <shared/memory.h>
#include <shared/string.h>

#include "../containers/locking_free_list.h"

struct Platform;

struct PagePool
{
	void init(Platform& platform, usize page_count, const UTF8StringView& name);
	void deinit();

	byte* alloc();
	void free(byte* ptr);

	static constexpr usize page_size = megabytes(2);

	struct Page
	{
		byte mem[page_size];
	};
	LockingFreeList<Page> pages;
	UTF8StringView name;
};