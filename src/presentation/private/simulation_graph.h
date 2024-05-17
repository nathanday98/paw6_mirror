#pragma once

struct JobGraph;
struct Platform;
struct PagePool;
struct ArenaAllocator;
struct GameStartupArgs;
struct GameEventDataSwicher;
struct RendererFrameDataStore;

JobGraph* simulation_graph_init(Platform& platform, PagePool& page_pool, PagePool& debug_page_pool, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, const GameStartupArgs& startup_args, GameEventDataSwicher& game_event_data_switch, RendererFrameDataStore& renderer_frame_data_store);
