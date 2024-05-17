#pragma once

#define PAW_UNUSED_ARG(arg) (void)arg

#define PAW_ERROR_ON_PADDING_BEGIN __pragma(warning(push)) __pragma(warning(error : 4820))
#define PAW_ERROR_ON_PADDING_END __pragma(warning(pop))
#define PAW_DISABLE_ALL_WARNINGS_BEGIN __pragma(warning(push, 0))
#define PAW_DISABLE_ALL_WARNINGS_END __pragma(warning(pop))

typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int_least8_t;
typedef short int_least16_t;
typedef int int_least32_t;
typedef long long int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long uint_least64_t;

typedef signed char int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;
typedef long long int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 byte;

typedef float f32;
typedef double f64;

typedef size_t usize;

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
#define PAW_EXTERN_C_BEGIN \
	extern "C"             \
	{

#define PAW_EXTERN_C_END }
#else
#define PAW_EXTERN_C_BEGIN
#define PAW_EXTERN_C_END
#endif

#define PAW_ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define PAW_CONCAT_EX(x, y) x##y
#define PAW_CONCAT(x, y) PAW_CONCAT_EX(x, y)