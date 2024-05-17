#include "simulation_graph.h"

#include <shared/std.h>
#include <shared/memory.h>
#include <shared/log.h>

#include <platform/platform.h>

#include "game_player.h"
#include "job_graph_add_helper.inl"
#include "client_graph.h"
#include "allocators/arena_allocator.h"
#include "allocators/frame_allocator.h"
#include "allocators/page_pool.h"
#include "profiler.h"
#include "frame_times.h"
#include "game_player.h"

struct TestData
{
	static constexpr usize arr_count = 10;

	int arr[arr_count];

	int get(usize index) const
	{
		PAW_ASSERT(index < arr_count);

		job_graph_verify_active_policy(this, {ResourceAccessType::ReadOnly, ResourceAccessType::ReadWrite});

		return arr[index];
	}

	void set(usize index, int value)
	{
		PAW_ASSERT(index < arr_count);

		job_graph_verify_active_policy(this, {ResourceAccessType::ReadWrite});

		arr[index] = value;
	}
};

static void test_job(JobGraph* /*graph*/, JobHandle /*job*/, TestData const* const /*test_data*/, Slice<int>)
{
	for (int i = 0; i < 100000; i++)
	{
	}
}

static void physics_job(JobGraph* /*graph*/, JobHandle /*job*/, int, int, TestData const* const /*test_data*/, Slice<int> const)
{
	for (int i = 0; i < 100000; i++)
	{
	}
}

struct FrameTime
{
	u64 frame_start_time_ticks;
};

static UTF8StringView pushStringNullTerminatedVA(Allocator& allocator, char const* format, va_list args)
{
	static byte buffer[2048];
	UTF8StringView result = string_formatVA(buffer, 2048, format, args);
	Slice<byte> mem = allocator.alloc<byte>(result.size_bytes + 1);
	memCopy(buffer, mem.ptr, result.size_bytes);
	mem[result.size_bytes] = 0;
	return UTF8StringView{.ptr = (char const*)mem.ptr, .size_bytes = result.size_bytes, .null_terminated = true};
}

static UTF8StringView push_string_null_terminated(Allocator& allocator, char const* format, ...)
{
	va_list args;
	va_start(args, format);
	const UTF8StringView result = pushStringNullTerminatedVA(allocator, format, args);
	va_end(args);
	return result;
}

void tick_start(JobGraph* job_graph, JobHandle job, GameEventDataSwicher& game_event_data_switch, GamePlayer& game_player, PagePool& debug_page_pool, ArenaAllocator& persistent_allocator, FrameAllocator& frame_allocator, Platform& platform, FrameTime& frame_time, RendererFrameDataStore& renderer_frame_data_store, PlatformWaitTimer wait_timer, JobWaitable timer_waitable);

JobGraph* simulation_graph_init(Platform& platform, PagePool& page_pool, PagePool& debug_page_pool, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, GameStartupArgs const& startup_args, GameEventDataSwicher& game_event_data_switch, RendererFrameDataStore& renderer_frame_data_store)
{
	GamePlayer& game_player = persistent_allocator.alloc_and_construct_single<GamePlayer>();
	game_player.init(platform, persistent_allocator, startup_args, page_pool, debug_page_pool);

	TestData& test_resource = persistent_allocator.alloc<TestData>(1)[0];
	(void)test_resource;

	FrameTime& simulation_frame_time = persistent_allocator.alloc_and_construct_single<FrameTime>();
	simulation_frame_time.frame_start_time_ticks = platform.get_cpu_ticks();

	FrameAllocator& frame_allocator = persistent_allocator.alloc_and_construct_single<FrameAllocator>();
	frame_allocator.init(persistent_allocator, page_pool);

	PlatformWaitTimer wait_timer = platform.create_timer();

	JobGraph* job_graph = job_graph_init(&platform, persistent_allocator, debug_persistent_allocator, page_pool, debug_page_pool, "Simulation");

	const JobWaitable timer_waitable = job_graph_create_waitable(job_graph, platform.get_wait_handle(wait_timer));

	job_graph_set_initial_job(job_graph, JOB_SRC, "GameTickStart"_str, tick_start, persistent_allocator, debug_persistent_allocator, game_event_data_switch, game_player, debug_page_pool, persistent_allocator, frame_allocator, platform, simulation_frame_time, renderer_frame_data_store, wait_timer, timer_waitable);

	job_graph_start(job_graph);

	return job_graph;
}

void tick_finish(JobGraph* job_graph, JobHandle job, FrameAllocator& frame_allocator, Platform& platform, FrameTime& frame_time)
{

	frame_allocator.reset();

	const u64 ticks_per_second = platform.get_tcks_per_second();
	const u64 ticks_per_frame = ticks_per_second / 60;

	{
		PAW_PROFILER_SCOPE("Extra Sleep");
		while (platform.get_cpu_ticks() < frame_time.frame_start_time_ticks + ticks_per_frame)
		{
			_mm_pause();
		}
	}

	job_graph_add_frame_end(job_graph, JOB_SRC, "Sim Frame End"_str, {job});
}

void tick_start(JobGraph* job_graph, JobHandle job, GameEventDataSwicher& game_event_data_switch, GamePlayer& game_player, PagePool& debug_page_pool, ArenaAllocator& /*persistent_allocator*/, FrameAllocator& frame_allocator, Platform& platform, FrameTime& frame_time, RendererFrameDataStore& renderer_frame_data_store, PlatformWaitTimer wait_timer, JobWaitable timer_waitable)
{

	const u64 frame_start_time_ticks = platform.get_cpu_ticks();
	const u64 ticks_per_second = platform.get_tcks_per_second();
	const f64 ticks_per_second_f64 = static_cast<f64>(ticks_per_second);

	const u64 frame_time_elapsed_ticks = frame_start_time_ticks - frame_time.frame_start_time_ticks;
	const f32 delta_time = static_cast<f32>(static_cast<f64>(frame_time_elapsed_ticks) / ticks_per_second_f64);

	GameEventData event_data{};
	{
		PAW_PROFILER_SCOPE("Copy Game Event Data");
		game_event_data_switch.copy_out_and_reset(event_data, platform);
	}

	if (event_data.quit_requested)
	{
		game_player.deinit();
		return;
	}

	FrameTimes::total_time_ms = delta_time * 1000.0f;

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
		ImGui::MenuItem("ImGui Demo Window", "", &gui_state.show_imgui_demo_window);
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
			ImGui::Text("Frame Time: %.2fms", FrameTimes::total_time_ms);
			ImGui::Separator();
			ImGui::Text("Cpu Render Time: %.2fms", FrameTimes::cpu_render_time_ms);
			ImGui::Separator();
			ImGui::Text("Page Pool (%d/%d pages used)", g_page_pool.pages.nodes.size - g_page_pool.pages.free_slot_count, g_page_pool.pages.nodes.size);
			ImGui::Separator();
			ImGui::Text("Debug Page Pool (%d/%d pages used)", debug_page_pool.pages.nodes.size - debug_page_pool.pages.free_slot_count, debug_page_pool.pages.nodes.size);
			ImGui::Separator();
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	ImGui::DockSpaceOverViewport();

	if (gui_state.show_imgui_demo_window)
	{
		ImGui::ShowDemoWindow(&gui_state.show_imgui_demo_window);
	}
#else
	PAW_UNUSED_ARG(debug_page_pool);
#endif

	RendererFrameData& renderer_frame_data = renderer_frame_data_store.lock_game_for_write(platform);

	game_player.tick(platform, delta_time, frame_allocator, event_data, renderer_frame_data);

	renderer_frame_data_store.unlock_game_for_submit(platform);

	TestData& test_data = frame_allocator.alloc_and_construct_single<TestData>();

	const u32 actor_count = 1000;

	Slice<int> const ints = frame_allocator.alloc<int>(actor_count * 2);
	JobHandle root = job_graph_add_job(job_graph, JOB_SRC, "Root"_str, {job}, test_job, &test_data, ints.sub_slice(1, 5));

	Slice<JobHandle> const actors = frame_allocator.alloc<JobHandle>(actor_count);
	for (u32 i = 0; i < actor_count; ++i)
	{
		actors[i] = job_graph_add_job(job_graph, JOB_SRC, push_string_null_terminated(frame_allocator, "Actor_Update%u", i), {root}, test_job, &test_data, ints.sub_slice(actor_count + i, 1));
	}

	const u32 physics_count = actor_count / 2;
	Slice<JobHandle> const physics = frame_allocator.alloc<JobHandle>(physics_count);

	for (u32 i = 0; i < physics_count; ++i)
	{
		const u32 index0 = i * 2 + 0;
		const u32 index1 = i * 2 + 1;
		physics[i] = job_graph_add_job(job_graph, JOB_SRC, push_string_null_terminated(frame_allocator, "Physics_%u_%u", index0, index1), {actors[index0], actors[index1]}, physics_job, 10, 5, &test_data, ints.sub_slice(actor_count + index0, 2));
	}
	JobHandle network = job_graph_add_job(job_graph, JOB_SRC, "Network"_str, {job}, test_job, &test_data, ints.sub_slice(0, 1));

	Slice<JobHandle> const finish_deps = frame_allocator.alloc<JobHandle>(physics_count + 1);
	for (usize i = 0; i < physics_count; ++i)
	{
		finish_deps[i] = physics[i];
	}

	finish_deps[physics_count] = network;

	frame_time.frame_start_time_ticks = frame_start_time_ticks;

	Slice<JobHandle const> const finish_deps_slice = finish_deps.to_const_slice();

	const u64 ticks_per_frame = ticks_per_second / 60;
	const u64 ticks_per_millisecond = ticks_per_second / 1000;
	const u64 ticks_per_microsecond = ticks_per_millisecond / 1000;

	const u64 ticks_elapsed = platform.get_cpu_ticks() - frame_time.frame_start_time_ticks;
	JobHandle wait_job_handle = g_null_job;
	if (ticks_elapsed < ticks_per_frame)
	{
		const u64 ticks_remaining = ticks_per_frame - ticks_elapsed;
		const u64 sleep_us = ticks_remaining / ticks_per_microsecond;
		if (sleep_us > 1)
		{
			// Delay for 1ms less in the hopes the OS doesn't wake us up too late
			// platform->sleep_ms(sleep_ms - 1);

			// PAW_LOG_INFO("Sim waiting for %gms", f64(sleep_us) / 1000.0f);

			platform.set_timer_wait_time(wait_timer, u32(sleep_us - 1));
			wait_job_handle = job_graph_add_wait(job_graph, JOB_SRC, "SimTickWait"_str, finish_deps_slice, timer_waitable);
		}
	}

	job_graph_add_job(job_graph, JOB_SRC, "SimTickFinish"_str, wait_job_handle == g_null_job ? finish_deps_slice : Slice<JobHandle const>{wait_job_handle}, tick_finish, frame_allocator, platform, frame_time);
}
