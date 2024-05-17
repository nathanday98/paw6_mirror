#include "game_player.h"

#include <shared/log.h>
#include <shared/math.inl>
#include <shared/string.h>

#include <platform/platform.h>

PAW_DISABLE_ALL_WARNINGS_BEGIN
#include <Windows.h>
#include <Xinput.h>
PAW_DISABLE_ALL_WARNINGS_END

#include "profiler.h"
#include "imgui_wrapper.h"
#include "frame_times.h"
#include "input.h"
#include "debug_draw.h"
#include "startup_args.h"
#include "allocators/page_pool.h"
#include "usd_anim.h"
#include "world.h"
#include "game.h"
#include "client_graph.h"

void GamePlayer::init(Platform& platform, ArenaAllocator& persistent_allocator, const GameStartupArgs& /*startup_args*/, PagePool& page_pool, PagePool& debug_page_pool)
{

	debug_draw_state.commands_3d.init(&debug_page_pool);
	debug_draw_state.depth_tested_commands_3d.init(&debug_page_pool);

	GameFrame& start_frame = getCurrentGameFrame();
	debugDrawSetState(&start_frame.debug_draw_state);

	// TODO(nathan): This should use a debug allocator
	debugDrawInit(platform, (Allocator&)persistent_allocator, &debug_page_pool);

	//tempLoadWorld("source_data/axis.usd", persistent_allocator, page_pool);
	tempLoadAnim("source_data/walk1_subject5.usd", persistent_allocator, anim_db);

	start_frame.state.init(platform, persistent_allocator, page_pool, anim_db);

	debug_camera.init();
}

void GamePlayer::deinit()
{

	debug_draw_state.commands_3d.deinit();
	debug_draw_state.depth_tested_commands_3d.deinit();
	// game_state.deinit();

	debugDrawDeinit();
}

void GamePlayer::processEvent(Platform& /*platform*/, const PlatformEvent& /*event*/)
{

	// switch (event.type)
	//{

	//	case PlatformEventType_FileUpdate:
	//	{
	//		const UTF8StringView shaders_path = PAW_STR("compiled_data/shaders", 21);
	//		if (UTF8StringView_beginsWith(&event.file_update.path, &shaders_path))
	//		{
	//			rendererOnShaderModification(platform, event.file_update.path);
	//		}
	//	}
	//	break;
	//}

	// rendererProcessEvent(event);
}

static Vec2 postProcessAxis(Vec2 axis, f32 deadzone)
{
	const f32 len = axis.length();
	if (len > deadzone)
	{
		Vec2 dir = axis / len;
		f32 clipped_len = len > 1.0f ? 1.0f : len * len;
		return Vec2{dir.x * clipped_len, dir.y * clipped_len};
	}
	else
	{
		return Vec2{};
	}
}

void GamePlayer::tick(Platform& platform, f32 delta_time, FrameAllocator& frame_allocator, const GameEventData& event_data, RendererFrameData& renderer_frame_data)
{
	PAW_PROFILER_FUNC();

	const GameInputState& input_state = event_data.input;
	if (event_data.focus_changed && !event_data.focused)
	{
		// onUnfocus(platform);
	}

#ifdef PAW_ENABLE_IMGUI
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	// ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
	Vec2 game_viewport_size{};
	if (ImGui::Begin("Game"))
	{
		const ImVec2 cursor_screen_pos = ImGui::GetCursorScreenPos();
		game_viewport_position = Vec2{cursor_screen_pos.x, cursor_screen_pos.y};
		game_viewport_size = ImGui::GetContentRegionAvail();
		if (game_viewport_size.x != previous_game_viewport_width ||
			game_viewport_size.y != previous_game_viewport_height)
		{
			PlatformEvent window_event{};
			window_event.type = PlatformEventType_Resize;
			window_event.resize.width = static_cast<s32>(game_viewport_size.x);
			window_event.resize.height = static_cast<s32>(game_viewport_size.y);
			/*rendererGameViewportResize(
				static_cast<usize>(window_event.resize.width), static_cast<usize>(window_event.resize.height));*/
		}

		// ImGui::Image(reinterpret_cast<ImTextureID>(rendererGameViewportGetImage()), game_viewport_size);
		/*if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			onFocus(platform, input_state);
			PAW_LOG_INFO("Game click");
		}*/

		debug_camera.tick(delta_time, mouse_locked_state, input_state);

		if (input_state.was_key_pressed(PlatformKeyType_Escape) && mouse_locked_state == MouseLockedState::Locked)
		{
			onUnfocus(platform);
		}

		debugDrawText2D(Vec2{50.0f, 300.0f}, mouse_locked_state == MouseLockedState::Locked ? "Focused"_str : "Unfocused"_str);

		previous_game_viewport_width = game_viewport_size.x;
		previous_game_viewport_height = game_viewport_size.y;
	}
	ImGui::End();
	ImGui::PopStyleVar();

	if (ImGui::Begin("Game Player"))
	{
		ImGui::Text("%s", paused ? "Paused" : "Unpaused");
		ImGui::BeginDisabled(!paused);
		// const s32 max_value = math_minS32(game_frame_count, game_frame_max - 1);
		ImGui::SliderInt("Frame", &selected_game_frame_index, math_maxS32(0, game_frame_count - game_frame_max + 1), game_frame_count - 1);
		ImGui::Checkbox("Simulate", &simulate);
		ImGui::EndDisabled();
	}
	ImGui::End();

	if (ImGui::IsKeyPressed(ImGuiKey_Space))
	{
		if (!paused && mouse_locked_state == MouseLockedState::Locked)
		{
			onUnfocus(platform);
		}
		paused = !paused;
	}
#else
	PAW_UNUSED_ARG(input_state);
#endif

	if (!paused)
	{
		GameFrame& current_frame = getCurrentGameFrame();
		if (game_frame_count > 0)
		{
			s32 prev_index = wrapGameFrameIndex(game_frame_write_index - 1);
			if (prev_index == -1)
			{
				prev_index = game_frame_max - 1;
			}
			GameFrame& prev_frame = game_frames[prev_index];

			current_frame.debug_draw_state.commands_3d.deinit();
			current_frame.debug_draw_state.depth_tested_commands_3d.deinit();

			memCopy(&prev_frame, &current_frame, sizeof(GameFrame));

			current_frame.debug_draw_state.commands_3d.cloneFrom(prev_frame.debug_draw_state.commands_3d);
			current_frame.debug_draw_state.depth_tested_commands_3d.cloneFrom(prev_frame.debug_draw_state.depth_tested_commands_3d);
		}

		current_frame.delta_time_seconds = delta_time;

		debugDrawSetState(&current_frame.debug_draw_state);
		debugDrawTickLifetimes(current_frame.delta_time_seconds);

		XINPUT_STATE state;
		if (XInputGetState(0, &state) == ERROR_SUCCESS)
		{
			const f32 axis_dead_zone = 0.2f;
			const f32 right_x = (f32)state.Gamepad.sThumbRX / 32767.0f;
			const f32 right_y = (f32)state.Gamepad.sThumbRY / 32767.0f;
			const f32 left_x = (f32)state.Gamepad.sThumbLX / 32767.0f;
			const f32 left_y = (f32)state.Gamepad.sThumbLY / 32767.0f;
			current_frame.gamepad.left_stick = postProcessAxis({left_x, left_y}, axis_dead_zone);
			current_frame.gamepad.right_stick = postProcessAxis({right_x, right_y}, axis_dead_zone);
		}

		current_frame.state.tick(platform, current_frame.delta_time_seconds, frame_allocator, current_frame.gamepad, anim_db);

		game_frame_count++;
		selected_game_frame_index = game_frame_count;
		game_frame_write_index = wrapGameFrameIndex(game_frame_write_index + 1);
	}

	GameFrame& selected_game_frame = game_frames[(selected_game_frame_index - 1) % game_frame_max];

	if (paused && simulate)
	{
		if (game_frame_count > 0)
		{
			const f32 selected_delta_time_seconds = selected_game_frame.delta_time_seconds;
			const GamepadInput selected_gamepad = selected_game_frame.gamepad;

			s32 prev_index = wrapGameFrameIndex(selected_game_frame_index - 1 - 1);
			if (prev_index == -1)
			{
				prev_index = game_frame_max - 1;
			}
			GameFrame& prev_frame = game_frames[prev_index];

			selected_game_frame.debug_draw_state.commands_3d.deinit();
			selected_game_frame.debug_draw_state.depth_tested_commands_3d.deinit();

			memCopy(&prev_frame, &selected_game_frame, sizeof(GameFrame));

			selected_game_frame.debug_draw_state.commands_3d.cloneFrom(prev_frame.debug_draw_state.commands_3d);
			selected_game_frame.debug_draw_state.depth_tested_commands_3d.cloneFrom(prev_frame.debug_draw_state.depth_tested_commands_3d);

			selected_game_frame.delta_time_seconds = selected_delta_time_seconds;
			selected_game_frame.gamepad = selected_gamepad;
		}

		debugDrawSetState(&selected_game_frame.debug_draw_state);
		debugDrawTickLifetimes(selected_game_frame.delta_time_seconds);

		selected_game_frame.state.tick(platform, selected_game_frame.delta_time_seconds, frame_allocator, selected_game_frame.gamepad, anim_db);
	}

	// debugDrawSetState(&debug_draw_state);
	// debugDrawTickLifetimes(delta_time);

	// anim_db.debugQuery(selected_game_frame.state, frame_allocator);

	/*const Vec2S32 drawable_size = platform.getDrawableSize();
	const Vec2 viewport_size{(f32)drawable_size.x, (f32)drawable_size.y};*/

	const Mat4 camera_mat = toMat4(debug_camera.transform.inversed());
	// const Mat4 camera_projection =
	//	rendererGetCameraProjectionMat(math_toRadians(60.0f), game_viewport_size.x, game_viewport_size.y, 0.01f, 1000.0f);

	renderer_frame_data.camera_mat = camera_mat;
	renderer_frame_data.camera_position = debug_camera.transform.translation;
	renderer_frame_data.game_state_debug_draw_state = &selected_game_frame.debug_draw_state;
	renderer_frame_data.debug_draw_state = &debug_draw_state;

	// rendererRender(platform, render_frame_data);
}

void GamePlayer::onFocus(Platform& platform, const GameInputState& input_state)
{
#ifdef PAW_ENABLE_IMGUI
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NavNoCaptureKeyboard;
#endif

	mouse_locked_state = MouseLockedState::Locked;
	locked_mouse_start_position = input_state.mouse_position;
	platform.set_cursor_visible(false);
	platform.set_mouse_locked(true);
	PAW_LOG_INFO("Focus gained");
}

void GamePlayer::onUnfocus(Platform& platform)
{
	mouse_locked_state = MouseLockedState::Unlocked;
	platform.set_cursor_position(locked_mouse_start_position.x, locked_mouse_start_position.y);
	platform.set_cursor_visible(true);
	platform.set_mouse_locked(false);
#ifdef PAW_ENABLE_IMGUI
	ImGui::GetIO().ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NavNoCaptureKeyboard);
#endif

	PAW_LOG_INFO("Focus lost");
}

GameFrame& GamePlayer::getCurrentGameFrame()
{
	return game_frames[game_frame_write_index];
}

s32 GamePlayer::wrapGameFrameIndex(s32 index)
{
	return index % game_frame_max;
}

void DebugCameraState::init()
{
	transform.translation = Vec3{0.0f, 3.0f, 0.0f};
	pitch = math_toRadians(0.0f);
	yaw = math_toRadians(0.0f);
}

void DebugCameraState::tick(f32 delta_time_seconds, MouseLockedState mouse_locked_state, const GameInputState& input_state)
{
	if (mouse_locked_state == MouseLockedState::Locked)
	{
		const Vec2 mouse_delta{(f32)input_state.raw_mouse_move_delta.x, (f32)input_state.raw_mouse_move_delta.y};
		pitch -= mouse_delta.y * 0.25f * delta_time_seconds;
		yaw -= mouse_delta.x * 0.25f * delta_time_seconds;
	}

	transform.rotation =
		fromAngleAxis(math_world_up, yaw) * fromAngleAxis(math_world_right, pitch);

	const f32 z_axis = (input_state.is_key_down(PlatformKeyType_W) ? 1.0f : 0.0f) - (input_state.is_key_down(PlatformKeyType_S) ? 1.0f : 0.0f);
	const f32 x_axis = (input_state.is_key_down(PlatformKeyType_D) ? 1.0f : 0.0f) - (input_state.is_key_down(PlatformKeyType_A) ? 1.0f : 0.0f);
	const f32 y_axis = (input_state.is_key_down(PlatformKeyType_E) ? 1.0f : 0.0f) - (input_state.is_key_down(PlatformKeyType_Q) ? 1.0f : 0.0f);

	const f32 move_speed = input_state.is_key_down(PlatformKeyType_LShift) ? 100.0f : 10.0f;
	const Vec3 camera_forward = transform.calcForward();
	const Vec3 camera_right = transform.calcRight();
	transform.translation += normalizeSafely(camera_forward * z_axis + camera_right * x_axis + math_world_up * y_axis) * move_speed * delta_time_seconds;
}
