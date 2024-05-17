#include <testing/testing.h>

#include <stdint.h>
#include <cstdio>
#include <unordered_map>

#include <Windows.h>

#define COLOR_RED "\033[1;31m"
#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[0;32m"

#define FAIL_TEST_CASE(context, formatter, value_str, value, expected)                                                                                                                                                                                                      \
	std::fprintf(stderr, COLOR_RED "Failed %s:%s:%s" COLOR_RESET "\nFile: %s\nLine: %d\nValues are not equal: %s\nValue: " formatter "\nExpected: " formatter "\n", context.project, context.module, context.name, context.file, context.line, value_str, value, expected); \
	if (IsDebuggerPresent())                                                                                                                                                                                                                                                \
	{                                                                                                                                                                                                                                                                       \
		__debugbreak();                                                                                                                                                                                                                                                     \
	}                                                                                                                                                                                                                                                                       \
	out_passed = false;

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, u64 const& value, u64 const& expected, bool& out_passed)
{
	FAIL_TEST_CASE(context, "%llu", value_str, value, expected);
}

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, byte* const& value, byte* const& expected, bool& out_passed)
{
	FAIL_TEST_CASE(context, "%llu", value_str, reinterpret_cast<u64>(value), reinterpret_cast<u64>(expected));
}

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, u32 const& value, u32 const& expected, bool& out_passed)
{
	FAIL_TEST_CASE(context, "%u", value_str, value, expected);
}

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, int const& value, int const& expected, bool& out_passed)
{
	FAIL_TEST_CASE(context, "%d", value_str, value, expected);
}

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, bool const& value, bool const& expected, bool& out_passed)
{
	FAIL_TEST_CASE(context, "%s", value_str, value ? "true" : "false", expected ? "true" : "false");
}

__declspec(allocate("tests$a")) TestCase* __start_test_section = nullptr;
__declspec(allocate("tests$c")) TestCase* __stop_test_section = nullptr;

int testingListAllTests()
{
	for (TestCase** test_ptr = &__start_test_section + 1; test_ptr < &__stop_test_section; test_ptr++)
	{
		TestCase* test = *test_ptr;
		if (test)
		{
			fprintf(stdout, "%s::%s::%s - %s::%d\n", test->project, test->module, test->name, test->file, test->line);
		}
	}

	return 0;
}

int testingRunAllTests()
{
	size_t failed_count = 0;
	size_t total_count = 0;
	for (TestCase** test_ptr = &__start_test_section + 1; test_ptr < &__stop_test_section; test_ptr++)
	{
		TestCase const* test = *test_ptr;
		if (test)
		{
			bool passed = true;
			test->function(*test, passed);
			failed_count += !passed;
			total_count++;
		}
	}

	fprintf(stdout, "Completed %llu/%llu tests\n", total_count - failed_count, total_count);

	if (failed_count > 0)
	{
		fprintf(stderr, COLOR_RED "Failed %llu/%llu tests\n" COLOR_RESET, failed_count, total_count);
		return -1;
	}
	else
	{
		fprintf(stdout, COLOR_GREEN "All Tests Succeeded!" COLOR_RESET);
	}

	return 0;
}