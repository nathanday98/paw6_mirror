#include "client_graph.h"

#include <shared/string_id.inl>
#include <shared/math.inl>
#include <shared/string.h>

#include "allocators/frame_allocator.h"
#include "allocators/arena_allocator.h"
#include "allocators/page_pool.h"
#include "job_graph.h"
#include "job_graph_add_helper.inl"
#include "profiler.h"
#include "new_renderer.h"
#define PAW_ENABLE_IMGUI
#include "imgui_wrapper.h"
#include "game.h"
#include "renderer/dx12_renderer.h"
#include "transform.h"

struct FrameTime
{
	u64 frame_start_time_ticks;
};

enum class MouseLockedState
{
	Unlocked,
	Locked,
};

struct DebugCameraState2
{
	f32 yaw = 0.0f;
	f32 pitch = 0.0f;
	Transform transform{};

	void init();
	void tick(f32 delta_time_seconds, MouseLockedState mouse_locked_state, GameInputState const& input_state);
};

struct GameViewportState
{
	MouseLockedState mouse_locked_state;
	Vec2S32 locked_mouse_start_position;
	DebugCameraState2 debug_camera;

	void onFocus(Platform& platform, GameInputState const& input_state)
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

	void onUnfocus(Platform& platform)
	{
		if (mouse_locked_state == MouseLockedState::Locked)
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
	}
};

static void tick_start(JobGraph* job_graph, JobHandle job, PagePool& debug_page_pool, ArenaAllocator& debug_persistent_allocator, FrameAllocator& frame_allocator, Platform& platform, FrameTime& frame_time, GameEventDataSwicher& game_event_data_switcher, RendererFrameDataStore& renderer_frame_data_store, DX12Renderer& renderer, GameViewportState& game_viewport_state);

JobGraph* client_graph_init(Platform& platform, PagePool& page_pool, PagePool& debug_page_pool, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, GameStartupArgs const& startup_args, GameEventDataSwicher& game_event_data_switch, RendererFrameDataStore& renderer_frame_data_store, void* graphics_adapter, void* graphics_factory)
{
	FrameAllocator& frame_allocator = persistent_allocator.alloc_and_construct_single<FrameAllocator>();
	frame_allocator.init(persistent_allocator, page_pool);

	JobGraph* job_graph = job_graph_init(&platform, persistent_allocator, debug_persistent_allocator, page_pool, debug_page_pool, nullptr);

	GameViewportState& game_viewport_state = persistent_allocator.alloc_and_construct_single<GameViewportState>();
	game_viewport_state.mouse_locked_state = MouseLockedState::Unlocked;
	game_viewport_state.debug_camera.init();

	DX12Renderer& renderer = dx12_renderer_init(platform, persistent_allocator, debug_persistent_allocator, graphics_adapter, startup_args, graphics_factory, &page_pool, &debug_page_pool, job_graph, Vec2S32{400, 400});

	// rendererInit(platform, startup_args, (Allocator&)persistent_allocator, page_pool);

	FrameTime& frame_time = persistent_allocator.alloc_and_construct_single<FrameTime>();
	frame_time.frame_start_time_ticks = platform.get_cpu_ticks();

	job_graph_set_initial_job(job_graph, JOB_SRC, "ClientTickStart"_str, tick_start, persistent_allocator, debug_persistent_allocator, debug_page_pool, debug_persistent_allocator, frame_allocator, platform, frame_time, game_event_data_switch, renderer_frame_data_store, renderer, game_viewport_state);
	job_graph_start(job_graph);

	return job_graph;
}

static void end_job_func(JobGraph*, JobHandle, FrameAllocator& frame_allocator, int&)
{
	frame_allocator.reset();
}

static void tick_start(JobGraph* job_graph, JobHandle job, PagePool& /*debug_page_pool*/, ArenaAllocator& /*debug_persistent_allocator*/, FrameAllocator& frame_allocator, Platform& platform, FrameTime& frame_time, GameEventDataSwicher& game_event_data_switcher, RendererFrameDataStore& renderer_frame_data_store, DX12Renderer& renderer, GameViewportState& game_viewport_state)
{
	const u64 frame_start_time_ticks = platform.get_cpu_ticks();
	const u64 ticks_per_second = platform.get_tcks_per_second();
	const f64 ticks_per_second_f64 = static_cast<f64>(ticks_per_second);

	const u64 frame_time_elapsed_ticks = frame_start_time_ticks - frame_time.frame_start_time_ticks;
	const f32 delta_time = static_cast<f32>(static_cast<f64>(frame_time_elapsed_ticks) / ticks_per_second_f64);

	frame_time.frame_start_time_ticks = frame_start_time_ticks;

	GameEventData& game_event_data = game_event_data_switcher.lock_for_write(platform);
	platform.pump_events(game_event_data);
	game_event_data_switcher.unlock_for_write(platform);

	if (game_event_data.quit_requested)
	{
		platform.request_quit();
		return;
	}

	if (game_event_data.focus_changed && !game_event_data.focused)
	{
		game_viewport_state.onUnfocus(platform);
	}

	// rendererBeginFrame();
#ifdef PAW_ENABLE_IMGUI
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Engine"))
	{
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Profiler"))
	{
		if (ImGui::MenuItem("Open Tracy"))
		{
			profilerLaunch();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
		// ImGui::MenuItem("ImGui Demo Window", "", &gui_state.show_imgui_demo_window);
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	const f32 frame_height = ImGui::GetFrameHeight();
	if (ImGui::BeginViewportSideBar(
			"##MainStatusBar",
			ImGui::GetMainViewport(),
			ImGuiDir_Down,
			frame_height,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::Text("Hey");
			// ImGui::Text("Frame Time: %.2fms", FrameTimes::total_time_ms);
			ImGui::Separator();
			// ImGui::Text("Cpu Render Time: %.2fms", FrameTimes::cpu_render_time_ms);
			ImGui::Separator();
			// ImGui::Text("Page Pool (%d/%d pages used)", g_page_pool.pages.nodes.size - g_page_pool.pages.free_slot_count, g_page_pool.pages.nodes.size);
			ImGui::Separator();
			// ImGui::Text("Debug Page Pool (%d/%d pages used)", debug_page_pool.pages.nodes.size - debug_page_pool.pages.free_slot_count, debug_page_pool.pages.nodes.size);
			ImGui::Separator();
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	ImGui::DockSpaceOverViewport();

	ImGui::ShowDemoWindow();
#endif

	RendererFrameData& renderer_frame_data = frame_allocator.alloc_and_construct_single<RendererFrameData>();

	// #TODO: Taking a copy here just temporarily, should be done better
	renderer_frame_data = renderer_frame_data_store.copy_for_read(platform);

	Vec2S32 game_viewport_size{};

#ifdef PAW_ENABLE_IMGUI
	ImGui::Begin("Renderer data");
	ImGui::Text("Camera position: %g, %g, %g", renderer_frame_data.camera_position.x, renderer_frame_data.camera_position.y, renderer_frame_data.camera_position.z);
	ImGui::Text("Game frame index: %llu", renderer_frame_data_store.render_read_index);
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("Game"))
	{
		const ImVec2 available_size = ImGui::GetContentRegionAvail();
		game_viewport_size.x = math_maxS32(math_floor_to_s32(available_size.x), 1);
		game_viewport_size.y = math_maxS32(math_floor_to_s32(available_size.y), 1);
		ImGui::Image(reinterpret_cast<ImTextureID>((u64)get_game_viewport_descriptor_index_for_current_frame(renderer).value), ImVec2((f32)game_viewport_size.x, (f32)game_viewport_size.y));

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			game_viewport_state.onFocus(platform, game_event_data.input);
		}

		// #TODO: don't use game input here, it should probably be separate?
		game_viewport_state.debug_camera.tick(delta_time, game_viewport_state.mouse_locked_state, game_event_data.input);

		if (game_event_data.input.was_key_pressed(PlatformKeyType_Escape) && game_viewport_state.mouse_locked_state == MouseLockedState::Locked)
		{
			game_viewport_state.onUnfocus(platform);
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
#endif

	renderer_frame_data.camera_mat = toMat4(game_viewport_state.debug_camera.transform.inversed());
	renderer_frame_data.camera_position = game_viewport_state.debug_camera.transform.translation;

	const JobHandle renderer_end_job = dx12_renderer_get_graph(job_graph, job, renderer, game_viewport_size, renderer_frame_data, platform, frame_allocator);

	// rendererRender(platform, renderer_frame_data);

	int& ii = frame_allocator.alloc_and_construct_single<int>();
	ii = 1234;

	const JobHandle end_job = job_graph_add_job(job_graph, JOB_SRC, "Client End"_str, {renderer_end_job}, end_job_func, frame_allocator, ii);

	job_graph_add_frame_end(job_graph, JOB_SRC, "Client Frame End"_str, {end_job});
}

void GameEventDataSwicher::init(Platform& platform)
{
	mutex = platform.create_mutex();
}

void GameEventDataSwicher::deinit(Platform& platform)
{
	platform.destroy_mutex(mutex);
}

void GameEventDataSwicher::copy_out_and_reset(GameEventData& out_data, Platform& platform)
{
	PAW_PROFILER_FUNC();
	platform.lock_mutex(mutex);
	memCopy(&render_write_data, &out_data, sizeof(GameEventData));
	render_write_data = GameEventData{};
	platform.unlock_mutex(mutex);
}

GameEventData& GameEventDataSwicher::lock_for_write(Platform& platform)
{
	PAW_PROFILER_FUNC();
	platform.lock_mutex(mutex);
	return render_write_data;
}

void GameEventDataSwicher::unlock_for_write(Platform& platform)
{
	PAW_PROFILER_FUNC();
	platform.unlock_mutex(mutex);
}

void RendererFrameDataStore::init(Platform& platform)
{
	mutex = platform.create_mutex();
}

void RendererFrameDataStore::deinit(Platform& platform)
{
	platform.destroy_mutex(mutex);
}

RendererFrameData& RendererFrameDataStore::lock_game_for_write(Platform& platform)
{
	PAW_PROFILER_FUNC();
	platform.lock_mutex(mutex);
	return game_write;
}

void RendererFrameDataStore::unlock_game_for_submit(Platform& platform)
{
	PAW_PROFILER_FUNC();
	game_write_index++;
	platform.unlock_mutex(mutex);
}

RendererFrameData const& RendererFrameDataStore::copy_for_read(Platform& platform)
{
	PAW_PROFILER_FUNC();
	platform.lock_mutex(mutex);
	if (game_write_index > render_read_index)
	{
		PAW_PROFILER_SCOPE("memCopy");
		const usize frame_delta = game_write_index - render_read_index;
		if (frame_delta > 1)
		{
			PAW_LOG_WARNING("Frame difference between game and render was %llu", frame_delta);
		}
		memCopy(&game_write, &render_read, sizeof(render_read));
		render_read_index = game_write_index;
	}
	platform.unlock_mutex(mutex);
	return render_read;
}

void DebugCameraState2::init()
{
	transform.translation = Vec3{0.0f, 3.0f, 0.0f};
	pitch = math_toRadians(0.0f);
	yaw = math_toRadians(0.0f);
}

void DebugCameraState2::tick(f32 delta_time_seconds, MouseLockedState mouse_locked_state, GameInputState const& input_state)
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
