#pragma once

#include <shared/std.h>

template <typename Func>
struct Defer
{
	Defer(Func&& func, bool trigger = true) : func(func), trigger(trigger)
	{
	}

	~Defer()
	{
		if (trigger)
		{
			func();
		}
	}

	Func func;
	bool trigger;
};
