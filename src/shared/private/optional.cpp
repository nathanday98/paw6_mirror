#include <shared/optional.h>

#if PAW_TESTS
#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME Optional

PAW_TEST(HasValue)
{
	Optional<int> a = 10;
	PAW_TEST_EXPECT(a.hasValue());

	Optional<int> b = {};
	PAW_TEST_EXPECT(!b.hasValue());

	Optional<int> c = {};
	PAW_TEST_EXPECT(c.orDefault(10) == 10);
}

#endif