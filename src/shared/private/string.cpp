#include <shared/string.h>
#include <shared/string_id.inl>

#include <cstring>

#include <shared/assert.h>
#include <shared/hash.h>
#include <shared/memory.h>

bool UTF8StringView_equals(UTF8StringView const* string, UTF8StringView const* rhs)
{
	if (string->size_bytes != rhs->size_bytes)
	{
		return false;
	}
	return memEqual(string->ptr, rhs->ptr, string->size_bytes);
}

bool UTF8StringView_beginsWith(UTF8StringView const* string, UTF8StringView const* test)
{
	if (string->size_bytes < test->size_bytes)
	{
		return false;
	}
	return memEqual(string->ptr, test->ptr, test->size_bytes);
}

void UTF8StringBuilder_append(UTF8StringBuilder* builder, UTF8StringView const* str)
{
	PAW_ASSERT(builder->write_index + str->size_bytes <= builder->out_buffer_size_bytes);
	memCopy(str->ptr, builder->out_buffer + builder->write_index, str->size_bytes);
	builder->write_index += str->size_bytes;
}

void UTF8StringBuilder_appendFormattedVA(UTF8StringBuilder* builder, char const* format, va_list args)
{
	const UTF8StringView printed =
		string_formatVA(builder->out_buffer + builder->write_index, builder->out_buffer_size_bytes - builder->write_index, format, args);
	builder->write_index += printed.size_bytes;
	PAW_ASSERT(builder->write_index <= builder->out_buffer_size_bytes);
}

void UTF8StringBuilder_appendFormatted(UTF8StringBuilder* builder, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	UTF8StringBuilder_appendFormattedVA(builder, format, args);
	va_end(args);
}

void UTF8StringBuilder_appendNullTermination(UTF8StringBuilder* builder)
{
	PAW_ASSERT(builder->write_index < builder->out_buffer_size_bytes);
	builder->out_buffer[builder->write_index] = 0;
	builder->write_index++;
}

static void customFormatCallback(
	char* out_buffer, size_t out_size, char const* type, size_t type_size, void const* data);

#define STBSP_CUSTOM_FORMAT_CALLBACK customFormatCallback
#define STB_SPRINTF_IMPLEMENTATION
#pragma warning(push, 0)
#include <stb_sprintf.h>
#pragma warning(pop)

static constexpr usize max_formatters = 64;
static struct
{
	StringID formatter_types[max_formatters];
	FormatterFunc* formatters[max_formatters];
	usize formatter_count = 0;
} g_data{};

void string_registerFormatter(StringID type, FormatterFunc* formatter)
{
	PAW_ASSERT(g_data.formatter_count < max_formatters);
	g_data.formatter_types[g_data.formatter_count] = type;
	g_data.formatters[g_data.formatter_count] = formatter;
	g_data.formatter_count++;
}

static void formatConstString(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	UTF8StringView const* slice = static_cast<UTF8StringView const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "%.*s", static_cast<int>(slice->size_bytes), slice->ptr);
}

void string_registerFormatters()
{
	string_registerFormatter(PAW_STRING_ID("str"), &formatConstString);
}

static void customFormatCallback(
	char* out_buffer, size_t out_size, char const* type, size_t type_size, void const* data)
{
	const StringID type_hash = hashCStringToStringID(type, type_size);
	for (usize i = 0; i < g_data.formatter_count; i++)
	{
		if (g_data.formatter_types[i] == type_hash)
		{
			g_data.formatters[i]((byte* const)out_buffer, out_size, data);
			return;
		}
	}

	PAW_UNREACHABLE;
}

UTF8StringView string_format(byte* out_buffer, usize out_buffer_size_bytes, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	const UTF8StringView result = string_formatVA(out_buffer, out_buffer_size_bytes, format, args);
	va_end(args);
	return result;
}

UTF8StringView string_formatNullTerminated(byte* out_buffer, usize out_buffer_size_bytes, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	UTF8StringView result = string_formatVA(out_buffer, out_buffer_size_bytes, format, args);
	va_end(args);
	PAW_ASSERT(out_buffer_size_bytes >= result.size_bytes + 1);
	out_buffer[result.size_bytes] = 0;
	result.null_terminated = true;
	return result;
}

UTF8StringView string_formatVA(byte* out_buffer, usize out_buffer_size_bytes, char const* format, va_list args)
{
	int num_written = stbsp_vsnprintf((char* const)out_buffer, (int)out_buffer_size_bytes, format, args);
	return {reinterpret_cast<char const*>(out_buffer), static_cast<usize>(num_written), false};
}

bool string_cStringEqual(char const* a, char const* b)
{
	return ::std::strcmp(a, b) == 0;
}

usize string_cStringLength(char const* str)
{
	return ::std::strlen(str);
}

#ifdef PAW_TESTS

#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME string

PAW_TEST(constCreate)
{
	UTF8StringView str = PAW_STR("test", 4);
	PAW_TEST_EXPECT_EQUAL(bool, str.size_bytes == 4, true);
}

PAW_TEST(constEqual)
{
	UTF8StringView a = PAW_STR("test", 4);
	UTF8StringView b = PAW_STR("test", 4);
	PAW_TEST_EXPECT_EQUAL(bool, UTF8StringView_equals(&a, &b), true);

	UTF8StringView c = PAW_STR("test1", 5);
	UTF8StringView d = PAW_STR("test", 4);
	PAW_TEST_EXPECT_EQUAL(bool, UTF8StringView_equals(&c, &d), false);
}

PAW_TEST(formatTest)
{
	UTF8StringView temp = PAW_STR("Test String View", 16);
	byte buffer[64];
	string_format(buffer, PAW_ARRAY_COUNT(buffer), "%p{str}", &temp);
	PAW_TEST_EXPECT_EQUAL(bool, memEqual(buffer, temp.ptr, temp.size_bytes), true);
}
#endif