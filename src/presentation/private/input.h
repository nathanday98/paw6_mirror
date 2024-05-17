#pragma once

#include <shared/math.h>

#include <platform/platform.h>

struct PlatformEvent;

// #TODO: Maybe come up with a better layout, since 3 bytes are wasted
struct GameButtonState
{
	u32 half_transition_count;
	bool ended_down;
};

struct GameInputState
{
	GameButtonState mouse_buttons[MouseButton_Count];
	GameButtonState keys[PlatformKeyType_Count];
	Vec2S32 mouse_position;
	Vec2S32 mouse_wheel_delta;
	Vec2S32 raw_mouse_move_delta;

	bool is_key_down(PlatformKeyType key) const;
	bool was_key_pressed(PlatformKeyType key) const;
};