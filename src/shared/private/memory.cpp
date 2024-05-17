#include <shared/memory.h>

#include <cstring>

#define PAW_TEST_MODULE_NAME memory

bool memEqual(void const* a, void const* b, usize size_in_bytes)
{
	return std::memcmp(a, b, size_in_bytes) == 0;
}

void memCopy(void const* source, void* destination, usize size)
{
	std::memcpy(destination, source, size);
}

#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME memory

PAW_TEST(calcAlignmentOffset)
{
	for (usize i = 0; i < 255; i++)
	{
		PAW_TEST_EXPECT_EQUAL(usize, calcAlignmentOffset((byte*)i, 1), 0);
	}
	auto const test_alignment = [&](usize max, usize alignment)
	{
		for (usize i = 0; i < max; i++)
		{
			usize const d = i % alignment;
			PAW_TEST_EXPECT_EQUAL(usize, calcAlignmentOffset((byte*)i, alignment), d == 0 ? 0 : alignment - d);
		}
	};

	test_alignment(255, 4);
	test_alignment(255, 8);
	test_alignment(255, 16);
	test_alignment(255, 32);
	test_alignment(255, 64);
	test_alignment(255, 128);
}
