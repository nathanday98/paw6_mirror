#pragma once

#include <shared/math.h>

#include "game_state.h"
#include "transform.h"
#include "debug_draw.h"
#include "motion_matching.h"

struct ArenaAllocator;
struct Platform;
struct PlatformEvent;
struct GameStartupArgs;
struct FrameAllocator;
struct PagePool;
struct GameInputState;
struct GameEventData;
struct RendererFrameData;

enum class MouseLockedState
{
	Unlocked,
	Locked,
};

struct DebugCameraState
{
	f32 yaw = 0.0f;
	f32 pitch = 0.0f;
	Transform transform{};

	void init();
	void tick(f32 delta_time_seconds, MouseLockedState mouse_locked_state, const GameInputState& input_state);
};

struct GameFrame
{
	GameState state;
	GamepadInput gamepad;
	DebugDrawState debug_draw_state;
	f32 delta_time_seconds;
};

struct GamePlayer
{
	void init(Platform& platform, ArenaAllocator& persistent_allocator, const GameStartupArgs& startup_args, PagePool& page_pool, PagePool& debug_page_pool);
	void deinit();
	void processEvent(Platform& platform, const PlatformEvent& event);
	void tick(Platform& platform, f32 delta_time, FrameAllocator& frame_allocator, const GameEventData& event_data, RendererFrameData& renderer_frame_data);
	void onFocus(Platform& platform, const GameInputState& input_state);
	void onUnfocus(Platform& platform);
	GameFrame& getCurrentGameFrame();
	s32 wrapGameFrameIndex(s32 index);

	static constexpr s32 game_frame_max = 64;
	GameFrame game_frames[game_frame_max];
	s32 game_frame_count;
	s32 game_frame_write_index;

	s32 selected_game_frame_index;

	f32 previous_game_viewport_width;
	f32 previous_game_viewport_height;
	Vec2 game_viewport_position;

	Vec2S32 locked_mouse_start_position;
	MouseLockedState mouse_locked_state;

	DebugCameraState debug_camera;
	DebugDrawState debug_draw_state;

	AnimDB anim_db;

	bool paused = false;
	bool simulate = true;
};