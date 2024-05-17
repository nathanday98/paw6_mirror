#pragma once

#include <shared/std.h>
#include <shared/math.h>

#include "input.h"

struct PagePool;
struct ArenaAllocator;
struct JobGraph;
struct Platform;
struct GameStartupArgs;
struct DebugDrawState;

struct GameEventData
{
	GameInputState input;
	bool quit_requested;
	bool focus_changed;
	bool focused;
};

struct RendererFrameData
{
	Mat4 camera_mat;
	Vec3 camera_position;
	const DebugDrawState* game_state_debug_draw_state;
	const DebugDrawState* debug_draw_state;
};

struct GameEventDataSwicher
{
	PlatformMutex mutex;
	GameEventData render_write_data;

	void init(Platform& platform);

	void deinit(Platform& platform);

	void copy_out_and_reset(GameEventData& out_data, Platform& platform);

	GameEventData& lock_for_write(Platform& platform);

	void unlock_for_write(Platform& platform);
};

struct RendererFrameDataStore
{
	RendererFrameData game_write;
	usize game_write_index;
	RendererFrameData render_read;
	usize render_read_index;
	PlatformMutex mutex;

	void init(Platform& platform);
	void deinit(Platform& platform);

	RendererFrameData& lock_game_for_write(Platform& platform);
	void unlock_game_for_submit(Platform& platform);
	const RendererFrameData& copy_for_read(Platform& platform);
};
JobGraph* client_graph_init(Platform& platform, PagePool& page_pool, PagePool& debug_page_pool, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, const GameStartupArgs& startup_args, GameEventDataSwicher& game_event_data_switch, RendererFrameDataStore& renderer_frame_data_store, void* graphics_adapter, void* graphics_factory);
