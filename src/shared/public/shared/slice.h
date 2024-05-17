#pragma once

#include <shared/std.h>
#include <shared/assert.h>

#include <initializer_list>

template <typename T>
struct Slice
{
	Slice();
	Slice(T* ptr, usize size);
	Slice(T* start, T* end);
	// Should only be used in when const ptr. I tried using enable_if, but it gave a less clear error
	Slice(std::initializer_list<T> list);

	T& operator[](usize index);
	T& operator[](usize index) const;

	usize calc_total_size_in_bytes() const;
	Slice<byte> to_byte_slice() const;
	Slice<byte const> to_const_byte_slice() const;
	Slice<T const> to_const_slice() const;

	struct Iterator;
	Iterator begin();
	Iterator end();

	Iterator begin() const;
	Iterator end() const;

	Slice<T> sub_slice(usize start, usize count) const;
	Slice<T const> const_sub_slice(usize start, usize count) const;

	T* ptr;
	usize size;
};

template <typename T>
Slice<T>::Slice()
	: ptr(nullptr), size(0)
{
}

template <typename T>
Slice<T>::Slice(T* ptr, usize size)
	: ptr(ptr), size(size)
{
}

template <typename T>
Slice<T>::Slice(T* start, T* end)
	: ptr(start), size((reinterpret_cast<usize>(end) - reinterpret_cast<usize>(start)) / sizeof(T))
{
}

// Should only be used in when const ptr. I tried using enable_if, but it gave a less clear error
template <typename T>
Slice<T>::Slice(std::initializer_list<T> list)
	: Slice(list.begin(), list.end())
{
}

template <typename T>
T& Slice<T>::operator[](usize index)
{
	PAW_ASSERT(index < size);
	return ptr[index];
}

template <typename T>
T& Slice<T>::operator[](usize index) const
{
	PAW_ASSERT(index < size);
	return ptr[index];
}

template <typename T>
usize Slice<T>::calc_total_size_in_bytes() const
{
	return size * sizeof(T);
}

template <typename T>
Slice<byte> Slice<T>::to_byte_slice() const
{
	return {reinterpret_cast<byte*>(ptr), calc_total_size_in_bytes()};
}

template <typename T>
Slice<byte const> Slice<T>::to_const_byte_slice() const
{
	return {reinterpret_cast<byte const*>(ptr), calc_total_size_in_bytes()};
}

template <typename T>
Slice<T const> Slice<T>::to_const_slice() const
{
	return {const_cast<T const*>(ptr), size};
}

template <typename T>
struct Slice<T>::Iterator
{
	Slice<T>::Iterator& operator++()
	{
		ptr++;
		return *this;
	}

	Slice<T>::Iterator& operator--()
	{
		ptr--;
		return *this;
	}

	T& operator*()
	{
		return *ptr;
	}

	bool operator==(Slice<T>::Iterator const& rhs) const
	{
		return ptr == rhs.ptr;
	}

	bool operator!=(Slice<T>::Iterator const& rhs) const
	{
		return ptr != rhs.ptr;
	}

	T* ptr;
};

template <typename T>
typename Slice<T>::Iterator Slice<T>::begin()
{
	return Slice<T>::Iterator{.ptr = ptr};
}

template <typename T>
typename Slice<T>::Iterator Slice<T>::end()
{
	return Slice<T>::Iterator{.ptr = ptr + size};
}

template <typename T>
typename Slice<T>::Iterator Slice<T>::begin() const
{
	return Slice<T>::Iterator{.ptr = ptr};
}

template <typename T>
typename Slice<T>::Iterator Slice<T>::end() const
{
	return Slice<T>::Iterator{.ptr = ptr + size};
}

template <typename T>
inline Slice<T> Slice<T>::sub_slice(usize start, usize count) const
{
	PAW_ASSERT(start + count <= size);
	return {ptr + start, count};
}

template <typename T>
inline Slice<T const> Slice<T>::const_sub_slice(usize start, usize count) const
{
	PAW_ASSERT(start + count <= size);
	return {ptr + start, count};
}
