#include "game.h"

#include <shared/memory.h>
#include <shared/math.inl>
#include <shared/log.h>
#include <shared/string_id.inl>

#include <platform/platform.h>

#include <testing/testing.h>

#include <simulation/simulation.h>

#include "transform.h"
#include "allocators/frame_allocator.h"
#include "allocators/page_pool.h"
#include "allocators/arena_allocator.h"
#include "imgui_wrapper.h"
#include "new_renderer.h"
#include "startup_args.h"
#include "profiler.h"
#include "job_queue.h"
#include "job_graph.h"
#include "client_graph.h"
#include "simulation_graph.h"
#include "swf.h"

#define PAW_TEST_MODULE_NAME game

// PAW_TEST(Thing)
//{
//	PAW_TEST_EXPECT_EQUAL(1, 0);
// }
//
// PAW_TEST(Thing2)
//{
//	PAW_TEST_EXPECT_EQUAL(1, 0);
// }

static PagePool g_page_pool{};
static ArenaAllocator g_persistent_allocator{};

struct GameSystems
{
	JobGraph* simulation_job_graph;
	JobGraph* client_job_graph;
};

GameSystems* game_init(Platform& platform, GameStartupArgs const& startup_args, void* graphics_adapter, void* graphics_factory)
{
	PAW_UNUSED_ARG(startup_args);

	g_page_pool.init(platform, 256, "Page Pool"_str);

	g_persistent_allocator.init(&g_page_pool, "Persistent Allocator"_str);

	simulation_init(g_shared_assert_func);

	//{
	//	ArenaAllocator swf_allocator{};
	//	swf_allocator.init(&g_page_pool, "swf allocator"_str);
	//	Slice<byte> file_data = platform.DEBUG_load_file_blocking("source_data/swfs/ground.swf"_str, swf_allocator);
	//	loadSwfFromMemory(file_data, swf_allocator, false, g_persistent_allocator);
	//	swf_allocator.reset();
	//}

	GameSystems* self = &g_persistent_allocator.alloc_and_construct_single<GameSystems>();

	PagePool& debug_page_pool = g_persistent_allocator.alloc_and_construct_single<PagePool>();
	debug_page_pool.init(platform, 256, "Debug Page Pool"_str);

	ArenaAllocator& debug_persistent_allocator = g_persistent_allocator.alloc_and_construct_single<ArenaAllocator>();
	debug_persistent_allocator.init(&debug_page_pool, "Debug Persistent Allocator"_str);

	GameEventDataSwicher& game_event_data_switch = g_persistent_allocator.alloc_and_construct_single<GameEventDataSwicher>();
	game_event_data_switch.init(platform);

	RendererFrameDataStore& renderer_frame_data_store = g_persistent_allocator.alloc_and_construct_single<RendererFrameDataStore>();
	renderer_frame_data_store.init(platform);

	job_queue_init(platform, g_persistent_allocator);

	Transform::registerFormatter();

	self->client_job_graph = client_graph_init(platform, g_page_pool, debug_page_pool, g_persistent_allocator, debug_persistent_allocator, startup_args, game_event_data_switch, renderer_frame_data_store, graphics_adapter, graphics_factory);
	self->simulation_job_graph = simulation_graph_init(platform, g_page_pool, debug_page_pool, g_persistent_allocator, debug_persistent_allocator, startup_args, game_event_data_switch, renderer_frame_data_store);

	job_queue_start();

	return self;
}

void game_deinit(GameSystems* self, Platform& /*platform*/)
{
	job_queue_deinit();
	job_graph_deinit(self->simulation_job_graph);
	job_graph_deinit(self->client_job_graph);
}
