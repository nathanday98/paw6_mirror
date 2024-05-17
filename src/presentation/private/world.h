#pragma once

struct ArenaAllocator;
struct PagePool;

void tempLoadWorld(const char* path, ArenaAllocator& persistent_allocator, PagePool& page_pool);