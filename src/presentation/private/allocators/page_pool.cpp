#include "page_pool.h"

#include <shared/log.h>

#include <platform/platform.h>

void PagePool::init(Platform& platform, usize page_count, const UTF8StringView& in_name)
{
	pages.init(page_count, &platform.get_system_allocator(), &platform);
	name = in_name;
}

void PagePool::deinit()
{
	pages.deinit();
}

byte* PagePool::alloc()
{
	Page* page = pages.alloc();
	// PAW_LOG_WARNING("%p{str} Alloc: %llu - New Head: %llu - Pages This: %llu - This: %llu", &name, pages.calcNodeIndex(page), pages.head, (u64)&pages, this);

	return reinterpret_cast<byte*>(page);
}

void PagePool::free(byte* ptr)
{
	Page* page = reinterpret_cast<Page*>(ptr);
	// PAW_LOG_WARNING("%p{str} Free: %llu", &name, pages.calcNodeIndex(page));
	pages.free(page);
}