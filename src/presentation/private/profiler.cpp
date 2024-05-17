#include "profiler.h"

#include <shared/log.h>

#include "startup_args.h"

void profilerInit(const GameStartupArgs& startup_args)
{
	if (startup_args.profiler_wait_for_connection)
	{
#ifdef TRACY_ENABLE
		PAW_LOG_INFO("Waiting for profiler to connect...");
		while (!TracyIsConnected)
		{
		}
#endif
	}
}

void profilerInitThread(const char* name)
{
#ifdef TRACY_ENABLE
	tracy::SetThreadName(name);
#else
	PAW_UNUSED_ARG(name);
#endif
}

void profilerLaunch()
{
	system("start utils/tracy/tracy.exe");
}