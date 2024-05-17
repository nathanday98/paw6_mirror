#pragma once

#include <stdarg.h>

#include <shared/std.h>
#include <shared/string_id.h>

typedef struct UTF8StringView
{
	const char* ptr;
	usize size_bytes;
	bool null_terminated;
} UTF8StringView;

bool UTF8StringView_equals(const UTF8StringView* string, const UTF8StringView* rhs);
bool UTF8StringView_beginsWith(const UTF8StringView* string, const UTF8StringView* test);

typedef struct UTF8StringBuilder
{
	byte* out_buffer;
	usize out_buffer_size_bytes;
	usize write_index;
} UTF8StringBuilder;

void UTF8StringBuilder_append(UTF8StringBuilder* builder, const UTF8StringView* str);
void UTF8StringBuilder_appendNullTermination(UTF8StringBuilder* builder);
void UTF8StringBuilder_appendFormattedVA(UTF8StringBuilder* builder, const char* format, va_list args);
void UTF8StringBuilder_appendFormatted(UTF8StringBuilder* builder, const char* format, ...);

typedef void FormatterFunc(byte* out_buffer, usize out_buffer_size_bytes, const void* data);
void string_registerFormatter(StringID type, FormatterFunc* formatter);
void string_registerFormatters();
UTF8StringView string_format(byte* out_buffer, usize out_buffer_size_bytes, const char* format, ...);
UTF8StringView string_formatNullTerminated(byte* out_buffer, usize out_buffer_size_bytes, const char* format, ...);
UTF8StringView string_formatVA(byte* out_buffer, usize out_buffer_size_bytes, const char* format, va_list args);
bool string_cStringEqual(const char* a, const char* b);
usize string_cStringLength(const char* str);

#if PAW_TESTS
void string_tests();
#endif

#ifdef __cplusplus
#define PAW_STR(str, len)                                       \
	UTF8StringView                                              \
	{                                                           \
		.ptr = str, .size_bytes = len, .null_terminated = true, \
	}
#else
#define PAW_STR(str, len)                                       \
	(UTF8StringView)                                            \
	{                                                           \
		.ptr = str, .size_bytes = len, .null_terminated = true, \
	}
#endif

constexpr UTF8StringView operator"" _str(const char* str, usize len)
{
	return UTF8StringView{.ptr = str, .size_bytes = len, .null_terminated = true};
}