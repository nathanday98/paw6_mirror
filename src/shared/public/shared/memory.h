#pragma once

#include <shared/std.h>
#include <shared/assert.h>

constexpr inline usize kilobytes(usize num)
{
	return num * 1024LL;
}

constexpr inline usize megabytes(usize num)
{
	return kilobytes(num) * 1024LL;
}

constexpr inline usize gigabytes(usize num)
{
	return megabytes(num) * 1024LL;
}

bool memEqual(void const* a, void const* b, usize size_in_bytes);
void memCopy(void const* source, void* destination, usize size);

inline usize calcAlignmentOffset(byte* ptr, usize alignment)
{
	u64 alignment_offset = 0;
	u64 alignment_mask = alignment - 1;
	u64 result_ptr = (u64)ptr;
	u64 mask = result_ptr & alignment_mask;
	alignment_offset = (mask > 0) * (alignment - (result_ptr & alignment_mask));
	return alignment_offset;
}

inline byte* alignPointerForward(byte* ptr, usize alignment)
{
	PAW_ASSERT((alignment & (alignment - 1)) == 0); // check if power of 2

	return ptr + calcAlignmentOffset(ptr, alignment);
}

inline byte* alignPointerBackward(byte* ptr, usize alignment)
{
	PAW_ASSERT((alignment & (alignment - 1)) == 0); // check if power of 2

	u64 alignment_mask = alignment - 1;
	u64 result_ptr = (u64)ptr;

	return ptr - (result_ptr & alignment_mask);
}
