#pragma once

#include <shared/std.h>

#ifdef TRACY_ENABLE
PAW_DISABLE_ALL_WARNINGS_BEGIN
#include <tracy/Tracy.hpp>
PAW_DISABLE_ALL_WARNINGS_END
#endif

#if defined(USE_OPTICK) && USE_OPTICK
#include <optick.h>
#endif

struct UTF8StringView;

struct GameStartupArgs;

void profilerInit(const GameStartupArgs& startup_args);
void profilerInitThread(const char* name);
void profilerLaunch();

#ifdef TRACY_ENABLE
#define PAW_PROFILER_SCOPE(name) ZoneScopedN(name)
#define PAW_PROFILER_SCOPE_DYNAMIC(name) \
	ZoneScoped;                          \
	ZoneName(name.ptr, name.size_bytes);
#define PAW_PROFILER_FUNC() ZoneScoped
#define PAW_PROFILER_FRAME_END(name) FrameMarkNamed(name)
#define PAW_PROFILER_THREAD(name) profilerInitThread(name)
#define PAW_PROFILER_GPU_ZONE(command_buffer, name)
#elif defined(USE_OPTICK) && USE_OPTICK
#define PAW_PROFILER_SCOPE(name) OPTICK_EVENT(name)
#define PAW_PROFILER_SCOPE_DYNAMIC(name) OPTICK_EVENT_DYNAMIC(name)
#define PAW_PROFILER_FUNC() OPTICK_EVENT()
#define PAW_PROFILER_FRAME_END() OPTICK_FRAME("Main")
#define PAW_PROFILER_THREAD(name) OPTICK_THREAD(name)
#else
#define PAW_PROFILER_SCOPE(name)
#define PAW_PROFILER_SCOPE_DYNAMIC(name)
#define PAW_PROFILER_FUNC()
#define PAW_PROFILER_FRAME_END()
#define PAW_PROFILER_THREAD(name)
#endif