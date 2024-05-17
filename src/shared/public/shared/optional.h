#pragma once

#include <shared/assert.h>

struct OptionalNoneType
{
};

template <typename T>
struct [[nodiscard]] Optional
{
	Optional() : has_value(false)
	{
	}

	Optional(T&& value) : value(value), has_value(true)
	{
	}

	Optional(const OptionalNoneType&) : has_value(false)
	{
	}

	Optional(const T& value) : value(value), has_value(true)
	{
	}

	void operator=(const T& in_value)
	{
		value = in_value;
		has_value = true;
	}

	void operator=(const OptionalNoneType&)
	{
		has_value = false;
	}

	T& getValue()
	{
		PAW_ASSERT(has_value);
		return value;
	}

	const T& getValue() const
	{
		PAW_ASSERT(has_value);
		return value;
	}

	bool hasValue() const
	{
		return has_value;
	}

	T orDefault(T default_value) const
	{
		return has_value ? value : default_value;
	}

private:
	T value;
	bool has_value = false;
};

namespace optional
{
	inline constexpr OptionalNoneType none{};

#if PAW_TESTS
	void tests();
#endif
}