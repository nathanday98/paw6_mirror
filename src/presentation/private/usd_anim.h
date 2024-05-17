#pragma once

struct ArenaAllocator;
struct AnimDB;

void tempLoadAnim(const char* path, ArenaAllocator& persistent_allocator, AnimDB& out_anim_db);