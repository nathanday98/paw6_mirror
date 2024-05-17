#pragma once

#include <shared/std.h>
#include <shared/slice.h>
#include <shared/slice2d.h>

#include <new>

struct Allocator
{
public:
	template <typename T>
	Slice<T> alloc(usize count)
	{
		usize const size = count * sizeof(T);
		Slice<byte> allocation = alloc_bytes(size, alignof(T));
		return {reinterpret_cast<T*>(allocation.ptr), allocation.size / sizeof(T)};
	}

	template <typename T>
	Slice2D<T> alloc_2d(usize row_count, usize column_count)
	{
		usize const count = row_count * column_count;
		usize const size = count * sizeof(T);
		Slice<byte> allocation = alloc_bytes(size, alignof(T));
		return {reinterpret_cast<T*>(allocation.ptr), allocation.size / (column_count * sizeof(T)), column_count};
	}

	template <typename T>
	void free(Slice<T> mem)
	{
		free_bytes({reinterpret_cast<byte*>(mem.ptr), mem.size * sizeof(T)}, alignof(T));
	}

	template <typename T>
	T& alloc_and_construct_single()
	{
		T& result = alloc<T>(1)[0];
		new (&result) T{};
		return result;
	}

	virtual ~Allocator()
	{
	}

	virtual Slice<byte> alloc_bytes(usize size, usize alignment) = 0;
	virtual void free_bytes(Slice<byte> mem, usize alignment) = 0;

#if PAW_TESTS
	static void tests();
#endif
};