#pragma once

#include <shared/std.h>

struct UTF8StringView;

u64 murmur64(const byte* data_ptr, usize data_size, u64 seed);
u64 murmur64(const UTF8StringView& str, u64 seed);