#pragma once

#include "../../../shared/public/shared/std.h"

struct TestCase;
typedef void TestCastFunc(TestCase const& context, bool& out_passed);

struct TestCase
{
	char const* project;
	char const* file;
	char const* module;
	char const* name;
	TestCastFunc* function;
	int line;
};

template <typename T>
void failTestCaseEqual(TestCase const& context, char const* value_str, T const& value, T const& expected, bool& out_passed);

#pragma section("tests$a", read)
#pragma section("tests$b", read)
#pragma section("tests$c", read)

#define PAW_TEST_CONCAT_EX(x, y) x##y
#define PAW_TEST_CONCAT(x, y) PAW_TEST_CONCAT_EX(x, y)

#define PAW_TEST_FUNC_NAME(name) PAW_TEST_CONCAT(PAW_TEST_CONCAT(name, __LINE__), _func)

#define PAW_TEST_STRINGIFY_EX(x) #x
#define PAW_TEST_STRINGIFY(x) PAW_TEST_STRINGIFY_EX(x)

#define PAW_TEST_VAR_NAME(name) PAW_CONCAT(g_, PAW_TEST_CONCAT(PAW_TEST_CONCAT(PAW_TEST_MODULE_NAME, name), __LINE__))

#define PAW_TEST(name)                                                                                              \
	static void PAW_TEST_FUNC_NAME(name)(TestCase const& context, bool& out_passed);                                \
	static TestCase PAW_TEST_VAR_NAME(name){                                                                        \
		PAW_TEST_STRINGIFY(PAW_TEST_PROJECT_NAME),                                                                  \
		__FILE__,                                                                                                   \
		PAW_TEST_STRINGIFY(PAW_TEST_MODULE_NAME),                                                                   \
		#name,                                                                                                      \
		&PAW_TEST_FUNC_NAME(name),                                                                                  \
		__LINE__,                                                                                                   \
	};                                                                                                              \
	__declspec(allocate("tests$b")) TestCase* PAW_CONCAT(PAW_TEST_VAR_NAME(name), _ptr) = &PAW_TEST_VAR_NAME(name); \
	static void PAW_TEST_FUNC_NAME(name)(TestCase const& context, bool& out_passed)

template <typename T>
void test_expect_equal(TestCase const& context, T const& value, T const& expected, bool& out_passed)
{
	if (value != expected)
	{
		failTestCaseEqual(context, "", value, expected, out_passed);
	}
}

#define PAW_TEST_EXPECT_EQUAL(type, value, expected) test_expect_equal<type>(context, value, expected, out_passed)

#define PAW_TEST_EXPECT(bool_expression) test_expect_equal<bool>(context, bool_expression, true, out_passed)

int testingRunAllTests();
int testingListAllTests();

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, u64 const& value, u64 const& expected, bool& out_passed);

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, byte* const& value, byte* const& expected, bool& out_passed);

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, u32 const& value, u32 const& expected, bool& out_passed);

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, int const& value, int const& expected, bool& out_passed);

template <>
void failTestCaseEqual(TestCase const& context, char const* value_str, bool const& value, bool const& expected, bool& out_passed);
