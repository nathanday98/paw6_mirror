#pragma once

#include <shared/std.h>

struct Hdr
{
	f32 min_luminance;
	f32 max_luminance;
	f32 max_full_frame_luminance;
	f32 white_sdr_scale;
	bool enabled;
};

struct GameStartupArgs
{
	bool profiler_wait_for_connection;
	bool enable_gpu_profiler;
	bool disable_gpu_debug;
	// This should be replaced by taking in sizes
	bool on_laptop;
	u16 port;
	bool editor_mode;

	Hdr hdr;
};
