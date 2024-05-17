#pragma once

#include <shared/std.h>
#include <shared/math.h>
#include <shared/slice.h>

#include "../job_graph.h"

struct DX12Renderer;
struct ArenaAllocator;
struct GameStartupArgs;
struct Platform;
struct FrameAllocator;
struct PagePool;
struct RendererFrameData;

struct DescriptorIndex
{
	u32 value;
};

DX12Renderer& dx12_renderer_init(Platform& platform, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, void* graphics_adapter, GameStartupArgs const& startup_args, void* graphics_factory, PagePool* page_pool, PagePool* debug_page_pool, JobGraph* job_graph, Vec2S32 game_viewport_size);
void dx12_renderer_deinit(DX12Renderer& self);
JobHandle dx12_renderer_get_graph(JobGraph* graph, JobHandle const& root_job, DX12Renderer& self, Vec2S32 game_viewport_size, RendererFrameData const& frame_data, Platform& platform, FrameAllocator& frame_allocator);
DescriptorIndex get_game_viewport_descriptor_index_for_current_frame(DX12Renderer const& self);

template <>
struct JobResourceTypeInfo<DX12Renderer&>
{
	static constexpr bool value = true;
	static JobResource get(DX12Renderer& ref, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(&ref);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + 8,
			.access = ResourceAccessType::ReadWrite,
			.arg_index = arg_index,
		};
	}
};