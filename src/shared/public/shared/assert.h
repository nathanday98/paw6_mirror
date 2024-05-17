#pragma once

#include <shared/std.h>

typedef void SharedAssertFunc(char const* file, u32 line, char const* expression);
extern SharedAssertFunc* g_shared_assert_func;

#ifdef PAW_DEBUG
#define PAW_ASSERT(expression)                                     \
	if (!(expression))                                             \
	{                                                              \
		if (true)                                                  \
		{                                                          \
			g_shared_assert_func(__FILE__, __LINE__, #expression); \
			__debugbreak();                                        \
		}                                                          \
		else                                                       \
		{                                                          \
		}                                                          \
	}                                                              \
	else                                                           \
	{                                                              \
	}

#define PAW_ASSERT_UNUSED(var)
#define PAW_UNREACHABLE \
	do                  \
	{                   \
		__debugbreak(); \
	} while (false);
#else
#define PAW_ASSERT(expression)
#define PAW_ASSERT_UNUSED(var) (void)var;
#define PAW_UNREACHABLE
#endif