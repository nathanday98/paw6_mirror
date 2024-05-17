#pragma once

#pragma once

#include <shared/std.h>
#include <shared/assert.h>
#include <shared/slice.h>

#include <initializer_list>

template <typename T>
struct Slice2D
{
	Slice2D();
	Slice2D(T* ptr, usize row_count, usize column_count);
	// Should only be used in when const ptr. I tried using enable_if, but it gave a less clear error

	// #TODO: Maybe in non-debug builds this should return a pointer
	Slice<T> operator[](usize row_index);
	Slice<T const> operator[](usize row_index) const;

	usize calc_total_size() const;
	usize calc_total_size_in_bytes() const;
	Slice2D<byte> to_byte_slice() const;
	Slice2D<byte const> to_const_byte_slice() const;
	Slice2D<T const> to_const_slice() const;

	T* ptr;
	usize row_count;
	usize column_count;
};

template <typename T>
Slice2D<T>::Slice2D()
	: ptr(nullptr), row_count(0), column_count(0)
{
}

template <typename T>
Slice2D<T>::Slice2D(T* ptr, usize row_count, usize column_count)
	: ptr(ptr), row_count(row_count), column_count(column_count)
{
}

template <typename T>
Slice<T> Slice2D<T>::operator[](usize row_index)
{
	PAW_ASSERT(row_index < row_count);
	return Slice<T>{ptr + (row_index * column_count), column_count};
}

template <typename T>
Slice<T const> Slice2D<T>::operator[](usize row_index) const
{
	PAW_ASSERT(row_index < row_count);
	return Slice<T const>{ptr + (row_index * column_count), column_count};
}

template <typename T>
usize Slice2D<T>::calc_total_size() const
{
	return row_count * column_count;
}

template <typename T>
usize Slice2D<T>::calc_total_size_in_bytes() const
{
	return row_count * column_count * sizeof(T);
}

template <typename T>
Slice2D<byte> Slice2D<T>::to_byte_slice() const
{
	return {reinterpret_cast<byte*>(ptr), calc_total_size_in_bytes()};
}

template <typename T>
Slice2D<byte const> Slice2D<T>::to_const_byte_slice() const
{
	return {reinterpret_cast<byte const*>(ptr), calc_total_size_in_bytes()};
}

template <typename T>
Slice2D<T const> Slice2D<T>::to_const_slice() const
{
	return {const_cast<T const*>(ptr), row_count, column_count};
}
