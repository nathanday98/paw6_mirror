#include <shared/slice2d.h>

void slice_2d_test_hack()
{
}

#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME slice_2d

PAW_TEST(Slice2D)
{
	Slice2D<u32> slice{};
	PAW_TEST_EXPECT_EQUAL(usize, slice.calc_total_size_in_bytes(), 0);

	u32 const arr[2][4]{{1, 2, 3, 4}, {5, 6, 7, 8}};
	Slice2D<u32 const> arr_slice{&arr[0][0], 2, 4};
	PAW_TEST_EXPECT_EQUAL(usize, arr_slice.calc_total_size_in_bytes(), 4 * 2 * 4);
	PAW_TEST_EXPECT_EQUAL(usize, arr_slice.calc_total_size(), 4 * 2);
	Slice<u32 const> const arr_row = arr_slice[1];
	PAW_TEST_EXPECT_EQUAL(u32, arr_row[0], 5);
	PAW_TEST_EXPECT_EQUAL(u32, arr_row[1], 6);
	PAW_TEST_EXPECT_EQUAL(u32, arr_row[2], 7);
	PAW_TEST_EXPECT_EQUAL(usize, arr_row.calc_total_size_in_bytes(), 4 * 4);
}
