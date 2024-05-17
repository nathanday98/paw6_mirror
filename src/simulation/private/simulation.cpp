#include <simulation/simulation.h>

SharedAssertFunc* g_shared_assert_func;

void simulation_init(SharedAssertFunc* assert_func)
{
	g_shared_assert_func = assert_func;

	//PAW_ASSERT(false);
}