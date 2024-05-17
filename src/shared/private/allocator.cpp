#include <shared/allocator.h>

#include <shared/assert.h>
#include <shared/memory.h>

#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME allocator

PAW_TEST(AlignPointerForwardTests)
{
	byte* ptr0 = reinterpret_cast<byte*>(1);
	ptr0 = alignPointerForward(ptr0, 4);
	PAW_TEST_EXPECT_EQUAL(byte*, ptr0, reinterpret_cast<byte*>(4));

	byte* ptr1 = reinterpret_cast<byte*>(22);
	ptr1 = alignPointerForward(ptr1, 8);
	PAW_TEST_EXPECT_EQUAL(byte*, ptr1, reinterpret_cast<byte*>(24));
}

PAW_TEST(AlignPointerBackwardTests)
{
	byte* ptr0 = reinterpret_cast<byte*>(5);
	ptr0 = alignPointerBackward(ptr0, 4);
	PAW_TEST_EXPECT_EQUAL(byte*, ptr0, reinterpret_cast<byte*>(4));

	byte* ptr1 = reinterpret_cast<byte*>(22);
	ptr1 = alignPointerBackward(ptr1, 8);
	PAW_TEST_EXPECT_EQUAL(byte*, ptr1, reinterpret_cast<byte*>(16));
}

PAW_TEST(Align)
{
	PAW_TEST_EXPECT_EQUAL(int, 1, 1);
}