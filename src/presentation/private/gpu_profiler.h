#pragma once

#include <shared/std.h>

#if 0
PAW_DISABLE_ALL_WARNINGS_BEGIN
#include <TracyVulkan.hpp>
PAW_DISABLE_ALL_WARNINGS_END
#endif

struct GameStartupArgs;

namespace Gpu
{
	void initProfiler(const GameStartupArgs& startup_args);
	void deinitProfiler();
	void collectProfiler(/* VkCommandBuffer command_buffer */);
	void* getProfilerContext();
	bool isProfilerActive();
}

#if 0
#define PAW_GPU_PROFILER_ZONE(name, command_buffer)                                                                    \
	TracyVkNamedZone(                                                                                                  \
		reinterpret_cast<TracyVkCtx>(Gpu::getProfilerContext()),                                                       \
		PAW_CONCAT(___tracy_gpu_zone, __LINE__),                                                                       \
		command_buffer,                                                                                                \
		name,                                                                                                          \
		Gpu::isProfilerActive())
#else
#define PAW_GPU_PROFILER_ZONE(name, command_buffer)
#endif