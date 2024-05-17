#include "input.h"

bool GameInputState::is_key_down(PlatformKeyType key) const
{
	return keys[key].ended_down;
}

bool GameInputState::was_key_pressed(PlatformKeyType key) const
{
	const GameButtonState& state = keys[key];
	return (state.ended_down && state.half_transition_count == 1) || state.half_transition_count > 1;
}
