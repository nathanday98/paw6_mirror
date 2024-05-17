#include "gpu_profiler.h"

#include <shared/assert.h>

#include "startup_args.h"

// static struct
// {
// 	VkCommandBuffer profiler_command_buffer;
// 	PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
// 	PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
// 	TracyVkCtx profiler_context;
// 	bool enabled;
// } g_data;

void Gpu::initProfiler(const GameStartupArgs& /* startup_args */)
{
	// 	g_data.enabled = startup_args.enable_gpu_profiler;
	// 	if (g_data.enabled)
	// 	{

	// 		g_data.vkGetPhysicalDeviceCalibrateableTimeDomainsEXT =
	// 			reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(
	// 				vkGetInstanceProcAddr(Gpu::instance(), "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
	// 		PAW_ASSERT(g_data.vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);

	// 		g_data.vkGetCalibratedTimestampsEXT = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(
	// 			vkGetInstanceProcAddr(Gpu::instance(), "vkGetCalibratedTimestampsEXT"));
	// 		PAW_ASSERT(g_data.vkGetCalibratedTimestampsEXT);

	// 		Gpu::allocCommandBuffers(1, &g_data.profiler_command_buffer);

	// 		g_data.profiler_context = TracyVkContextCalibrated(
	// 			Gpu::physicalDevice(),
	// 			Gpu::device(),
	// 			Gpu::graphicsQueue(),
	// 			g_data.profiler_command_buffer,
	// 			g_data.vkGetPhysicalDeviceCalibrateableTimeDomainsEXT,
	// 			g_data.vkGetCalibratedTimestampsEXT);
	// 	}
}

void Gpu::deinitProfiler()
{
	// if (g_data.enabled)
	// {
	// 	TracyVkDestroy(g_data.profiler_context);
	// }
}

void Gpu::collectProfiler(/* VkCommandBuffer command_buffer */)
{

	// if (g_data.enabled)
	// {
	// 	TracyVkCollect(g_data.profiler_context, command_buffer);
	// }
}

void* Gpu::getProfilerContext()
{
	// return reinterpret_cast<void*>(g_data.profiler_context);
	return nullptr;
}

bool Gpu::isProfilerActive()
{
	return false;
	// return g_data.enabled && TracyIsConnected;
}