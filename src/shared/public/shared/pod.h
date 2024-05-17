#pragma once

#include <type_traits>

#define PAW_ASSERT_IS_POD(type) static_assert(std::is_trivial<type>::value && std::is_standard_layout<type>::value)