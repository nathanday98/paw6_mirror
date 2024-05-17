#include "dx12_renderer.h"

#include <shared/log.h>
#include <shared/defer.h>
#include <shared/memory.h>
#include <shared/math.inl>
#include <shared/string.h>
#include <shared/pod.h>
#include <shared/non_copyable.h>
#include <shared/slice.h>
#include <shared/slice2d.h>

#include <platform/platform.h>

PAW_DISABLE_ALL_WARNINGS_BEGIN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d12/d3d12.h>
#include <dxgi1_6.h>
#include <dxc/dxcapi.h>
#include <dxc/d3d12shader.h>
#include <d3d12/d3dx12/d3dx12_pipeline_state_stream.h>
#include <WinPixEventRuntime/pix3.h>
#include <tracy/TracyD3D12.hpp>
PAW_DISABLE_ALL_WARNINGS_END

#include "../allocators/arena_allocator.h"
#include "../startup_args.h"
#define PAW_ENABLE_IMGUI
#include "../imgui_wrapper.h"
#include "../debug_draw.h"
#include "../client_graph.h"
#include "../job_graph_add_helper.inl"
#include "../profiler.h"

static constexpr u32 g_frames_in_flight = 2;

struct float2
{
	f32 x, y;
};

struct float3
{
	f32 x, y, z;
};

struct float4
{
	f32 x, y, z, w;
};

static void set_debug_name(ID3D12Resource* resource, UTF8StringView const& name)
{
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.size_bytes), name.ptr);
}

static char const* const get_d3d12_access_name(D3D12_BARRIER_ACCESS access)
{
	switch (access)
	{
		case D3D12_BARRIER_ACCESS_COMMON:
			return "COMMON";
		case D3D12_BARRIER_ACCESS_VERTEX_BUFFER:
			return "VERTEX_BUFFER";
		case D3D12_BARRIER_ACCESS_CONSTANT_BUFFER:
			return "CONSTANT_BUFFER";
		case D3D12_BARRIER_ACCESS_INDEX_BUFFER:
			return "INDEX_BUFFER";
		case D3D12_BARRIER_ACCESS_RENDER_TARGET:
			return "RENDER_TARGET";
		case D3D12_BARRIER_ACCESS_UNORDERED_ACCESS:
			return "UNORDERED_ACCESS";
		case D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE:
			return "DEPTH_STENCIL_WRITE";
		case D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ:
			return "DEPTH_STENCIL_READ";
		case D3D12_BARRIER_ACCESS_SHADER_RESOURCE:
			return "SHADER_RESOURCE";
		case D3D12_BARRIER_ACCESS_STREAM_OUTPUT:
			return "STREAM_OUTPUT";
		case D3D12_BARRIER_ACCESS_INDIRECT_ARGUMENT:
			return "INDIRECT_ARGUMENT or PREDICATION";
		case D3D12_BARRIER_ACCESS_COPY_DEST:
			return "COPY_DEST";
		case D3D12_BARRIER_ACCESS_COPY_SOURCE:
			return "COPY_SOURCE";
		case D3D12_BARRIER_ACCESS_RESOLVE_DEST:
			return "RESOLVE_DEST";
		case D3D12_BARRIER_ACCESS_RESOLVE_SOURCE:
			return "RESOLVE_SOURCE";
		case D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ:
			return "RAYTRACING_ACCELERATION_STRUCTURE_READ";
		case D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE:
			return "RAYTRACING_ACCELERATION_STRUCTURE_WRITE";
		case D3D12_BARRIER_ACCESS_SHADING_RATE_SOURCE:
			return "SHADING_RATE_SOURCE";
		case D3D12_BARRIER_ACCESS_VIDEO_DECODE_READ:
			return "VIDEO_DECODE_READ";
		case D3D12_BARRIER_ACCESS_VIDEO_DECODE_WRITE:
			return "VIDEO_DECODE_WRITE";
		case D3D12_BARRIER_ACCESS_VIDEO_PROCESS_READ:
			return "VIDEO_PROCESS_READ";
		case D3D12_BARRIER_ACCESS_VIDEO_PROCESS_WRITE:
			return "VIDEO_PROCESS_WRITE";
		case D3D12_BARRIER_ACCESS_VIDEO_ENCODE_READ:
			return "VIDEO_ENCODE_READ";
		case D3D12_BARRIER_ACCESS_VIDEO_ENCODE_WRITE:
			return "VIDEO_ENCODE_WRITE";
		case D3D12_BARRIER_ACCESS_NO_ACCESS:
			return "NO_ACCESS";
	}
	PAW_UNREACHABLE;
	return "Unknown";
}

static char const* const get_d3d12_sync_name(D3D12_BARRIER_SYNC sync)
{
	switch (sync)
	{
		case D3D12_BARRIER_SYNC_NONE:
			return "NONE";
		case D3D12_BARRIER_SYNC_ALL:
			return "ALL";
		case D3D12_BARRIER_SYNC_DRAW:
			return "DRAW";
		case D3D12_BARRIER_SYNC_INDEX_INPUT:
			return "INDEX_INPUT";
		case D3D12_BARRIER_SYNC_VERTEX_SHADING:
			return "VERTEX_SHADING";
		case D3D12_BARRIER_SYNC_PIXEL_SHADING:
			return "PIXEL_SHADING";
		case D3D12_BARRIER_SYNC_DEPTH_STENCIL:
			return "DEPTH_STENCIL";
		case D3D12_BARRIER_SYNC_RENDER_TARGET:
			return "RENDER_TARGET";
		case D3D12_BARRIER_SYNC_COMPUTE_SHADING:
			return "COMPUTE_SHADING";
		case D3D12_BARRIER_SYNC_RAYTRACING:
			return "RAYTRACING";
		case D3D12_BARRIER_SYNC_COPY:
			return "COPY";
		case D3D12_BARRIER_SYNC_RESOLVE:
			return "RESOLVE";
		case D3D12_BARRIER_SYNC_EXECUTE_INDIRECT:
			return "EXECUTE_INDIRECT or PREDICATION";
		case D3D12_BARRIER_SYNC_ALL_SHADING:
			return "ALL_SHADING";
		case D3D12_BARRIER_SYNC_NON_PIXEL_SHADING:
			return "NON_PIXEL_SHADING";
		case D3D12_BARRIER_SYNC_EMIT_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO:
			return "EMIT_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO";
		case D3D12_BARRIER_SYNC_CLEAR_UNORDERED_ACCESS_VIEW:
			return "CLEAR_UNORDERED_ACCESS_VIEW";
		case D3D12_BARRIER_SYNC_VIDEO_DECODE:
			return "VIDEO_DECODE";
		case D3D12_BARRIER_SYNC_VIDEO_PROCESS:
			return "VIDEO_PROCESS";
		case D3D12_BARRIER_SYNC_VIDEO_ENCODE:
			return "VIDEO_ENCODE";
		case D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE:
			return "BUILD_RAYTRACING_ACCELERATION_STRUCTURE";
		case D3D12_BARRIER_SYNC_COPY_RAYTRACING_ACCELERATION_STRUCTURE:
			return "COPY_RAYTRACING_ACCELERATION_STRUCTURE";
		case D3D12_BARRIER_SYNC_SPLIT:
			return "SPLIT";
	}
	PAW_UNREACHABLE;
	return "Unknown";
}

static char const* const get_d3d12_layour_name(D3D12_BARRIER_LAYOUT layout)
{
	switch (layout)
	{
		case D3D12_BARRIER_LAYOUT_UNDEFINED:
			return "UNDEFINED";
		case D3D12_BARRIER_LAYOUT_COMMON:
			return "COMMON or PRESENT";
		case D3D12_BARRIER_LAYOUT_GENERIC_READ:
			return "GENERIC_READ";
		case D3D12_BARRIER_LAYOUT_RENDER_TARGET:
			return "RENDER_TARGET";
		case D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS:
			return "UNORDERED_ACCESS";
		case D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE:
			return "DEPTH_STENCIL_WRITE";
		case D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ:
			return "DEPTH_STENCIL_READ";
		case D3D12_BARRIER_LAYOUT_SHADER_RESOURCE:
			return "SHADER_RESOURCE";
		case D3D12_BARRIER_LAYOUT_COPY_SOURCE:
			return "COPY_SOURCE";
		case D3D12_BARRIER_LAYOUT_COPY_DEST:
			return "COPY_DEST";
		case D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE:
			return "RESOLVE_SOURCE";
		case D3D12_BARRIER_LAYOUT_RESOLVE_DEST:
			return "RESOLVE_DEST";
		case D3D12_BARRIER_LAYOUT_SHADING_RATE_SOURCE:
			return "SHADING_RATE_SOURCE";
		case D3D12_BARRIER_LAYOUT_VIDEO_DECODE_READ:
			return "VIDEO_DECODE_READ";
		case D3D12_BARRIER_LAYOUT_VIDEO_DECODE_WRITE:
			return "VIDEO_DECODE_WRITE";
		case D3D12_BARRIER_LAYOUT_VIDEO_PROCESS_READ:
			return "VIDEO_PROCESS_READ";
		case D3D12_BARRIER_LAYOUT_VIDEO_PROCESS_WRITE:
			return "VIDEO_PROCESS_WRITE";
		case D3D12_BARRIER_LAYOUT_VIDEO_ENCODE_READ:
			return "VIDEO_ENCODE_READ";
		case D3D12_BARRIER_LAYOUT_VIDEO_ENCODE_WRITE:
			return "VIDEO_ENCODE_WRITE";
		case D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COMMON:
			return "DIRECT_QUEUE_COMMON";
		case D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_GENERIC_READ:
			return "DIRECT_QUEUE_GENERIC_READ";
		case D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_UNORDERED_ACCESS:
			return "DIRECT_QUEUE_UNORDERED_ACCESS";
		case D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_SHADER_RESOURCE:
			return "DIRECT_QUEUE_SHADER_RESOURCE";
		case D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COPY_SOURCE:
			return "DIRECT_QUEUE_COPY_SOURCE";
		case D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COPY_DEST:
			return "DIRECT_QUEUE_COPY_DEST";
		case D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_COMMON:
			return "COMPUTE_QUEUE_COMMON";
		case D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_GENERIC_READ:
			return "COMPUTE_QUEUE_GENERIC_READ";
		case D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_UNORDERED_ACCESS:
			return "COMPUTE_QUEUE_UNORDERED_ACCESS";
		case D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_SHADER_RESOURCE:
			return "COMPUTE_QUEUE_SHADER_RESOURCE";
		case D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_COPY_SOURCE:
			return "COMPUTE_QUEUE_COPY_SOURCE";
		case D3D12_BARRIER_LAYOUT_COMPUTE_QUEUE_COPY_DEST:
			return "COMPUTE_QUEUE_COPY_DEST";
		case D3D12_BARRIER_LAYOUT_VIDEO_QUEUE_COMMON:
			return "VIDEO_QUEUE_COMMON";
	}
	PAW_UNREACHABLE;
	return "Unknown";
}

struct PixEventScope
{
	ID3D12GraphicsCommandList* command_list;

	PixEventScope(ID3D12GraphicsCommandList* in_command_list, char const* name)
		: command_list(in_command_list)
	{
		PIXBeginEvent(command_list, 0, name);
	}

	~PixEventScope()
	{
		PIXEndEvent(command_list);
	}
};

#define PIX_EVENT_SCOPE(command_list, name) PixEventScope PAW_CONCAT(__pix_event_, __LINE__)(command_list, name);

enum QueueType
{
	QueueType_Graphics,
	QueueType_Compute,
	QueueType_Copy,
	QueueType_Count,
};

static constexpr D3D12_COMMAND_LIST_TYPE g_queue_type_to_command_list_type[QueueType_Count]{
	/* QueueType_Graphics */ D3D12_COMMAND_LIST_TYPE_DIRECT,
	/* QueueType_Compute */ D3D12_COMMAND_LIST_TYPE_COMPUTE,
	/* QueueType_Copy */ D3D12_COMMAND_LIST_TYPE_COPY,
};

typedef usize SlotIndex;

struct CommandListPool
{
	struct Slot
	{
		ID3D12CommandList* command_list;
		usize next_free_index;
	};

	static constexpr usize null_slot = (usize)-1;

	Slice<Slot> slots;
	usize first_free_index;

	void init(ID3D12Device12* device, D3D12_COMMAND_LIST_TYPE type, ArenaAllocator& persistent_allocator, usize count)
	{
		HRESULT result;
		PAW_ASSERT_UNUSED(result);
		slots = persistent_allocator.alloc<Slot>(count);
		for (usize i = 0; i < slots.size; ++i)
		{
			result = device->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&slots[i].command_list));
			PAW_ASSERT(SUCCEEDED(result));
			slots[i].next_free_index = i + 1;
		}

		slots[slots.size - 1].next_free_index = null_slot;
		first_free_index = 0;
	}

	SlotIndex alloc(ID3D12CommandList*& out_command_list)
	{
		PAW_ASSERT(first_free_index != null_slot);
		const usize free_slot_index = first_free_index;
		Slot& free_slot = slots[free_slot_index];
		first_free_index = free_slot.next_free_index;
		out_command_list = free_slot.command_list;
		return free_slot_index;
	}

	SlotIndex allocGraphics(ID3D12GraphicsCommandList9*& out_command_list)
	{
		ID3D12CommandList* command_list;
		SlotIndex index = alloc(command_list);
		HRESULT result = command_list->QueryInterface(&out_command_list);
		PAW_ASSERT_UNUSED(result);
		PAW_ASSERT(SUCCEEDED(result));
		return index;
	}

	void free(SlotIndex index)
	{
		PAW_ASSERT(index != null_slot);
		PAW_ASSERT(index < slots.size);
		Slot& slot = slots[index];
		slot.next_free_index = first_free_index;
		first_free_index = index;
	}
};

enum ShaderType
{
	ShaderType_Vertex,
	ShaderType_Pixel,
	ShaderType_Amplification,
	ShaderType_Mesh,
	ShaderType_Compute,
	ShaderType_Count,
};

struct GraphicsPipelineState
{
	ID3D12PipelineState* pso;
	ID3D12RootSignature* root_signature;
};

struct ComputePipelineState
{
	ID3D12PipelineState* pso;
	ID3D12RootSignature* root_signature;
	u32 num_threads_x;
	u32 num_threads_y;
	u32 num_threads_z;
};

struct CompiledShader
{
	IDxcBlob* blob;
	u32 num_constant_params;
	u32 num_threads_x;
	u32 num_threads_y;
	u32 num_threads_z;
};

static CompiledShader compile_shader(UTF8StringView const& shader_path, ShaderType type, IDxcIncludeHandler* include_handler, IDxcCompiler3* compiler, IDxcUtils* utils, ArenaAllocator& allocator, Platform& platform);

static GraphicsPipelineState create_graphics_pipeline_state_object(
	UTF8StringView const& shader_path, UTF8StringView const& name, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
	IDxcIncludeHandler* include_handler, IDxcCompiler3* compiler, IDxcUtils* utils, ID3D12Device* device,
	Platform& platform, ArenaAllocator& allocator);

static constexpr D3D12_HEAP_PROPERTIES g_upload_heap_props{
	.Type = D3D12_HEAP_TYPE_UPLOAD,
	.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
	.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
	.CreationNodeMask = 1,
	.VisibleNodeMask = 1,
};

static constexpr D3D12_HEAP_PROPERTIES g_main_heap_props{
	.Type = D3D12_HEAP_TYPE_DEFAULT,
	.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
	.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
	.CreationNodeMask = 1,
	.VisibleNodeMask = 1,
};

static constexpr DescriptorIndex g_null_descriptor_index{u32(-1)};

static inline bool operator==(DescriptorIndex a, DescriptorIndex b)
{
	return a.value == b.value;
}

struct DescriptorPool
{
	ID3D12DescriptorHeap* heap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE start_cpu_handle;
	D3D12_GPU_DESCRIPTOR_HANDLE start_gpu_handle;
	u32 handle_increment_size;
	Slice<u32> free_handles;
	usize free_handle_count;
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	D3D12_DESCRIPTOR_HEAP_FLAGS flags;

	void init(ID3D12Device12* device, ArenaAllocator& persistent_allocator, D3D12_DESCRIPTOR_HEAP_TYPE in_type, D3D12_DESCRIPTOR_HEAP_FLAGS in_flags, u32 count)
	{
		const D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc{
			.Type = in_type,
			.NumDescriptors = count,
			.Flags = in_flags,
		};

		HRESULT result = device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&heap));
		PAW_ASSERT_UNUSED(result);
		PAW_ASSERT(SUCCEEDED(result));

		start_cpu_handle = heap->GetCPUDescriptorHandleForHeapStart();

		if ((in_flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			start_gpu_handle = heap->GetGPUDescriptorHandleForHeapStart();
		}
		handle_increment_size = device->GetDescriptorHandleIncrementSize(descriptor_heap_desc.Type);

		free_handles = persistent_allocator.alloc<u32>(count);
		for (u32 i = 0; i < count; i++)
		{
			// Put them in reverse order so we use the lower numbers first
			free_handles[i] = count - i - 1;
		}

		free_handle_count = free_handles.size;
		type = in_type;
		flags = in_flags;
	}

	void deinit()
	{
		heap->Release();
	}

	DescriptorIndex alloc()
	{
		PAW_ASSERT(free_handle_count > 0);
		free_handle_count--;
		const u32 free_handle = free_handles[free_handle_count];
		free_handles[free_handle_count] = 0xDEADBEEF;
		return {free_handle};
	}

	void free(DescriptorIndex index)
	{
		PAW_ASSERT(free_handle_count < free_handles.size);
		PAW_ASSERT(free_handles[free_handle_count] == 0xDEADBEEF);
		free_handles[free_handle_count] = index.value;
		free_handle_count++;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE get_cpu(DescriptorIndex index)
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE{start_cpu_handle.ptr + index.value * handle_increment_size};
	}

	D3D12_GPU_DESCRIPTOR_HANDLE get_gpu(DescriptorIndex index)
	{
		PAW_ASSERT((flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		return D3D12_GPU_DESCRIPTOR_HANDLE{start_gpu_handle.ptr + index.value * handle_increment_size};
	}
};

typedef u64 GpuImageHandle;

struct ImageStore2
{
	enum SlotState
	{
		SlotState_Empty,
		SlotState_Active,
		SlotState_ReadyForDeletion,
	};

	static constexpr usize max_images = 256;
	static constexpr UTF8StringView empty_name = "Empty"_str;
	D3D12_RESOURCE_DESC1 descs[max_images];
	ID3D12Resource* resources[max_images];
	u32 ref_counts[max_images];
	usize most_recent_frame_uses[max_images];
	SlotState states[max_images];
	UTF8StringView debug_names[max_images];
	DescriptorIndex srv_descriptor_indices[max_images];
	DescriptorIndex rtv_descriptor_indices[max_images];
	DescriptorIndex dsv_descriptor_indices[max_images];
	Vec2S32 sizes[max_images];

	static constexpr usize null_frame_index = usize(-1);

	void init()
	{
		for (usize i = 0; i < max_images; i++)
		{
			ref_counts[i] = 0;
			most_recent_frame_uses[i] = null_frame_index;
			debug_names[i] = empty_name;
			states[i] = SlotState_Empty;
			resources[i] = nullptr;
			srv_descriptor_indices[i] = g_null_descriptor_index;
			rtv_descriptor_indices[i] = g_null_descriptor_index;
			dsv_descriptor_indices[i] = g_null_descriptor_index;
			sizes[i] = Vec2S32{-1, -1};
		}
	}

	void deinit(DescriptorPool& srv_pool, DescriptorPool& rtv_pool, DescriptorPool& dsv_pool, usize current_frame_index)
	{
		for (usize i = 0; i < max_images; i++)
		{
			PAW_ASSERT(states[i] == SlotState_Empty || states[i] == SlotState_ReadyForDeletion);
			if (states[i] == SlotState_ReadyForDeletion)
			{
				delete_image(i, srv_pool, rtv_pool, dsv_pool, current_frame_index);
			}
		}
	}

	void delete_pending_images(DescriptorPool& srv_pool, DescriptorPool& rtv_pool, DescriptorPool& dsv_pool, usize current_frame_index)
	{
		PAW_PROFILER_FUNC();
		for (usize i = 0; i < max_images; i++)
		{
			if (states[i] == SlotState_ReadyForDeletion && (most_recent_frame_uses[i] == null_frame_index || most_recent_frame_uses[i] + g_frames_in_flight < current_frame_index))
			{
				delete_image(i, srv_pool, rtv_pool, dsv_pool, current_frame_index);
			}
		}
	}
	void delete_image(usize index, DescriptorPool& srv_pool, DescriptorPool& rtv_pool, DescriptorPool& dsv_pool, usize current_frame_index)
	{
		PAW_ASSERT(ref_counts[index] == 0);

		resources[index]->Release();

		if (srv_descriptor_indices[index] != g_null_descriptor_index)
		{
			srv_pool.free(srv_descriptor_indices[index]);
		}

		if (rtv_descriptor_indices[index] != g_null_descriptor_index)
		{
			rtv_pool.free(rtv_descriptor_indices[index]);
		}

		if (dsv_descriptor_indices[index] != g_null_descriptor_index)
		{
			dsv_pool.free(dsv_descriptor_indices[index]);
		}

		PAW_LOG_INFO(
			"Deleted %p{str}, last used in frame %llu, current frame is %llu",
			&debug_names[index],
			most_recent_frame_uses[index],
			current_frame_index);

		states[index] = SlotState_Empty;
		debug_names[index] = empty_name;
		resources[index] = nullptr;
		most_recent_frame_uses[index] = null_frame_index;
		srv_descriptor_indices[index] = g_null_descriptor_index;
		rtv_descriptor_indices[index] = g_null_descriptor_index;
		dsv_descriptor_indices[index] = g_null_descriptor_index;
		sizes[index] = Vec2S32{-1, -1};
	}

	usize grab_first_free_slot()
	{
		for (usize i = 0; i < max_images; i++)
		{
			if (states[i] == SlotState_Empty)
			{
				states[i] = SlotState_Active;
				return i;
			}
		}

		PAW_UNREACHABLE;
		return ~0u;
	}

	GpuImageHandle create_and_allocate(
		usize width, usize height, DXGI_FORMAT format, const D3D12_HEAP_PROPERTIES& heap_props, ID3D12Device12* device, D3D12_RESOURCE_FLAGS flags, D3D12_BARRIER_LAYOUT initial_layout, UTF8StringView debug_name = "Unnamed"_str,
		u32 sample_count = 1, const D3D12_CLEAR_VALUE* clear_value = nullptr)
	{
		const D3D12_RESOURCE_DESC1 desc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = width,
			.Height = UINT(height),
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = format,
			.SampleDesc = {
				.Count = sample_count,
				.Quality = 0,
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = flags,
		};

		const usize free_index = grab_first_free_slot();

		HRESULT result = device->CreateCommittedResource3(&heap_props, D3D12_HEAP_FLAG_NONE, &desc, initial_layout, clear_value, nullptr, 0, nullptr, IID_PPV_ARGS(&resources[free_index]));
		PAW_ASSERT(SUCCEEDED(result));
		PAW_ASSERT_UNUSED(result);

		set_debug_name(resources[free_index], debug_name);

		ref_counts[free_index] = 1;
		debug_names[free_index] = debug_name;
		descs[free_index] = desc;
		sizes[free_index] = Vec2S32{s32(width), s32(height)};

		PAW_LOG_INFO("Created %p{str}", &debug_names[free_index]);

		return GpuImageHandle{free_index};
	}

	GpuImageHandle create_placed(
		usize width, usize height, DXGI_FORMAT format, ID3D12Heap* heap, usize heap_offset, ID3D12Device12* device, D3D12_RESOURCE_FLAGS flags, D3D12_BARRIER_LAYOUT initial_layout, UTF8StringView debug_name = "Unnamed"_str,
		u32 sample_count = 1, const D3D12_CLEAR_VALUE* clear_value = nullptr)
	{
		const D3D12_RESOURCE_DESC1 desc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = width,
			.Height = UINT(height),
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = format,
			.SampleDesc = {
				.Count = sample_count,
				.Quality = 0,
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = flags,
		};

		const usize free_index = grab_first_free_slot();

		HRESULT result = device->CreatePlacedResource2(heap, heap_offset, &desc, initial_layout, clear_value, 0, nullptr, IID_PPV_ARGS(&resources[free_index]));
		PAW_ASSERT(SUCCEEDED(result));
		PAW_ASSERT_UNUSED(result);

		set_debug_name(resources[free_index], debug_name);

		ref_counts[free_index] = 1;
		debug_names[free_index] = debug_name;
		descs[free_index] = desc;
		sizes[free_index] = Vec2S32{s32(width), s32(height)};

		PAW_LOG_INFO("Created %p{str}", &debug_names[free_index]);

		return GpuImageHandle{free_index};
	}

	void remove_ref(GpuImageHandle handle)
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		PAW_ASSERT(ref_counts[handle] > 0);

		ref_counts[handle]--;
		if (ref_counts[handle] == 0)
		{
			// PAW_ASSERT(most_recent_frame_uses[handle] != null_frame_index);
			PAW_LOG_INFO("%p{str} (%llu) ready for deletion", &debug_names[handle], handle);
			states[handle] = SlotState_ReadyForDeletion;
		}
	}

	void add_ref(GpuImageHandle handle)
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		PAW_ASSERT(ref_counts[handle] > 0);
		ref_counts[handle]++;
	}

	void add_frame_usage(GpuImageHandle handle, usize frame_index)
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		PAW_ASSERT(ref_counts[handle] > 0);
		// if (frame_index > most_recent_frame_uses[handle])
		{
			// PAW_LOG_INFO("Using %p{str} (%llu) on frame %llu", &debug_names[handle], handle, frame_index);
			most_recent_frame_uses[handle] = frame_index;
		}
	}

	ID3D12Resource* get_resource(GpuImageHandle handle) const
	{
		PAW_ASSERT(states[handle] == SlotState_Active);

		return resources[handle];
	}

	DescriptorIndex get_srv_descriptor_index(GpuImageHandle handle) const
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		const DescriptorIndex result = srv_descriptor_indices[handle];
		PAW_ASSERT(result != g_null_descriptor_index);
		return result;
	}

	DescriptorIndex create_srv(GpuImageHandle handle, DescriptorPool& srv_pool, ID3D12Device12* device)
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		PAW_ASSERT(srv_descriptor_indices[handle] == g_null_descriptor_index);

		PAW_ASSERT(srv_pool.type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		const D3D12_RESOURCE_DESC1& desc = descs[handle];

		const D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{
			.Format = desc.Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MostDetailedMip = 0,
				.MipLevels = desc.MipLevels,
			},
		};

		DescriptorIndex index = srv_pool.alloc();
		device->CreateShaderResourceView(resources[handle], &srv_desc, srv_pool.get_cpu(index));
		srv_descriptor_indices[handle] = index;
		return index;
	}

	DescriptorIndex get_rtv_descriptor_index(GpuImageHandle handle) const
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		const DescriptorIndex result = rtv_descriptor_indices[handle];
		PAW_ASSERT(result != g_null_descriptor_index);
		return result;
	}

	DescriptorIndex create_rtv(GpuImageHandle handle, DescriptorPool& srv_pool, ID3D12Device12* device)
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		PAW_ASSERT(rtv_descriptor_indices[handle] == g_null_descriptor_index);

		PAW_ASSERT(srv_pool.type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		const D3D12_RESOURCE_DESC1& desc = descs[handle];

		const D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{
			.Format = desc.Format,
			.ViewDimension = desc.SampleDesc.Count > 1 ? D3D12_RTV_DIMENSION_TEXTURE2DMS : D3D12_RTV_DIMENSION_TEXTURE2D,
		};

		DescriptorIndex index = srv_pool.alloc();

		device->CreateRenderTargetView(resources[handle], &rtv_desc, srv_pool.get_cpu(index));
		rtv_descriptor_indices[handle] = index;
		return index;
	}

	DescriptorIndex get_dsv_descriptor_index(GpuImageHandle handle) const
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		const DescriptorIndex result = dsv_descriptor_indices[handle];
		PAW_ASSERT(result != g_null_descriptor_index);
		return result;
	}

	DescriptorIndex create_dsv(GpuImageHandle handle, DescriptorPool& dsv_pool, ID3D12Device12* device)
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		PAW_ASSERT(dsv_descriptor_indices[handle] == g_null_descriptor_index);

		PAW_ASSERT(dsv_pool.type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		const D3D12_RESOURCE_DESC1& desc = descs[handle];

		const D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{
			.Format = desc.Format,
			.ViewDimension = desc.SampleDesc.Count > 1 ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D,
		};

		DescriptorIndex index = dsv_pool.alloc();

		device->CreateDepthStencilView(resources[handle], &dsv_desc, dsv_pool.get_cpu(index));
		dsv_descriptor_indices[handle] = index;
		return index;
	}

	const D3D12_RESOURCE_DESC1& get_resource_desc(GpuImageHandle handle) const
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		return descs[handle];
	}

	Vec2S32 const& get_size(GpuImageHandle handle) const
	{
		PAW_ASSERT(states[handle] == SlotState_Active);
		Vec2S32 const& result = sizes[handle];
		PAW_ASSERT(result.x != -1 && result.y != -1);
		return result;
	}
};

struct DX12Renderer;

namespace render_graph
{
	enum class Format
	{
		R16G16B16A16_Float,
		R8G8B8A8_Unorm,
		R32_Float,
		Depth32_Float,
		Count,
	};

	static constexpr usize g_texture_format_sizes_bytes[(int)Format::Count]{
		/* R16G16B16A16_Float */ 8,
		/* R8G8B8A8_Unorm */ 4,
		/* R32_Float */ 4,
		/* Depth32_Float */ 4,
	};

	static constexpr DXGI_FORMAT g_texture_format_map[(int)Format::Count]{
		/* R16G16B16A16_Float */ DXGI_FORMAT_R16G16B16A16_FLOAT,
		/* R8G8B8A8_Unorm */ DXGI_FORMAT_R8G8B8A8_UNORM,
		/* R32_Float */ DXGI_FORMAT_R32_FLOAT,
		/* Depth32_Float */ DXGI_FORMAT_D32_FLOAT,
	};

	static constexpr DXGI_FORMAT g_srv_format_map[int(Format::Count)]{
		/* R16G16B16A16_Float */ DXGI_FORMAT_R16G16B16A16_FLOAT,
		/* R8G8B8A8_Unorm */ DXGI_FORMAT_R8G8B8A8_UNORM,
		/* R32_Float */ DXGI_FORMAT_R32_FLOAT,
		/* Depth32_Float */ DXGI_FORMAT_R32_FLOAT,
	};

	// These should be ordered by stage order

	union ClearValue_t
	{
		f32 color[4];
		struct
		{
			f32 depth;
			u8 stencil;
		} depth_stencil;
	};

	PAW_ASSERT_IS_POD(ClearValue_t);

	enum class Access
	{
		None,
		RenderTarget,
		VertexShader,
		PixelShader,
		Depth,
		Stencil,
		Count,
	};

	struct AccessFlags
	{
		u32 flags;

		bool contains(Access access) const
		{
			return (flags & (1 << u32(access))) > 0;
		}

		void operator=(Access rhs)
		{
			flags = 1 << u32(rhs);
		}
	};

	static constexpr inline AccessFlags operator|(Access lhs, Access rhs)
	{
		return AccessFlags((1 << u32(lhs)) | (1 << u32(rhs)));
	}

	static void operator|=(AccessFlags& lhs, Access rhs)
	{
		lhs.flags |= 1 << u32(rhs);
	}

	static constexpr D3D12_BARRIER_SYNC g_access_to_sync_map[int(Access::Count)]{
		D3D12_BARRIER_SYNC_NONE,
		D3D12_BARRIER_SYNC_RENDER_TARGET,
		D3D12_BARRIER_SYNC_VERTEX_SHADING,
		D3D12_BARRIER_SYNC_PIXEL_SHADING,
		D3D12_BARRIER_SYNC_DEPTH_STENCIL,
		D3D12_BARRIER_SYNC_DEPTH_STENCIL,
	};

	static constexpr D3D12_BARRIER_ACCESS g_access_to_access_map[int(Access::Count)]{
		D3D12_BARRIER_ACCESS_NO_ACCESS,
		D3D12_BARRIER_ACCESS_RENDER_TARGET,
		D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
		D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
		D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE,
		D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE,
	};

	static constexpr D3D12_BARRIER_LAYOUT g_access_to_layout_map[int(Access::Count)]{
		D3D12_BARRIER_LAYOUT_UNDEFINED,
		D3D12_BARRIER_LAYOUT_RENDER_TARGET,
		D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
		D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
		D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE,
		D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE,
	};

	enum class InitialState
	{
		Undefined,
		Clear,
	};

	struct Resource
	{
		UTF8StringView name;
		usize width;
		usize height;
		Format format;
		u32 sample_count;
		usize ref_count;
		usize index;
		AccessFlags all_accesses;
		Access first_access;
		Access last_access;

		InitialState inital_state;
		ClearValue_t clear_value;
	};

	struct ResourceInstance;
	struct ResourceInstanceRef;
	class RuntimeRenderGraph;
	struct RuntimeGraphPass_t;

	typedef void RenderGraphNodeFunc(RuntimeRenderGraph const&, RuntimeGraphPass_t const&, ID3D12GraphicsCommandList9* command_list, DX12Renderer& self);

	struct Pass
	{
		UTF8StringView name;
		RenderGraphNodeFunc* func;
		ResourceInstanceRef* first_write_ref;
		ResourceInstanceRef* current_write_ref;
		ResourceInstanceRef* first_read_ref;
		ResourceInstanceRef* current_read_ref;
		Pass* next_pass;
		usize index;
		usize final_index;
	};

	struct PassRef
	{
		Pass* pass;
		PassRef* next_ref;
	};

	struct ResourceInstance
	{
		Resource* resource;
		usize ref_index;

		Pass* writer_ref;
		PassRef* first_reader_ref;
		PassRef* current_reader_ref;
	};

	struct ResourceInstanceRef
	{
		ResourceInstance* instance;
		ResourceInstanceRef* next;
		Access access;
	};

	struct RuntimeGraphResource_t
	{
	};

	PAW_ASSERT_IS_POD(RuntimeGraphResource_t);

	struct RuntimeGraphResourceDebugData_t
	{
		UTF8StringView name;
		u64 size_bytes;
		u64 offset_bytes;
		u32 start_pass_index;
		u32 end_pass_index;
	};

	PAW_ASSERT_IS_POD(RuntimeGraphResourceDebugData_t);

	struct RuntimeGraphPassInput_t
	{
		u32 resource_index;
	};

	PAW_ASSERT_IS_POD(RuntimeGraphPassInput_t);

	struct RuntimeGraphPassOutput_t
	{
		u32 resource_index;
	};

	PAW_ASSERT_IS_POD(RuntimeGraphPassOutput_t);

	struct RuntimeGraphPass_t
	{
		RenderGraphNodeFunc* func;
		u32 inputs_start_index;
		u32 input_count;
		u32 outputs_start_index;
		u32 output_count;
		u32 barrier_groups_start_index;
		u32 barrier_group_count;
		u32 color_target_start_index;
		u32 color_target_count;
		u32 depth_target_index;
		u32 clear_color_start_index;
		u32 clear_color_count;
		u32 clear_depth_start_index;
		u32 clear_depth_count;
		u32 discard_start_index;
		u32 discard_count;
	};

	static constexpr u32 g_null_depth_target_index = u32(-1);

	PAW_ASSERT_IS_POD(RuntimeGraphPass_t);

	struct RuntimeGraphPassDebugData_t
	{
		UTF8StringView name;
	};

	PAW_ASSERT_IS_POD(RuntimeGraphPassDebugData_t);

	struct RuntimeGraphClearColorCommand_t
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
		f32 color[4];
	};

	PAW_ASSERT_IS_POD(RuntimeGraphClearColorCommand_t);

	struct RuntimeGraphClearDepthCommand_t
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;
		D3D12_CLEAR_FLAGS flags;
		f32 depth;
		u8 stencil;
	};

	PAW_ASSERT_IS_POD(RuntimeGraphClearDepthCommand_t);

	class RuntimeRenderGraph
	{
	public:
		Slice<RuntimeGraphResource_t> resources;
		Slice<RuntimeGraphPass_t> passes;
		Slice<RuntimeGraphPassInput_t> inputs;
		Slice<RuntimeGraphPassOutput_t> outputs;
		Slice<D3D12_VIEWPORT> viewports;
		Slice<D3D12_RECT> scissors;

		Slice2D<D3D12_BARRIER_GROUP> barrier_groups;				   // Per-frame
		Slice2D<D3D12_TEXTURE_BARRIER> texture_barriers;			   // Per-frame
		Slice2D<D3D12_CPU_DESCRIPTOR_HANDLE> color_target_handles;	   // Per-frame
		Slice2D<D3D12_CPU_DESCRIPTOR_HANDLE> depth_target_handles;	   // Per-frame
		Slice2D<RuntimeGraphClearColorCommand_t> clear_color_commands; // Per-frame
		Slice2D<RuntimeGraphClearDepthCommand_t> clear_depth_commands; // Per-frame
		Slice2D<ID3D12Resource1*> discard_commands;					   // Per-frame

		Slice<RuntimeGraphPassDebugData_t> passes_debug_data;
		Slice<RuntimeGraphResourceDebugData_t> resources_debug_data;

		ID3D12Heap* heap;
		u64 total_frame_heap_size_bytes;
		u64 total_heap_size_bytes;
	};

	struct RenderGraphBuilder
	{
		ArenaAllocator allocator;
		usize pass_count;
		Pass* first_pass;
		Pass* current_pass;
		usize resource_count;

		void init(PagePool& page_pool)
		{
			allocator.init(&page_pool, "RenderGraphBuilder Scratch Mem"_str);
			reset();
		}

		void reset()
		{
			pass_count = 0;
			first_pass = nullptr;
			current_pass = nullptr;
			resource_count = 0;
		}

		void pass_read(Pass& pass, ResourceInstance* handle, Access access)
		{
			handle->resource->all_accesses |= access;
			handle->resource->last_access = access;

			PassRef& pass_ref = allocator.alloc_and_construct_single<PassRef>();
			pass_ref.next_ref = nullptr;
			pass_ref.pass = &pass;

			PAW_ASSERT(handle->ref_index >= handle->resource->ref_count);

			if (handle->first_reader_ref == nullptr)
			{
				handle->first_reader_ref = &pass_ref;
			}
			else
			{
				handle->current_reader_ref->next_ref = &pass_ref;
			}
			handle->current_reader_ref = &pass_ref;

			ResourceInstanceRef& instance_ref = allocator.alloc_and_construct_single<ResourceInstanceRef>();
			instance_ref.instance = handle;
			instance_ref.next = nullptr;
			instance_ref.access = access;

			if (pass.first_read_ref == nullptr)
			{
				pass.first_read_ref = &instance_ref;
			}
			else
			{
				pass.current_read_ref->next = &instance_ref;
			}
			pass.current_read_ref = &instance_ref;
		}

		ResourceInstance* pass_write(Pass& pass, ResourceInstance* handle, Access access)
		{
			handle->resource->ref_count++;
			handle->resource->all_accesses |= access;
			handle->resource->last_access = access;

			PassRef& pass_ref = allocator.alloc_and_construct_single<PassRef>();
			pass_ref.next_ref = nullptr;
			pass_ref.pass = &pass;

			if (handle->first_reader_ref == nullptr)
			{
				handle->first_reader_ref = &pass_ref;
			}
			else
			{
				handle->current_reader_ref->next_ref = &pass_ref;
			}
			handle->current_reader_ref = &pass_ref;

			ResourceInstance& ref = allocator.alloc_and_construct_single<ResourceInstance>();
			ref.resource = handle->resource;
			ref.ref_index = handle->resource->ref_count;

			ref.writer_ref = &pass;

			{

				ResourceInstanceRef& instance_ref = allocator.alloc_and_construct_single<ResourceInstanceRef>();
				instance_ref.instance = &ref;
				instance_ref.next = nullptr;
				instance_ref.access = access;

				if (pass.first_write_ref == nullptr)
				{
					pass.first_write_ref = &instance_ref;
				}
				else
				{
					pass.current_write_ref->next = &instance_ref;
				}
				pass.current_write_ref = &instance_ref;
			}

			{
				ResourceInstanceRef& instance_ref = allocator.alloc_and_construct_single<ResourceInstanceRef>();
				instance_ref.instance = handle;
				instance_ref.next = nullptr;
				instance_ref.access = access;

				if (pass.first_read_ref == nullptr)
				{
					pass.first_read_ref = &instance_ref;
				}
				else
				{
					pass.current_read_ref->next = &instance_ref;
				}
				pass.current_read_ref = &instance_ref;
			}

			return &ref;
		}

		ResourceInstance* pass_create_texture(Pass& pass, Access access, usize width, usize height, Format format, UTF8StringView name, InitialState initial_state, ClearValue_t clear_value, u32 sample_count = 1)
		{
			PAW_ASSERT(sample_count == 1);

			Resource& resource = allocator.alloc_and_construct_single<Resource>();
			resource.width = width;
			resource.height = height;
			resource.format = format;
			resource.sample_count = sample_count;
			resource.name = name;
			resource.ref_count = 0;
			resource.index = resource_count++;
			resource.all_accesses = access;
			resource.first_access = access;
			resource.last_access = access;
			resource.inital_state = initial_state;
			resource.clear_value = clear_value;

			ResourceInstance& ref = allocator.alloc_and_construct_single<ResourceInstance>();
			ref.resource = &resource;
			ref.ref_index = resource.ref_count;

			ref.writer_ref = &pass;

			ResourceInstanceRef& instance_ref = allocator.alloc_and_construct_single<ResourceInstanceRef>();
			instance_ref.instance = &ref;
			instance_ref.next = nullptr;
			instance_ref.access = access;

			if (pass.first_write_ref == nullptr)
			{
				pass.first_write_ref = &instance_ref;
			}
			else
			{
				pass.current_write_ref->next = &instance_ref;
			}
			pass.current_write_ref = &instance_ref;

			return &ref;
		}

		Pass& add_pass(UTF8StringView name, RenderGraphNodeFunc* func)
		{
			Pass& pass = allocator.alloc_and_construct_single<Pass>();
			pass.name = name;
			pass.func = func;
			pass.next_pass = nullptr;
			pass.first_read_ref = nullptr;
			pass.current_read_ref = nullptr;
			pass.first_write_ref = nullptr;
			pass.current_write_ref = nullptr;
			pass.index = pass_count++;

			if (first_pass == nullptr)
			{
				first_pass = &pass;
			}
			else
			{
				current_pass->next_pass = &pass;
			}
			current_pass = &pass;

			return pass;
		}
	};

	void opaque_node(RuntimeRenderGraph const& /*graph*/, RuntimeGraphPass_t const& /*pass*/, ID3D12GraphicsCommandList9* command_list, DX12Renderer& temp_renderer);

	struct RenderGraph
	{
		RenderGraphBuilder graph;
		RuntimeRenderGraph runtime_graph;
	};

	static void process_pass(Pass* pass, Slice<u32> const& visited, Slice<u32> const& on_stack, Slice<Pass const*> const& final_passes, usize& final_write_index)
	{
		const usize index = pass->index;
		if (visited[index])
		{
			PAW_ASSERT(!on_stack[index]); // Circular dependency detected
			return;
		}
		visited[index] = true;
		on_stack[index] = true;

		for (ResourceInstanceRef* write_ref = pass->first_write_ref; write_ref; write_ref = write_ref->next)
		{
			for (PassRef* read_ref = write_ref->instance->first_reader_ref; read_ref; read_ref = read_ref->next_ref)
			{
				Pass* dep_pass = read_ref->pass;
				process_pass(dep_pass, visited, on_stack, final_passes, final_write_index);
			}
		}
		const usize new_index = final_passes.size - final_write_index - 1;
		pass->final_index = new_index;
		final_passes[new_index] = pass;
		++final_write_index;
		on_stack[index] = false;
	}

	RenderGraph& init(ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, PagePool& page_pool, PagePool& /*debug_page_pool*/, ID3D12Device12* device, DescriptorPool& rtv_descriptor_pool, DescriptorPool& srv_pool, DescriptorPool& dsv_descriptor_pool)
	{
		RenderGraph& self = persistent_allocator.alloc_and_construct_single<RenderGraph>();

		self.graph.init(page_pool);

		RenderGraphBuilder& graph = self.graph;

		Pass& light_bin_pass = graph.add_pass("Lighting_Bin"_str, nullptr);
		ResourceInstance* light_bin = graph.pass_create_texture(light_bin_pass, Access::PixelShader, 100, 100, Format::R32_Float, "Light_Bin"_str, InitialState::Clear, {.color = {0.0f, 0.0f, 0.0f, 0.0f}});

		Pass& opaque_pass = graph.add_pass("Opaque"_str, &opaque_node);
		ResourceInstance* color_rt = graph.pass_create_texture(opaque_pass, Access::RenderTarget, 1920, 1080, Format::R16G16B16A16_Float, "Color_RT"_str, InitialState::Clear, {.color = {0.33f, 0.33f, 0.33f, 1.0f}});
		ResourceInstance* depth_rt = graph.pass_create_texture(opaque_pass, Access::Depth, 1920, 1080, Format::Depth32_Float, "Depth_RT"_str, InitialState::Clear, {.depth_stencil = {.depth = 0.0f, .stencil = 0}});

		Pass& translucent_pass = graph.add_pass("Translucents"_str, nullptr);
		color_rt = graph.pass_write(translucent_pass, color_rt, Access::RenderTarget);
		graph.pass_read(translucent_pass, depth_rt, Access::Depth);

		Pass& lighting_pass = graph.add_pass("Lighting"_str, nullptr);
		graph.pass_read(lighting_pass, color_rt, Access::PixelShader);
		graph.pass_read(lighting_pass, depth_rt, Access::PixelShader);
		graph.pass_read(lighting_pass, light_bin, Access::PixelShader);

		ResourceInstance* lighting_out = graph.pass_create_texture(lighting_pass, Access::RenderTarget, 1920, 1080, Format::R16G16B16A16_Float, "Lighting_Out"_str, InitialState::Undefined, {});

		Pass& grade_pass = graph.add_pass("Grade"_str, nullptr);
		ResourceInstance* grade_out = graph.pass_create_texture(grade_pass, Access::RenderTarget, 1920, 1080, Format::R16G16B16A16_Float, "Grade_Out"_str, InitialState::Undefined, {});
		graph.pass_read(grade_pass, lighting_out, Access::PixelShader);

		Pass& tonemap_pass = graph.add_pass("Tonemap"_str, nullptr);
		graph.pass_create_texture(tonemap_pass, Access::RenderTarget, 1920, 1080, Format::R8G8B8A8_Unorm, "Tonemap_Out"_str, InitialState::Undefined, {});
		graph.pass_read(tonemap_pass, grade_out, Access::PixelShader);

		// for (Pass* pass = graph.first_pass; pass; pass = pass->next_pass)
		//{
		//	for (ResourceInstanceRef* write_ref = pass->first_write_ref; write_ref; write_ref = write_ref->next)
		//	{
		//		printf("\t%s{%s}-->%s_%llu;\n", pass->name.ptr, pass->name.ptr, write_ref->instance->resource->name.ptr, write_ref->instance->ref_index);
		//	}
		//	for (ResourceInstanceRef* read_ref = pass->first_read_ref; read_ref; read_ref = read_ref->next)
		//	{
		//		printf("\t%s_%llu-.->%s{%s};\n", read_ref->instance->resource->name.ptr, read_ref->instance->ref_index, pass->name.ptr, pass->name.ptr);
		//	}
		// }

		// PAW_LOG_INFO("====================================================================================================");

		// for (Pass* pass = graph.first_pass; pass; pass = pass->next_pass)
		//{
		//	for (ResourceInstanceRef* read_ref = pass->first_read_ref; read_ref; read_ref = read_ref->next)
		//	{
		//		const Pass* dep_pass = read_ref->instance->writer_ref;
		//		printf("\t%s-->|%s_%llu|%s;\n", pass->name.ptr, read_ref->instance->resource->name.ptr, read_ref->instance->ref_index, dep_pass->name.ptr);
		//	}
		// }

		// PAW_LOG_INFO("====================================================================================================");

		Slice<u32> const visited_passes = graph.allocator.alloc<u32>(graph.pass_count);
		std::memset(visited_passes.ptr, 0, visited_passes.calc_total_size_in_bytes());
		Slice<u32> const on_stack = graph.allocator.alloc<u32>(graph.pass_count);
		std::memset(on_stack.ptr, 0, on_stack.calc_total_size_in_bytes());
		Slice<Pass const*> const topological_order = graph.allocator.alloc<Pass const*>(graph.pass_count);
		usize final_write_index = 0;

		for (Pass* pass = graph.first_pass; pass; pass = pass->next_pass)
		{
			process_pass(pass, visited_passes, on_stack, topological_order, final_write_index);
		}

		// for (const Pass* pass : topological_order)
		//{
		//	printf("%s\n", pass->name.ptr);
		// }

		// PAW_LOG_INFO("====================================================================================================");

		Slice<usize> const distances = graph.allocator.alloc<usize>(graph.pass_count);
		std::memset(distances.ptr, 0, distances.calc_total_size_in_bytes());
		for (Pass const* pass : topological_order)
		{
			for (ResourceInstanceRef* write_ref = pass->first_write_ref; write_ref; write_ref = write_ref->next)
			{
				for (PassRef* read_ref = write_ref->instance->first_reader_ref; read_ref; read_ref = read_ref->next_ref)
				{
					Pass* dep_pass = read_ref->pass;
					if (distances[dep_pass->final_index] < distances[pass->final_index] + 1)
					{
						distances[dep_pass->final_index] = distances[pass->final_index] + 1;
					}
				}
			}
		}

		// for (const Pass* pass : topological_order)
		//{
		//	PAW_LOG_INFO("%p{str}: %llu", &pass->name, distances[pass->final_index]);
		// }

		struct ResourceLifetime
		{
			Resource* resource;
			usize start_pass_index;
			usize end_pass_index;

			D3D12_RESOURCE_DESC1 desc;
			usize total_size_bytes;
			usize alignment_bytes;
		};

		// PAW_LOG_INFO("====================================================================================================");

		Slice<ResourceLifetime> const resource_lifetimes = graph.allocator.alloc<ResourceLifetime>(graph.resource_count);
		for (ResourceLifetime& lifetime : resource_lifetimes)
		{
			lifetime.resource = nullptr;
			lifetime.start_pass_index = graph.pass_count;
			lifetime.end_pass_index = 0;
		}

		for (Pass const* pass : topological_order)
		{
			const usize pass_index = pass->final_index;
			for (ResourceInstanceRef* write_ref = pass->first_write_ref; write_ref; write_ref = write_ref->next)
			{
				Resource* resource = write_ref->instance->resource;
				ResourceLifetime& lifetime = resource_lifetimes[resource->index];
				lifetime.resource = resource;
				if (pass_index < lifetime.start_pass_index)
				{
					lifetime.start_pass_index = pass_index;
				}

				if (pass_index > lifetime.end_pass_index)
				{
					lifetime.end_pass_index = pass_index;
				}
			}

			for (ResourceInstanceRef* read_ref = pass->first_read_ref; read_ref; read_ref = read_ref->next)
			{
				Resource* resource = read_ref->instance->resource;
				ResourceLifetime& lifetime = resource_lifetimes[resource->index];
				if (pass_index < lifetime.start_pass_index)
				{
					lifetime.start_pass_index = pass_index;
				}

				if (pass_index > lifetime.end_pass_index)
				{
					lifetime.end_pass_index = pass_index;
				}
			}
		}

		Slice<ResourceLifetime> const sorted_resource_lifetimes = graph.allocator.alloc<ResourceLifetime>(resource_lifetimes.size);
		memCopy(resource_lifetimes.ptr, sorted_resource_lifetimes.ptr, resource_lifetimes.calc_total_size_in_bytes());

		/*printf("section Passes\n");
		for (usize i = 0; i < topological_order.size; ++i)
		{
			const Pass* pass = topological_order[i];
			printf("%s: %llu, %llu\n", pass->name.ptr, i, i + 1);
		}

		printf("section Resources\n");
		for (const ResourceLifetime& lifetime : resource_lifetimes)
		{
			printf("%s: %llu, %llu\n", lifetime.resource->name.ptr, lifetime.start_pass_index, lifetime.end_pass_index + 1);
		}*/

		for (ResourceLifetime& lifetime : sorted_resource_lifetimes)
		{
			Resource* const resource = lifetime.resource;

			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

			if (resource->all_accesses.contains(Access::RenderTarget))
			{
				flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			}

			if (resource->all_accesses.contains(Access::Depth))
			{
				flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			}

			lifetime.desc = D3D12_RESOURCE_DESC1{
				.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
				.Alignment = 0,
				.Width = resource->width,
				.Height = UINT(resource->height),
				.DepthOrArraySize = 1,
				.MipLevels = 1,
				.Format = g_texture_format_map[int(resource->format)],
				.SampleDesc = {
					.Count = resource->sample_count,
					.Quality = 0,
				},
				.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN, // #TODO: Maybe this should be in the first layout detected in the graph
				.Flags = flags,
			};

			D3D12_RESOURCE_ALLOCATION_INFO const alloc_info = device->GetResourceAllocationInfo2(0, 1, &lifetime.desc, nullptr);
			lifetime.total_size_bytes = alloc_info.SizeInBytes;
			lifetime.alignment_bytes = alloc_info.Alignment;
		}

		std::sort(sorted_resource_lifetimes.ptr, sorted_resource_lifetimes.ptr + sorted_resource_lifetimes.size, [](ResourceLifetime& a, ResourceLifetime& b)
				  { return a.total_size_bytes > b.total_size_bytes; });

		usize resource_lifetime_count = sorted_resource_lifetimes.size;

		struct Bucket
		{
			usize start_pass_index;
			usize end_pass_index;

			Resource* resource;
			D3D12_RESOURCE_DESC1 desc;
			AccessFlags all_accesses;

			usize start_region_index;
			usize region_count;

			usize size_bytes;
		};

		struct Region
		{
			usize start_pass_index;
			usize end_pass_index;

			Resource* resource;
			D3D12_RESOURCE_DESC1 desc;
			AccessFlags all_accesses;

			usize offset_bytes;
			usize size_bytes;
		};

		struct UnaliasableOffset
		{
			enum class Type
			{
				Start,
				End,
			};
			usize offset_bytes;
			Type type;
		};

		Slice<Bucket> const buckets = graph.allocator.alloc<Bucket>(graph.resource_count);
		Slice<Region> const regions = graph.allocator.alloc<Region>(graph.resource_count);
		Slice<UnaliasableOffset> const offsets_scratch = graph.allocator.alloc<UnaliasableOffset>(graph.resource_count);
		usize bucket_count = 0;

		static constexpr auto lifetime_intersects = [](usize a_start, usize a_end, usize b_start, usize b_end) -> bool
		{
			return a_end >= b_start && b_end >= a_start;
		};

		{
			usize region_offset = 0;
			for (usize bucket_lifetime_index = 0; bucket_lifetime_index < resource_lifetime_count; ++bucket_lifetime_index)
			{
				ResourceLifetime const& first_lifetime = sorted_resource_lifetimes[bucket_lifetime_index];
				buckets[bucket_count++] = Bucket{
					.start_pass_index = first_lifetime.start_pass_index,
					.end_pass_index = first_lifetime.end_pass_index,
					.resource = first_lifetime.resource,
					.desc = first_lifetime.desc,
					.all_accesses = first_lifetime.resource->all_accesses,
					.start_region_index = region_offset,
					.region_count = 0,
					.size_bytes = first_lifetime.total_size_bytes,
				};

				Bucket& bucket = buckets[bucket_count - 1];

				for (usize lifetime_index = bucket_lifetime_index + 1; lifetime_index < resource_lifetime_count; ++lifetime_index)
				{
					ResourceLifetime const& lifetime = sorted_resource_lifetimes[lifetime_index];
					bool const intersects_bucket = lifetime_intersects(lifetime.start_pass_index, lifetime.end_pass_index, bucket.start_pass_index, bucket.end_pass_index);
					if (intersects_bucket)
					{
						continue;
					}

					usize unaliasable_offset_count = 0;
					offsets_scratch[unaliasable_offset_count++] = {0, UnaliasableOffset::Type::End};
					offsets_scratch[unaliasable_offset_count++] = {bucket.size_bytes, UnaliasableOffset::Type::Start};

					for (usize region_index = bucket.start_region_index; region_index < bucket.start_region_index + bucket.region_count; ++region_index)
					{
						Region const& region = regions[region_index];
						// We can alias the region so we don't need to mark it as unaliasable
						if (!lifetime_intersects(lifetime.start_pass_index, lifetime.end_pass_index, region.start_pass_index, region.end_pass_index))
						{
							continue;
						}

						offsets_scratch[unaliasable_offset_count++] = {region.offset_bytes, UnaliasableOffset::Type::Start};
						offsets_scratch[unaliasable_offset_count++] = {region.offset_bytes + region.size_bytes, UnaliasableOffset::Type::End};
					}

					std::sort(offsets_scratch.ptr, offsets_scratch.ptr + unaliasable_offset_count, [](UnaliasableOffset const& a, UnaliasableOffset const& b)
							  { return b.offset_bytes > a.offset_bytes; });

					usize overlap_counter = 0;
					usize smallest_region_size_bytes = bucket.size_bytes + 1;
					usize smallest_region_offset_bytes = 0;
					bool found_region = false;
					for (usize i = 0; i < unaliasable_offset_count - 1; ++i)
					{
						UnaliasableOffset const& offset = offsets_scratch[i];
						UnaliasableOffset const& next_offset = offsets_scratch[i + 1];
						if (offset.type == UnaliasableOffset::Type::End && next_offset.type == UnaliasableOffset::Type::Start && overlap_counter == 0)
						{
							const usize region_size = next_offset.offset_bytes - offset.offset_bytes;
							if (lifetime.total_size_bytes <= region_size && region_size < smallest_region_size_bytes)
							{
								found_region = true;
								smallest_region_size_bytes = region_size;
								smallest_region_offset_bytes = offset.offset_bytes;
							}
						}

						if (offset.type == UnaliasableOffset::Type::Start)
						{
							overlap_counter++;
						}
						else if (offset.type == UnaliasableOffset::Type::End)
						{
							overlap_counter--;
						}
					}

					if (found_region)
					{
						usize const region_index = bucket.start_region_index + bucket.region_count++;
						regions[region_index] = {
							.start_pass_index = lifetime.start_pass_index,
							.end_pass_index = lifetime.end_pass_index,
							.resource = lifetime.resource,
							.desc = lifetime.desc,
							.all_accesses = lifetime.resource->all_accesses,
							.offset_bytes = smallest_region_offset_bytes,
							.size_bytes = lifetime.total_size_bytes,
						};

						PAW_LOG_INFO("Wrote region: %llu", region_index);

						PAW_ASSERT(smallest_region_offset_bytes % lifetime.alignment_bytes == 0);

						sorted_resource_lifetimes[lifetime_index] = sorted_resource_lifetimes[resource_lifetime_count - 1];
						resource_lifetime_count--;
						std::sort(sorted_resource_lifetimes.ptr, sorted_resource_lifetimes.ptr + resource_lifetime_count, [](ResourceLifetime& a, ResourceLifetime& b)
								  { return a.total_size_bytes > b.total_size_bytes; });
					}
				}
				region_offset += bucket.region_count;
			}
		}

		usize total_frame_heap_size_bytes = 0;
		u64 largest_image_size = 0;
		for (usize i = 0; i < bucket_count; ++i)
		{
			total_frame_heap_size_bytes += buckets[i].size_bytes;
			largest_image_size = math_maxU64(largest_image_size, buckets[i].size_bytes);
		}

		usize const total_heap_size_bytes = total_frame_heap_size_bytes * g_frames_in_flight;

		D3D12_HEAP_DESC const heap_desc{
			.SizeInBytes = total_heap_size_bytes,
			.Properties = {
				.Type = D3D12_HEAP_TYPE_DEFAULT,
				.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
				.CreationNodeMask = 1,
				.VisibleNodeMask = 1,
			},
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Flags = D3D12_HEAP_FLAG_NONE,
		};

		ID3D12Heap* heap = nullptr;
		HRESULT result = device->CreateHeap(&heap_desc, IID_PPV_ARGS(&heap));
		PAW_ASSERT(SUCCEEDED(result));

		struct FinalResource
		{
			ID3D12Resource1* resources[g_frames_in_flight];
			DescriptorIndex rtv_indices[g_frames_in_flight];
			DescriptorIndex srv_indices[g_frames_in_flight];
			DescriptorIndex dsv_indices[g_frames_in_flight];
			u64 total_size_bytes;
			u64 offset_bytes;
			D3D12_CLEAR_VALUE clear_value;
			bool needs_clear; // if not clear, discard
		};

		u32 total_clear_color_count = 0;
		u32 total_clear_depth_count = 0;
		u32 total_discard_count = 0;

		usize bucket_offset_bytes = 0;
		Slice<FinalResource> const final_resources = graph.allocator.alloc<FinalResource>(graph.resource_count);
		for (usize bucket_index = 0; bucket_index < bucket_count; ++bucket_index)
		{
			Bucket const& bucket = buckets[bucket_index];

			{
				Resource const* const resource = bucket.resource;
				FinalResource& final_resource = final_resources[bucket.resource->index];

				D3D12_CLEAR_VALUE clear_value{
					.Format = bucket.desc.Format,
				};

				memCopy(&resource->clear_value, &clear_value.Color, sizeof(resource->clear_value));

				bool const needs_clear_value = resource->inital_state == InitialState::Clear && ((bucket.desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) > 0) || ((bucket.desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) > 0);

				final_resource.needs_clear = needs_clear_value;
				final_resource.clear_value = clear_value;

				for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
				{
					result = device->CreatePlacedResource2(heap, bucket_offset_bytes + total_frame_heap_size_bytes * frame_index, &bucket.desc, g_access_to_layout_map[u32(resource->last_access)], needs_clear_value ? &clear_value : nullptr, 0, nullptr, IID_PPV_ARGS(&final_resource.resources[frame_index]));
					set_debug_name(final_resource.resources[frame_index], bucket.resource->name);
				}

				if (bucket.all_accesses.contains(Access::RenderTarget))
				{

					D3D12_RENDER_TARGET_VIEW_DESC const desc{
						.Format = bucket.desc.Format,
						.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
					};
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.rtv_indices[frame_index] = rtv_descriptor_pool.alloc();
						device->CreateRenderTargetView(final_resource.resources[frame_index], &desc, rtv_descriptor_pool.get_cpu(final_resource.rtv_indices[frame_index]));
					}

					if (needs_clear_value)
					{
						total_clear_color_count++;
					}
				}
				else
				{
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.rtv_indices[frame_index] = g_null_descriptor_index;
					}
				}

				if (bucket.all_accesses.contains(Access::VertexShader) || bucket.all_accesses.contains(Access::PixelShader))
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC const desc{
						.Format = g_srv_format_map[u32(resource->format)],
						.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
						.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
						.Texture2D = {
							.MostDetailedMip = 0,
							.MipLevels = bucket.desc.MipLevels,
						},
					};
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.srv_indices[frame_index] = srv_pool.alloc();
						device->CreateShaderResourceView(final_resource.resources[frame_index], &desc, srv_pool.get_cpu(final_resource.srv_indices[frame_index]));
					}
				}
				else
				{
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.srv_indices[frame_index] = g_null_descriptor_index;
					}
				}

				if (bucket.all_accesses.contains(Access::Depth))
				{
					D3D12_DEPTH_STENCIL_VIEW_DESC const desc{
						.Format = bucket.desc.Format,
						.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
						.Flags = D3D12_DSV_FLAG_NONE, // #TODO: re-evaluate if this can be narrowed down
					};
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.dsv_indices[frame_index] = dsv_descriptor_pool.alloc();
						device->CreateDepthStencilView(final_resource.resources[frame_index], &desc, dsv_descriptor_pool.get_cpu(final_resource.dsv_indices[frame_index]));
					}
					if (needs_clear_value)
					{
						total_clear_depth_count++;
					}
				}
				else
				{
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.dsv_indices[frame_index] = g_null_descriptor_index;
					}
				}

				if (!needs_clear_value)
				{
					total_discard_count++;
				}

				final_resource.total_size_bytes = bucket.size_bytes;
				final_resource.offset_bytes = bucket_offset_bytes;
			}

			for (usize i = 0; i < bucket.region_count; ++i)
			{
				usize const region_index = bucket.start_region_index + i;
				Region const& region = regions[region_index];
				Resource const* const resource = region.resource;

				FinalResource& final_resource = final_resources[resource->index];

				D3D12_CLEAR_VALUE clear_value{
					.Format = region.desc.Format,
				};
				memCopy(&resource->clear_value, &clear_value.Color, sizeof(resource->clear_value));

				bool const needs_clear_value = resource->inital_state == InitialState::Clear && ((region.desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) > 0) || ((region.desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) > 0);

				final_resource.needs_clear = needs_clear_value;
				final_resource.clear_value = clear_value;

				u64 const region_offset = bucket_offset_bytes + region.offset_bytes;

				for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
				{
					// Initialize layout to last access to emulate having a previous frame end before us
					result = device->CreatePlacedResource2(heap, region_offset + total_frame_heap_size_bytes * frame_index, &region.desc, g_access_to_layout_map[u32(resource->last_access)], needs_clear_value ? &clear_value : nullptr, 0, nullptr, IID_PPV_ARGS(&final_resource.resources[frame_index]));
					set_debug_name(final_resource.resources[frame_index], region.resource->name);
				}

				if (region.all_accesses.contains(Access::RenderTarget))
				{
					D3D12_RENDER_TARGET_VIEW_DESC const desc{
						.Format = region.desc.Format,
						.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
					};
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.rtv_indices[frame_index] = rtv_descriptor_pool.alloc();
						device->CreateRenderTargetView(final_resource.resources[frame_index], &desc, rtv_descriptor_pool.get_cpu(final_resource.rtv_indices[frame_index]));
					}

					if (needs_clear_value)
					{
						total_clear_color_count++;
					}
				}
				else
				{
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.rtv_indices[frame_index] = g_null_descriptor_index;
					}
				}

				if (region.all_accesses.contains(Access::VertexShader) || region.all_accesses.contains(Access::PixelShader))
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC const desc{
						.Format = g_srv_format_map[u32(resource->format)],
						.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
						.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
						.Texture2D = {
							.MostDetailedMip = 0,
							.MipLevels = region.desc.MipLevels,
						},
					};

					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.srv_indices[frame_index] = srv_pool.alloc();
						device->CreateShaderResourceView(final_resource.resources[frame_index], &desc, srv_pool.get_cpu(final_resource.srv_indices[frame_index]));
					}
				}
				else
				{
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.srv_indices[frame_index] = g_null_descriptor_index;
					}
				}

				if (region.all_accesses.contains(Access::Depth))
				{
					D3D12_DEPTH_STENCIL_VIEW_DESC const desc{
						.Format = region.desc.Format,
						.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
						.Flags = D3D12_DSV_FLAG_NONE, // #TODO: re-evaluate if this can be narrowed down
					};
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.dsv_indices[frame_index] = dsv_descriptor_pool.alloc();
						device->CreateDepthStencilView(final_resource.resources[frame_index], &desc, dsv_descriptor_pool.get_cpu(final_resource.dsv_indices[frame_index]));
					}

					if (needs_clear_value)
					{
						total_clear_depth_count++;
					}
				}
				else
				{
					for (usize frame_index = 0; frame_index < g_frames_in_flight; frame_index++)
					{
						final_resource.dsv_indices[frame_index] = g_null_descriptor_index;
					}
				}

				if (!needs_clear_value)
				{
					total_discard_count++;
				}

				final_resource.total_size_bytes = region.size_bytes;
				final_resource.offset_bytes = region_offset;
			}
			bucket_offset_bytes += bucket.size_bytes;
		}

		u32 total_input_count = 0;
		u32 total_output_count = 0;
		u32 total_color_target_count = 0;
		u32 total_depth_target_count = 0;

		for (Pass const* const pass : topological_order)
		{
			for (ResourceInstanceRef* read_ref = pass->first_read_ref; read_ref; read_ref = read_ref->next)
			{
				total_input_count++;
			}

			for (ResourceInstanceRef* write_ref = pass->first_write_ref; write_ref; write_ref = write_ref->next)
			{
				if (write_ref->access == Access::RenderTarget)
				{
					total_color_target_count++;
				}

				if (write_ref->access == Access::Depth)
				{
					total_depth_target_count++;
				}

				total_output_count++;
			}
		}

		// ArenaAllocator runtime_allocator{};
		// runtime_allocator.init(&page_pool, "Render Graph Runtime"_str);

		// ArenaAllocator debug_allocator{};
		// debug_allocator.init(&debug_page_pool, "Render Graph Runtime Debug Data"_str);

		self.runtime_graph.resources = persistent_allocator.alloc<RuntimeGraphResource_t>(graph.resource_count);
		self.runtime_graph.passes = persistent_allocator.alloc<RuntimeGraphPass_t>(graph.pass_count);
		self.runtime_graph.inputs = persistent_allocator.alloc<RuntimeGraphPassInput_t>(total_input_count);
		self.runtime_graph.outputs = persistent_allocator.alloc<RuntimeGraphPassOutput_t>(total_output_count);
		self.runtime_graph.viewports = persistent_allocator.alloc<D3D12_VIEWPORT>(total_output_count);
		self.runtime_graph.scissors = persistent_allocator.alloc<D3D12_RECT>(total_output_count);

		// Per-frame data
		self.runtime_graph.barrier_groups = persistent_allocator.alloc_2d<D3D12_BARRIER_GROUP>(g_frames_in_flight, graph.pass_count * 3);
		self.runtime_graph.texture_barriers = persistent_allocator.alloc_2d<D3D12_TEXTURE_BARRIER>(g_frames_in_flight, total_input_count + total_output_count);
		self.runtime_graph.color_target_handles = persistent_allocator.alloc_2d<D3D12_CPU_DESCRIPTOR_HANDLE>(g_frames_in_flight, total_color_target_count);
		self.runtime_graph.depth_target_handles = persistent_allocator.alloc_2d<D3D12_CPU_DESCRIPTOR_HANDLE>(g_frames_in_flight, total_depth_target_count);
		self.runtime_graph.clear_color_commands = persistent_allocator.alloc_2d<RuntimeGraphClearColorCommand_t>(g_frames_in_flight, total_clear_color_count);
		self.runtime_graph.clear_depth_commands = persistent_allocator.alloc_2d<RuntimeGraphClearDepthCommand_t>(g_frames_in_flight, total_clear_depth_count);
		self.runtime_graph.discard_commands = persistent_allocator.alloc_2d<ID3D12Resource1*>(g_frames_in_flight, total_discard_count);

		// Debug data
		self.runtime_graph.passes_debug_data = debug_persistent_allocator.alloc<RuntimeGraphPassDebugData_t>(graph.pass_count);
		self.runtime_graph.resources_debug_data = debug_persistent_allocator.alloc<RuntimeGraphResourceDebugData_t>(graph.resource_count);

		self.runtime_graph.heap = heap;
		self.runtime_graph.total_heap_size_bytes = total_heap_size_bytes;
		self.runtime_graph.total_frame_heap_size_bytes = total_frame_heap_size_bytes;

		struct ResourceTrackingState_t
		{
			D3D12_BARRIER_SYNC last_sync;
			D3D12_BARRIER_ACCESS last_access;
			D3D12_BARRIER_LAYOUT last_layout;

			static ResourceTrackingState_t create(AccessFlags flags)
			{
				ResourceTrackingState_t result{};
				result.last_sync = D3D12_BARRIER_SYNC_NONE;
				result.last_access = D3D12_BARRIER_ACCESS_COMMON;
				result.last_layout = D3D12_BARRIER_LAYOUT_UNDEFINED;
				for (Access access = Access::RenderTarget; access < Access::Count; access = Access(u32(access) + 1))
				{
					u32 const index = u32(access);
					if (flags.contains(access))
					{
						result.last_sync = g_access_to_sync_map[index];
						result.last_access |= g_access_to_access_map[index];
						result.last_layout = g_access_to_layout_map[index];
					}
				}

				return result;
			}

			static ResourceTrackingState_t create(Access access)
			{
				u32 const index = u32(access);
				return ResourceTrackingState_t{
					.last_sync = g_access_to_sync_map[index],
					.last_access = g_access_to_access_map[index],
					.last_layout = g_access_to_layout_map[index],
				};
			}
		};

		PAW_ASSERT_IS_POD(ResourceTrackingState_t);

		Slice<ResourceTrackingState_t> const trackers = graph.allocator.alloc<ResourceTrackingState_t>(graph.resource_count);

		for (usize i = 0; i < graph.resource_count; i++)
		{
			ResourceTrackingState_t& tracker = trackers[i];
			Resource const* const resource = resource_lifetimes[i].resource;
			FinalResource const& final_resource = final_resources[i];
			// Update to last access so that we emulate a needing to transition from a previous frame
			tracker = ResourceTrackingState_t::create(resource->last_access);

			RuntimeGraphResourceDebugData_t& debug_data = self.runtime_graph.resources_debug_data[i];
			debug_data.name = resource->name;
			debug_data.size_bytes = final_resource.total_size_bytes;
			debug_data.offset_bytes = final_resource.offset_bytes;
			debug_data.start_pass_index = u32(resource_lifetimes[i].start_pass_index);
			debug_data.end_pass_index = u32(resource_lifetimes[i].end_pass_index);
		}

		{
			u32 pass_index = 0;
			// u32 resource_index = 0;
			u32 input_index = 0;
			u32 output_index = 0;
			u32 texture_barrier_index = 0;
			u32 barrier_group_index = 0;
			u32 color_target_index = 0;
			u32 depth_target_index = 0;
			u32 viewport_index = 0;
			u32 scissor_index = 0;
			u32 clear_color_index = 0;
			u32 clear_depth_index = 0;
			u32 discard_index = 0;

			for (Pass const* const pass : topological_order)
			{
				u32 const texture_barrier_start_index = texture_barrier_index;
				u32 const output_start_index = output_index;
				u32 const color_target_start_index = color_target_index;
				u32 depth_target_start_index = g_null_depth_target_index;
				u32 const clear_color_start_index = clear_color_index;
				u32 const clear_depth_start_index = clear_depth_index;
				u32 const discard_start_index = discard_index;

				for (ResourceInstanceRef* write_ref = pass->first_write_ref; write_ref; write_ref = write_ref->next)
				{
					static byte buffer[512];
					Resource* resource = write_ref->instance->resource;
					FinalResource& final_resource = final_resources[resource->index];
					UINT name_length = PAW_ARRAY_COUNT(buffer);
					final_resource.resources[0]->GetPrivateData(WKPDID_D3DDebugObjectName, &name_length, buffer);
					PAW_LOG_INFO("Write %p{str} == %.*s", &resource->name, name_length, buffer);
					RuntimeGraphPassOutput_t& output = self.runtime_graph.outputs[output_index++];
					output.resource_index = u32(resource->index);

					ResourceTrackingState_t const new_tracker = ResourceTrackingState_t::create(write_ref->access);

					ResourceTrackingState_t const& current_tracker = trackers[resource->index];

					if (current_tracker.last_sync != new_tracker.last_sync || current_tracker.last_access != new_tracker.last_access || current_tracker.last_layout != new_tracker.last_layout)
					{
						u32 const write_index = texture_barrier_index++;
						for (usize i = 0; i < g_frames_in_flight; i++)
						{
							D3D12_TEXTURE_BARRIER& texture_barrier = self.runtime_graph.texture_barriers[i][write_index];
							texture_barrier.SyncBefore = current_tracker.last_sync;
							texture_barrier.SyncAfter = new_tracker.last_sync;
							texture_barrier.AccessBefore = current_tracker.last_access;
							texture_barrier.AccessAfter = new_tracker.last_access;
							texture_barrier.LayoutBefore = current_tracker.last_layout;
							texture_barrier.LayoutAfter = new_tracker.last_layout;
							texture_barrier.pResource = final_resource.resources[i];
							// #TODO: Properly handle subresources
							texture_barrier.Subresources = {
								.IndexOrFirstMipLevel = 0,
								.NumMipLevels = 1,
								.FirstArraySlice = 0,
								.NumArraySlices = 1,
								.FirstPlane = 0,
								.NumPlanes = 1,
							};
							texture_barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
						}
					}
					trackers[resource->index] = new_tracker;

					bool const is_first_reference = write_ref->instance->ref_index == 0;

					if (write_ref->access == Access::RenderTarget)
					{
						u32 const color_target_write_index = color_target_index++;
						bool const needs_clear_command = is_first_reference && final_resource.needs_clear;
						u32 const clear_command_write_index = needs_clear_command ? clear_color_index++ : 0;
						for (usize i = 0; i < g_frames_in_flight; i++)
						{
							D3D12_CPU_DESCRIPTOR_HANDLE const cpu_handle = rtv_descriptor_pool.get_cpu(final_resource.rtv_indices[i]);

							self.runtime_graph.color_target_handles[i][color_target_write_index] = cpu_handle;

							if (needs_clear_command)
							{
								RuntimeGraphClearColorCommand_t command{
									.rtv_handle = cpu_handle,
								};

								memCopy(&final_resource.clear_value.Color, &command.color, sizeof(command.color));

								self.runtime_graph.clear_color_commands[i][clear_command_write_index] = command;
							}
						}
					}
					else if (write_ref->access == Access::Depth)
					{
						PAW_ASSERT(depth_target_start_index == g_null_depth_target_index);
						depth_target_start_index = depth_target_index++;
						bool const needs_clear_command = is_first_reference && final_resource.needs_clear;
						u32 const clear_depth_write_index = needs_clear_command ? clear_depth_index++ : 0;
						for (usize i = 0; i < g_frames_in_flight; i++)
						{
							D3D12_CPU_DESCRIPTOR_HANDLE const cpu_handle = dsv_descriptor_pool.get_cpu(final_resource.dsv_indices[i]);

							self.runtime_graph.depth_target_handles[i][depth_target_start_index] = cpu_handle;

							if (needs_clear_command)
							{
								self.runtime_graph.clear_depth_commands[i][clear_depth_write_index] = RuntimeGraphClearDepthCommand_t{
									.dsv_handle = cpu_handle,
									.flags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, // #TODO: Handle this properly
									.depth = final_resource.clear_value.DepthStencil.Depth,
									.stencil = final_resource.clear_value.DepthStencil.Stencil,
								};
							}
						}
					}

					if (is_first_reference && !final_resource.needs_clear)
					{
						u32 const write_index = discard_index++;
						for (usize i = 0; i < g_frames_in_flight; i++)
						{
							self.runtime_graph.discard_commands[i][write_index] = final_resource.resources[i];
						}
					}

					// #TODO: Specify min / max depth values
					self.runtime_graph.viewports[viewport_index++] = D3D12_VIEWPORT{
						.TopLeftX = 0.0f,
						.TopLeftY = 0.0f,
						.Width = f32(resource->width),
						.Height = f32(resource->height),
						.MinDepth = 0.0f,
						.MaxDepth = 1.0f,
					};
					self.runtime_graph.scissors[scissor_index++] = D3D12_RECT{
						.left = 0,
						.top = 0,
						.right = LONG(resource->width),
						.bottom = LONG(resource->height),
					};
				}

				u32 const input_start_index = input_index;

				for (ResourceInstanceRef* read_ref = pass->first_read_ref; read_ref; read_ref = read_ref->next)
				{
					static byte buffer[512];
					Resource* resource = read_ref->instance->resource;
					FinalResource& final_resource = final_resources[resource->index];
					UINT name_length = PAW_ARRAY_COUNT(buffer);
					final_resource.resources[0]->GetPrivateData(WKPDID_D3DDebugObjectName, &name_length, buffer);
					PAW_LOG_INFO("Read %p{str} == %.*s", &resource->name, name_length, buffer);
					RuntimeGraphPassInput_t& input = self.runtime_graph.inputs[input_index++];
					input.resource_index = u32(resource->index);

					ResourceTrackingState_t const new_tracker = ResourceTrackingState_t::create(read_ref->access);

					ResourceTrackingState_t const& current_tracker = trackers[resource->index];

					if (current_tracker.last_sync != new_tracker.last_sync || current_tracker.last_access != new_tracker.last_access || current_tracker.last_layout != new_tracker.last_layout)
					{
						u32 const texture_barrier_write_index = texture_barrier_index++;
						for (usize i = 0; i < g_frames_in_flight; i++)
						{
							D3D12_TEXTURE_BARRIER& texture_barrier = self.runtime_graph.texture_barriers[i][texture_barrier_write_index];
							texture_barrier.SyncBefore = current_tracker.last_sync;
							texture_barrier.SyncAfter = new_tracker.last_sync;
							texture_barrier.AccessBefore = current_tracker.last_access;
							texture_barrier.AccessAfter = new_tracker.last_access;
							texture_barrier.LayoutBefore = current_tracker.last_layout;
							texture_barrier.LayoutAfter = new_tracker.last_layout;
							texture_barrier.pResource = final_resource.resources[i];
							// #TODO: Properly handle subresources
							texture_barrier.Subresources = {
								.IndexOrFirstMipLevel = 0,
								.NumMipLevels = 1,
								.FirstArraySlice = 0,
								.NumArraySlices = 1,
								.FirstPlane = 0,
								.NumPlanes = 1,
							};
							texture_barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
						}
					}
					trackers[resource->index] = new_tracker;
				}

				u32 const input_count = input_index - input_start_index;
				u32 const output_count = output_index - output_start_index;
				u32 const texture_barrier_count = texture_barrier_index - texture_barrier_start_index;
				u32 const barrier_groups_start_index = barrier_group_index;
				u32 const color_target_count = color_target_index - color_target_start_index;

				u32 barrier_group_count = 0;

				if (texture_barrier_count > 0)
				{
					u32 const write_index = barrier_group_index++;
					for (usize i = 0; i < g_frames_in_flight; i++)
					{
						D3D12_BARRIER_GROUP& group = self.runtime_graph.barrier_groups[i][write_index];
						group.Type = D3D12_BARRIER_TYPE_TEXTURE;
						group.NumBarriers = texture_barrier_count;
						group.pTextureBarriers = &self.runtime_graph.texture_barriers[i][texture_barrier_start_index];
					}
					barrier_group_count++;
				}

				u32 const used_pass_index = pass_index++;

				RuntimeGraphPass_t& runtime_pass = self.runtime_graph.passes[used_pass_index];
				runtime_pass.func = pass->func;
				runtime_pass.inputs_start_index = input_start_index;
				runtime_pass.input_count = input_count;
				runtime_pass.outputs_start_index = output_start_index;
				runtime_pass.output_count = output_count;
				runtime_pass.barrier_groups_start_index = barrier_groups_start_index;
				runtime_pass.barrier_group_count = barrier_group_count;
				runtime_pass.color_target_start_index = color_target_start_index;
				runtime_pass.color_target_count = color_target_count;
				runtime_pass.depth_target_index = depth_target_start_index;
				runtime_pass.clear_color_start_index = clear_color_start_index;
				runtime_pass.clear_color_count = clear_color_index - clear_color_start_index;
				runtime_pass.clear_depth_start_index = clear_depth_start_index;
				runtime_pass.clear_depth_count = clear_depth_index - clear_depth_start_index;
				runtime_pass.discard_start_index = discard_start_index;
				runtime_pass.discard_count = discard_index - discard_start_index;

				RuntimeGraphPassDebugData_t& runtime_pass_debug_data = self.runtime_graph.passes_debug_data[used_pass_index];
				runtime_pass_debug_data.name = pass->name;
			}
		}

		PAW_LOG_INFO("============================================================================================");
		PAW_LOG_INFO("============================================================================================");
		PAW_LOG_INFO("============================================================================================");

		for (usize pass_index = 0; pass_index < self.runtime_graph.passes.size; pass_index++)
		{
			RuntimeGraphPass_t const& pass = self.runtime_graph.passes[pass_index];
			RuntimeGraphPassDebugData_t const& pass_debug_data = self.runtime_graph.passes_debug_data[pass_index];
			PAW_LOG_INFO("Pass: %p{str}", &pass_debug_data.name);
			Slice<D3D12_BARRIER_GROUP const> const barrier_groups = self.runtime_graph.barrier_groups[0].const_sub_slice(pass.barrier_groups_start_index, pass.barrier_group_count);
			for (D3D12_BARRIER_GROUP const barrier_group : barrier_groups)
			{
				for (u32 barrier_index = 0; barrier_index < barrier_group.NumBarriers; barrier_index++)
				{
					D3D12_TEXTURE_BARRIER const& barrier = barrier_group.pTextureBarriers[barrier_index];
					static byte buffer[512];
					UINT name_length = PAW_ARRAY_COUNT(buffer);
					barrier.pResource->GetPrivateData(WKPDID_D3DDebugObjectName, &name_length, buffer);

					PAW_LOG_INFO("Barrier for %.*s:\n\tSync: %s to %s\n\tAccess: %s to %s\n\tLayout: %s to %s", name_length, buffer, get_d3d12_sync_name(barrier.SyncBefore), get_d3d12_sync_name(barrier.SyncAfter), get_d3d12_access_name(barrier.AccessBefore), get_d3d12_access_name(barrier.AccessAfter), get_d3d12_layour_name(barrier.LayoutBefore), get_d3d12_layour_name(barrier.LayoutAfter));
				}
			}

			PAW_LOG_INFO("============================================================================================");
		}

		return self;
	}

	void deinit(RenderGraph& /*self*/)
	{
	}

	void tick(RenderGraph& self)
	{
		PAW_PROFILER_FUNC();

		RuntimeRenderGraph const& graph = self.runtime_graph;

		{
			PAW_PROFILER_SCOPE("ImGui");
			if (ImGui::Begin("Render Graph"))
			{
				// ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 const available = ImGui::GetContentRegionAvail();
				f32 const pass_count = f32(graph.passes.size);
				f32 const padding = 5.0f;
				f32 const total_padding = (pass_count - 1) * padding;
				f32 const width_per_pass = (available.x - total_padding) / pass_count;
				f32 const pass_height = 50.0f;

				ImGui::Text("Lifetimes");

				// ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 30.0f);
				const ImVec2 start_cursor = ImGui::GetCursorPos();
				const ImVec2 pass_size(width_per_pass, pass_height);
				f32 x_offset = start_cursor.x;
				f32 y_offset = start_cursor.y;

				for (RuntimeGraphPassDebugData_t const& pass : graph.passes_debug_data)
				{
					ImGui::SetCursorPos(ImVec2(x_offset, y_offset));
					ImGui::Button(pass.name.ptr, pass_size);
					// draw_list->AddRectFilled(min, max, IM_COL32(200, 200, 200, 255), 5.0f);
					x_offset += width_per_pass + padding;
				}

				ImGui::SetCursorPos(ImVec2(start_cursor.x, start_cursor.y * 2.0f + pass_height));

				x_offset = start_cursor.x;
				y_offset += pass_height + padding;

				const f32 lifetime_height = pass_height * 0.5f;

				f32 total_resource_size_bytes = 0.0f;

				for (RuntimeGraphResourceDebugData_t const& resource : graph.resources_debug_data)
				{
					const f32 span_pass_count = f32((resource.end_pass_index + 1) - resource.start_pass_index);
					const f32 start_x = x_offset + ((width_per_pass + padding) * resource.start_pass_index);
					const f32 width = (span_pass_count * width_per_pass) + ((span_pass_count - 1.0f) * padding);
					ImGui::SetCursorPos(ImVec2(start_x, y_offset));
					ImGui::Button(resource.name.ptr, ImVec2(width, lifetime_height));
					y_offset += lifetime_height + padding;
					total_resource_size_bytes += f32(resource.size_bytes);
				}

				ImGui::SetCursorPos(ImVec2(x_offset, y_offset));
				ImGui::Separator();

				ImGui::Text("Per Frame Heap Layout");

				x_offset = ImGui::GetCursorPosX();
				y_offset = ImGui::GetCursorPosY();
				// const f32 heap_start_y = y_offset;

				// f32 max_y_offset = 0.0f;

				for (RuntimeGraphResourceDebugData_t const& resource : graph.resources_debug_data)
				{
					f32 const pixels_per_heap_byte = available.x / f32(graph.total_frame_heap_size_bytes);
					f32 const heap_start_x = x_offset;
					f32 const width = pixels_per_heap_byte * resource.size_bytes;
					f32 const offset = pixels_per_heap_byte * resource.offset_bytes;
					ImGui::SetCursorPos(ImVec2(heap_start_x + offset, y_offset));
					ImGui::Button(resource.name.ptr, ImVec2(width, lifetime_height));
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("%.*s", int(resource.name.size_bytes), resource.name.ptr);
					}

					y_offset += lifetime_height + padding;
				}

				ImGui::Separator();
				ImGui::Text("Total Heap Size: %gMB", f64(graph.total_heap_size_bytes) / 1024.0 / 1024.0);
				ImGui::Text("Per Frame Heap Size: %gMB", f64(graph.total_frame_heap_size_bytes) / 1024.0 / 1024.0);
				ImGui::Separator();
			}
			ImGui::End();
		}
	}

	void render(render_graph::RenderGraph& self, ID3D12GraphicsCommandList9* command_list, TracyD3D12Ctx& profiler_ctx, usize local_frame_index, DX12Renderer& temp_renderer);

}

struct ImGuiFrameData
{
	ID3D12Resource* vertex_buffer;
	ID3D12Resource* index_buffer;

	DescriptorIndex vertex_buffer_index;

	s32 vertex_buffer_size;
	s32 index_buffer_size;
};

struct GameViewportFrameData
{
	GpuImageHandle color_rt_msaa;
	GpuImageHandle depth_rt_msaa;
	GpuImageHandle color_rt_resolved;
	bool resized;
};

struct PerFrameData
{
	GpuImageHandle pre_oetf_rt;
};

struct DX12Renderer
{
	ID3D12Device12* device;
	IDXGISwapChain3* swapchain;
	PlatformAutoResetEvent swapchain_event;
	JobWaitable swapchain_event_waitable;
	DXGI_FORMAT swapchain_format;
	DXGI_FORMAT swapchain_read_format;
	ID3D12Resource* backbuffer_resources[g_frames_in_flight];
	D3D12_CPU_DESCRIPTOR_HANDLE backbuffer_handles[g_frames_in_flight];
	bool hdr;
	f32 white_sdr_scale;

	ID3D12CommandQueue* present_command_queue;

	usize global_frame_index;
	u32 local_frame_index;

	u64 present_fence_values[g_frames_in_flight];
	PlatformAutoResetEvent present_fence_event;
	JobWaitable present_fence_event_waitable;
	ID3D12Fence* present_fence;
	u64 fence_value;

	ID3D12CommandQueue* command_queues[QueueType_Count];
	ID3D12CommandAllocator* command_allocators[QueueType_Count][g_frames_in_flight];
	CommandListPool command_list_pools[QueueType_Count];

	ID3D12Fence* queue_fences[QueueType_Count];

	DescriptorPool cbv_srv_uav_descriptor_pool;
	DescriptorPool sampler_descriptor_pool;
	DescriptorPool rtv_descriptor_pool;
	DescriptorPool dsv_descriptor_pool;

	GraphicsPipelineState triangle_pipeline_state;
	GraphicsPipelineState imgui_pipeline_state;
	GraphicsPipelineState aces_to_acescg_pipeline_state;
	GraphicsPipelineState oetf_pipeline_state;
	ComputePipelineState display_map_sdr_pipeline_state;
	ComputePipelineState display_map_hdr_pipeline_state;
	GraphicsPipelineState debug_geom_pipeline_state;
	GraphicsPipelineState mesh_shader_pipeline_state;

	DescriptorIndex imgui_font_index;
	GpuImageHandle imgui_font_image;

	DescriptorIndex imgui_sampler_index;
	ID3D12Resource* imgui_sampler_resource;

	ImGuiFrameData imgui_frame_datas[g_frames_in_flight];

	DXGI_FORMAT game_color_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	u32 game_color_sample_count = 4;

	DescriptorIndex reference_image_sampler_index;
	ID3D12Resource* reference_image;
	DescriptorIndex reference_image_index;
	s32 reference_image_width;
	s32 reference_image_height;

	ID3D12Resource* debug_draw_3d_buffers[g_frames_in_flight];
	DescriptorIndex debug_draw_3d_buffer_slots[g_frames_in_flight];
	usize debug_draw_3d_buffer_sizes[g_frames_in_flight];
	usize debug_draw_3d_command_counts[g_frames_in_flight];

	ImageStore2 image_store;

	PerFrameData per_frame_data[g_frames_in_flight];

	GameViewportFrameData game_viewport_frame_data[g_frames_in_flight];

	ID3D12Heap* game_viewport_msaa_heap;
	ID3D12Heap* game_viewport_normal_heap;

	TracyD3D12Ctx profiler_ctx;

	render_graph::RenderGraph render_graph;
	Mat4 temp_camera_mat;
};

void render_graph::opaque_node(RuntimeRenderGraph const& /*graph*/, RuntimeGraphPass_t const& /*pass*/, ID3D12GraphicsCommandList9* command_list, DX12Renderer& temp_renderer)
{
	// command_list->SetGraphicsRootSignature(self.triangle_pipeline_state.root_signature);
	// command_list->SetPipelineState(self.triangle_pipeline_state.pso);
	// command_list->SetGraphicsRoot32BitConstants(0, 3, &color, 0);
	// command_list->SetGraphicsRoot32BitConstants(0, 1, &one_over_aspect_ratio, 3);
	// command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// command_list->DrawInstanced(3, 1, 0, 0);

	command_list->SetGraphicsRootSignature(temp_renderer.debug_geom_pipeline_state.root_signature);
	command_list->SetPipelineState(temp_renderer.debug_geom_pipeline_state.pso);
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DrawConstants
	{
		Mat4 projection;
		u32 line_buffer_index;
		u32 line_count;
		f32 white_sdr_scale;
	};
	PAW_ERROR_ON_PADDING_END

	const DrawConstants constants{
		.projection = temp_renderer.temp_camera_mat,
		.line_buffer_index = temp_renderer.debug_draw_3d_buffer_slots[temp_renderer.local_frame_index].value,
		.line_count = (u32)temp_renderer.debug_draw_3d_command_counts[temp_renderer.local_frame_index],
		.white_sdr_scale = temp_renderer.white_sdr_scale,
	};
	static_assert(sizeof(DrawConstants) % 4 == 0);
	command_list->SetGraphicsRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	// command_list->DrawInstanced(36 * (u32)temp_renderer.debug_draw_3d_command_counts[temp_renderer.local_frame_index], 1, 0, 0);

	command_list->SetPipelineState(temp_renderer.mesh_shader_pipeline_state.pso);
	command_list->SetGraphicsRootSignature(temp_renderer.mesh_shader_pipeline_state.root_signature);
	command_list->SetGraphicsRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	const u32 dispatch_count = (u32)math_floor_to_s32((f32)temp_renderer.debug_draw_3d_command_counts[temp_renderer.local_frame_index] / 5.0f);
	command_list->DispatchMesh(dispatch_count, 1, 1);
}

void render_graph::render(RenderGraph& self, ID3D12GraphicsCommandList9* command_list, TracyD3D12Ctx& profiler_ctx, usize local_frame_index, DX12Renderer& temp_renderer)
{
	PAW_PROFILER_FUNC();

	RuntimeRenderGraph const& graph = self.runtime_graph;

	ID3D12DescriptorHeap* const descriptor_heaps[]{
		temp_renderer.cbv_srv_uav_descriptor_pool.heap,
		temp_renderer.sampler_descriptor_pool.heap,
	};
	command_list->SetDescriptorHeaps(PAW_ARRAY_COUNT(descriptor_heaps), descriptor_heaps);

	for (usize pass_index = 0; pass_index < graph.passes.size; pass_index++)
	{
		RuntimeGraphPass_t const& pass = graph.passes[pass_index];
		RuntimeGraphPassDebugData_t const& pass_debug_data = graph.passes_debug_data[pass_index];
		PAW_ASSERT(pass_debug_data.name.null_terminated);
		PIXBeginEvent(command_list, 0, pass_debug_data.name.ptr);
		TracyD3D12ZoneTransient(profiler_ctx, tracy_d3d12_zone, command_list, pass_debug_data.name.ptr, true);
		if (pass.barrier_group_count > 0)
		{
			command_list->Barrier(pass.barrier_group_count, &graph.barrier_groups[local_frame_index][pass.barrier_groups_start_index]);
		}
		if (pass.output_count > 0)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE const* const depth_target_handle = pass.depth_target_index != g_null_depth_target_index ? &graph.depth_target_handles[local_frame_index][pass.depth_target_index] : nullptr;
			command_list->OMSetRenderTargets(pass.color_target_count, &graph.color_target_handles[local_frame_index][pass.color_target_start_index], false, depth_target_handle);
			command_list->RSSetViewports(pass.output_count, &graph.viewports[pass.outputs_start_index]);
			command_list->RSSetScissorRects(pass.output_count, &graph.scissors[pass.outputs_start_index]);
		}
		for (u32 i = pass.clear_color_start_index; i < pass.clear_color_start_index + pass.clear_color_count; i++)
		{
			RuntimeGraphClearColorCommand_t const& clear_command = graph.clear_color_commands[local_frame_index][i];
			command_list->ClearRenderTargetView(clear_command.rtv_handle, clear_command.color, 0, nullptr);
		}

		for (u32 i = pass.clear_depth_start_index; i < pass.clear_depth_start_index + pass.clear_depth_count; i++)
		{
			RuntimeGraphClearDepthCommand_t const& clear_command = graph.clear_depth_commands[local_frame_index][i];
			command_list->ClearDepthStencilView(clear_command.dsv_handle, clear_command.flags, clear_command.depth, clear_command.stencil, 0, nullptr);
		}

		for (u32 i = pass.discard_start_index; i < pass.discard_start_index + pass.discard_count; i++)
		{
			command_list->DiscardResource(graph.discard_commands[local_frame_index][i], nullptr);
		}

		if (pass.func)
		{
			pass.func(graph, pass, command_list, temp_renderer);
		}

		PIXEndEvent(command_list);
	}
}

static void d3d12_message_callback(
	D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID, LPCSTR description, void*)
{
	if (category == D3D12_MESSAGE_CATEGORY_STATE_CREATION)
	{
		return;
	}

	switch (severity)
	{
		case D3D12_MESSAGE_SEVERITY_CORRUPTION:
		case D3D12_MESSAGE_SEVERITY_ERROR:
		{
			PAW_LOG_ERROR("D3D12: %s", description);
		}
		break;

		case D3D12_MESSAGE_SEVERITY_WARNING:
		{
			PAW_LOG_WARNING("D3D12: %s", description);
		}
		break;

		case D3D12_MESSAGE_SEVERITY_INFO:
		case D3D12_MESSAGE_SEVERITY_MESSAGE:
		{
			PAW_LOG_INFO("D3D12: %s", description);
		}
		break;

		default:
			PAW_UNREACHABLE;
	}
}

static void wait_on_gpu(DX12Renderer* self)
{
	self->fence_value++;

	HRESULT result = self->present_command_queue->Signal(self->present_fence, self->fence_value);
	PAW_ASSERT(SUCCEEDED(result));
	PAW_ASSERT_UNUSED(result);

	// WAit until the fence has been processed
	result = self->present_fence->SetEventOnCompletion(self->fence_value, HANDLE(self->present_fence_event.handle));
	PAW_ASSERT(SUCCEEDED(result));
	WaitForSingleObjectEx(HANDLE(self->present_fence_event.handle), INFINITE, false);
}

static void draw_triangle_node(DX12Renderer& self, ID3D12GraphicsCommandList9* command_list, Mat4 camera_projection)
{
	TracyD3D12Zone(self.profiler_ctx, command_list, "draw_triangle");

	// command_list->SetGraphicsRootSignature(self.triangle_pipeline_state.root_signature);
	// command_list->SetPipelineState(self.triangle_pipeline_state.pso);
	// command_list->SetGraphicsRoot32BitConstants(0, 3, &color, 0);
	// command_list->SetGraphicsRoot32BitConstants(0, 1, &one_over_aspect_ratio, 3);
	// command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// command_list->DrawInstanced(3, 1, 0, 0);

	ID3D12DescriptorHeap* descriptor_heaps[]{self.cbv_srv_uav_descriptor_pool.heap, self.sampler_descriptor_pool.heap};
	command_list->SetDescriptorHeaps(PAW_ARRAY_COUNT(descriptor_heaps), descriptor_heaps);
	command_list->SetGraphicsRootSignature(self.debug_geom_pipeline_state.root_signature);
	command_list->SetPipelineState(self.debug_geom_pipeline_state.pso);
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DrawConstants
	{
		Mat4 projection;
		u32 line_buffer_index;
		u32 line_count;
		f32 white_sdr_scale;
	};
	PAW_ERROR_ON_PADDING_END

	const DrawConstants constants{
		.projection = camera_projection,
		.line_buffer_index = self.debug_draw_3d_buffer_slots[self.local_frame_index].value,
		.line_count = (u32)self.debug_draw_3d_command_counts[self.local_frame_index],
		.white_sdr_scale = self.white_sdr_scale,
	};
	static_assert(sizeof(DrawConstants) % 4 == 0);
	command_list->SetGraphicsRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	// command_list->DrawInstanced(36 * (u32)self.debug_draw_3d_command_counts[self.local_frame_index], 1, 0, 0);

	command_list->SetPipelineState(self.mesh_shader_pipeline_state.pso);
	command_list->SetGraphicsRootSignature(self.mesh_shader_pipeline_state.root_signature);
	command_list->SetGraphicsRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	const u32 dispatch_count = (u32)math_floor_to_s32((f32)self.debug_draw_3d_command_counts[self.local_frame_index] / 5.0f);
	command_list->DispatchMesh(dispatch_count, 1, 1);
}

static void draw_imgui(DX12Renderer& self, ID3D12GraphicsCommandList9* command_list)
{
	PIX_EVENT_SCOPE(command_list, "draw_imgui");
	HRESULT result = S_OK;
	PAW_ASSERT_UNUSED(result);
	TracyD3D12Zone(self.profiler_ctx, command_list, "draw_imgui");

	/*DX12Renderer* self = (DX12Renderer*)context->pUserRecordContext;

	if (game_color_texture != nullptr)
	{
		const D3D12_RESOURCE_DESC desc = game_color_texture->GetDesc();

		const D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{
			.Format = desc.Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MostDetailedMip = 0,
				.MipLevels = desc.MipLevels,
			},
		};

		DescriptorIndex& game_color_index = self->virtual_indices[VirtualTextureIndex_GameColor][self->local_frame_index];

		if (game_color_index != g_null_descriptor_index)
		{
			self->cbv_srv_uav_descriptor_pool.free(game_color_index);
		}

		game_color_index = self->cbv_srv_uav_descriptor_pool.alloc();

		self->device->CreateShaderResourceView(game_color_texture, &srv_desc, self->cbv_srv_uav_descriptor_pool.get_cpu(game_color_index));
	}

	if (texture_acescg != nullptr)
	{
		const D3D12_RESOURCE_DESC desc = texture_acescg->GetDesc();

		const D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{
			.Format = desc.Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MostDetailedMip = 0,
				.MipLevels = desc.MipLevels,
			},
		};

		DescriptorIndex& texture_acescg_index = self->virtual_indices[VirtualTextureIndex_AcesAcesCG][self->local_frame_index];

		if (texture_acescg_index != g_null_descriptor_index)
		{
			self->cbv_srv_uav_descriptor_pool.free(texture_acescg_index);
		}

		texture_acescg_index = self->cbv_srv_uav_descriptor_pool.alloc();

		self->device->CreateShaderResourceView(texture_acescg, &srv_desc, self->cbv_srv_uav_descriptor_pool.get_cpu(texture_acescg_index));
	}*/

	ImGui::Render();
	ImDrawData const* draw_data = ImGui::GetDrawData();

	ImGuiFrameData& frame_data = self.imgui_frame_datas[self.local_frame_index];

	if (frame_data.vertex_buffer == nullptr || frame_data.vertex_buffer_size < draw_data->TotalVtxCount)
	{
		if (frame_data.vertex_buffer != nullptr)
		{
			frame_data.vertex_buffer->Release();
			self.cbv_srv_uav_descriptor_pool.free(frame_data.vertex_buffer_index);
		}
		frame_data.vertex_buffer_size = draw_data->TotalVtxCount + 5000;

		const D3D12_RESOURCE_DESC desc{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Width = frame_data.vertex_buffer_size * sizeof(ImDrawVert),
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {.Count = 1},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		result = self.device->CreateCommittedResource(&g_upload_heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&frame_data.vertex_buffer));
		PAW_ASSERT(SUCCEEDED(result));
		frame_data.vertex_buffer_index = self.cbv_srv_uav_descriptor_pool.alloc();

		const D3D12_SHADER_RESOURCE_VIEW_DESC buffer_view_desc{
			.Format = DXGI_FORMAT_R32_TYPELESS,
			.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Buffer = {
				.FirstElement = 0,
				.NumElements = (frame_data.vertex_buffer_size * sizeof(ImDrawVert)) / 4,
				.Flags = D3D12_BUFFER_SRV_FLAG_RAW,
			},
		};

		self.device->CreateShaderResourceView(frame_data.vertex_buffer, &buffer_view_desc, self.cbv_srv_uav_descriptor_pool.get_cpu(frame_data.vertex_buffer_index));
	}

	if (frame_data.index_buffer == nullptr || frame_data.index_buffer_size < draw_data->TotalIdxCount)
	{
		if (frame_data.index_buffer != nullptr)
		{
			frame_data.index_buffer->Release();
		}
		frame_data.index_buffer_size = draw_data->TotalIdxCount + 10000;

		const D3D12_RESOURCE_DESC desc{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Width = frame_data.index_buffer_size * sizeof(ImDrawIdx),
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {.Count = 1},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		result = self.device->CreateCommittedResource(&g_upload_heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&frame_data.index_buffer));
		PAW_ASSERT(SUCCEEDED(result));
	}

	{
		const D3D12_RANGE vertex_buffer_range{0, 0};
		void* vertex_buffer_ptr_raw = nullptr;
		result = frame_data.vertex_buffer->Map(0, &vertex_buffer_range, &vertex_buffer_ptr_raw);
		PAW_ASSERT(SUCCEEDED(result));

		const D3D12_RANGE index_buffer_range{0, 0};
		void* index_buffer_ptr_raw = nullptr;
		result = frame_data.index_buffer->Map(0, &index_buffer_range, &index_buffer_ptr_raw);
		PAW_ASSERT(SUCCEEDED(result));

		ImDrawVert* vertex_buffer_ptr = (ImDrawVert*)vertex_buffer_ptr_raw;
		ImDrawIdx* index_buffer_ptr = (ImDrawIdx*)index_buffer_ptr_raw;
		for (int i = 0; i < draw_data->CmdListsCount; ++i)
		{
			// These writes are implicitly flushed because they are in an upload heap which is write-combined
			ImDrawList const* cmd_list = draw_data->CmdLists[i];
			memCopy(cmd_list->VtxBuffer.Data, vertex_buffer_ptr, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memCopy(cmd_list->IdxBuffer.Data, index_buffer_ptr, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vertex_buffer_ptr += cmd_list->VtxBuffer.Size;
			index_buffer_ptr += cmd_list->IdxBuffer.Size;
		}
		frame_data.vertex_buffer->Unmap(0, &vertex_buffer_range);
		frame_data.index_buffer->Unmap(0, &index_buffer_range);
	}

	ID3D12DescriptorHeap* descriptor_heaps[]{self.cbv_srv_uav_descriptor_pool.heap, self.sampler_descriptor_pool.heap};
	command_list->SetDescriptorHeaps(PAW_ARRAY_COUNT(descriptor_heaps), descriptor_heaps);
	command_list->SetGraphicsRootSignature(self.imgui_pipeline_state.root_signature);
	command_list->SetPipelineState(self.imgui_pipeline_state.pso);
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DrawConstants
	{
		float2 translation;
		float2 scale;
		u32 buffer_index;
		u32 sampler_index;
		u32 texture_index;
		u32 base_vertex_location;
		f32 white_sdr_scale;
	};
	PAW_ERROR_ON_PADDING_END

	DrawConstants draw_constants{
		.translation = {-1.0f, 1.0f},
		.scale = {2.0f / draw_data->DisplaySize.x, -2.0f / draw_data->DisplaySize.y},
		.buffer_index = frame_data.vertex_buffer_index.value,
		.sampler_index = self.imgui_sampler_index.value,
	};

	const D3D12_INDEX_BUFFER_VIEW index_buffer_view{
		.BufferLocation = frame_data.index_buffer->GetGPUVirtualAddress(),
		.SizeInBytes = frame_data.index_buffer_size * sizeof(ImDrawIdx),
		.Format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
	};
	command_list->IASetIndexBuffer(&index_buffer_view);

	int global_vtx_offset = 0;
	int global_idx_offset = 0;
	ImVec2 clip_off = draw_data->DisplayPos;
	for (int cmd_list_index = 0; cmd_list_index < draw_data->CmdListsCount; ++cmd_list_index)
	{
		ImDrawList const* cmd_list = draw_data->CmdLists[cmd_list_index];
		for (int cmd_index = 0; cmd_index < cmd_list->CmdBuffer.Size; ++cmd_index)
		{
			ImDrawCmd const& cmd = cmd_list->CmdBuffer[cmd_index];
			ImVec2 clip_min(cmd.ClipRect.x - clip_off.x, cmd.ClipRect.y - clip_off.y);
			ImVec2 clip_max(cmd.ClipRect.z - clip_off.x, cmd.ClipRect.w - clip_off.y);
			if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
				continue;

			const D3D12_RECT scissor_rect = {(LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y};

			const DescriptorIndex texture_index{u32(u64(cmd.GetTexID()))};

			draw_constants.white_sdr_scale = texture_index == self.imgui_font_index ? self.white_sdr_scale : 1.0f;

			draw_constants.texture_index = texture_index.value;
			draw_constants.base_vertex_location = cmd.VtxOffset + global_vtx_offset;
			command_list->RSSetScissorRects(1, &scissor_rect);
			command_list->SetGraphicsRoot32BitConstants(0, sizeof(DrawConstants) / 4, &draw_constants, 0);
			command_list->DrawIndexedInstanced(cmd.ElemCount, 1, cmd.IdxOffset + global_idx_offset, cmd.VtxOffset + global_vtx_offset, 0);
		}
		global_vtx_offset += cmd_list->VtxBuffer.Size;
		global_idx_offset += cmd_list->IdxBuffer.Size;
	}
}

void oetf(DX12Renderer& self, ID3D12GraphicsCommandList9* command_list, DescriptorIndex pre_oetf_rt)
{
	/*HRESULT result = S_OK;
	PAW_ASSERT_UNUSED(result);*/

	ID3D12DescriptorHeap* descriptor_heaps[]{self.cbv_srv_uav_descriptor_pool.heap, self.sampler_descriptor_pool.heap};
	command_list->SetDescriptorHeaps(PAW_ARRAY_COUNT(descriptor_heaps), descriptor_heaps);
	command_list->SetGraphicsRootSignature(self.oetf_pipeline_state.root_signature);
	command_list->SetPipelineState(self.oetf_pipeline_state.pso);
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DrawConstants
	{
		u32 image_index;
		u32 sampler_index;
	};
	PAW_ERROR_ON_PADDING_END

	const DrawConstants constants{
		.image_index = pre_oetf_rt.value,
		.sampler_index = self.reference_image_sampler_index.value,

	};

	command_list->SetGraphicsRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	command_list->DrawInstanced(6, 1, 0, 0);
}

void display_map_sdr(DX12Renderer& self, GameViewportFrameData const& game_viewport_frame_data, ID3D12GraphicsCommandList9* command_list)
{
	/*HRESULT result = S_OK;
	PAW_ASSERT_UNUSED(result);*/

	PIX_EVENT_SCOPE(command_list, "display_map_sdr");
	TracyD3D12Zone(self.profiler_ctx, command_list, "display_map_sdr");

	ID3D12DescriptorHeap* descriptor_heaps[]{self.cbv_srv_uav_descriptor_pool.heap};
	command_list->SetDescriptorHeaps(PAW_ARRAY_COUNT(descriptor_heaps), descriptor_heaps);
	command_list->SetComputeRootSignature(self.display_map_sdr_pipeline_state.root_signature);
	command_list->SetPipelineState(self.display_map_sdr_pipeline_state.pso);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DrawConstants
	{
		u32 image_index;
		u32 width;
		u32 height;
	};
	PAW_ERROR_ON_PADDING_END

	const Vec2S32 tex_size = self.image_store.get_size(game_viewport_frame_data.color_rt_resolved);

	const DrawConstants constants{
		.image_index = self.image_store.get_srv_descriptor_index(game_viewport_frame_data.color_rt_resolved).value,
		.width = u32(tex_size.x),
		.height = u32(tex_size.y),
	};

	command_list->SetComputeRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	command_list->Dispatch(UINT(math_ceil((f32)tex_size.x / (f32)self.display_map_sdr_pipeline_state.num_threads_x)), UINT((f32)tex_size.y / (f32)self.display_map_hdr_pipeline_state.num_threads_y), 1);
}

void display_map_hdr(DX12Renderer& self, GameViewportFrameData const& game_viewport_frame_data, ID3D12GraphicsCommandList9* command_list)
{
	/*HRESULT result = S_OK;
	PAW_ASSERT_UNUSED(result);*/

	PIX_EVENT_SCOPE(command_list, "display_map_hdr");
	TracyD3D12Zone(self.profiler_ctx, command_list, "display_map_hdr");

	ID3D12DescriptorHeap* descriptor_heaps[]{self.cbv_srv_uav_descriptor_pool.heap};
	command_list->SetDescriptorHeaps(PAW_ARRAY_COUNT(descriptor_heaps), descriptor_heaps);
	command_list->SetComputeRootSignature(self.display_map_hdr_pipeline_state.root_signature);
	command_list->SetPipelineState(self.display_map_hdr_pipeline_state.pso);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DrawConstants
	{
		u32 image_index;
		u32 width;
		u32 height;
	};
	PAW_ERROR_ON_PADDING_END

	const Vec2S32 tex_size = self.image_store.get_size(game_viewport_frame_data.color_rt_resolved);

	const DrawConstants constants{
		.image_index = self.image_store.get_srv_descriptor_index(game_viewport_frame_data.color_rt_resolved).value,
		.width = u32(tex_size.x),
		.height = u32(tex_size.y),
	};

	command_list->SetComputeRoot32BitConstants(0, sizeof(DrawConstants) / 4, &constants, 0);
	command_list->Dispatch(UINT(math_ceil((f32)tex_size.x / (f32)self.display_map_hdr_pipeline_state.num_threads_x)), UINT((f32)tex_size.y / (f32)self.display_map_hdr_pipeline_state.num_threads_y), 1);
}

static usize calc_alignment_offset(usize offset, usize alignment)
{
	PAW_ASSERT((alignment & (alignment - 1)) == 0); // check if power of 2

	usize alignment_offset = 0;
	usize alignment_mask = alignment - 1;
	if (offset & alignment_mask)
	{
		alignment_offset = alignment - (offset & alignment_mask);
	}
	return alignment_offset;
}

static usize align_offset_forward(usize offset, usize alignment)
{
	return offset + calc_alignment_offset(offset, alignment);
}

static void create_game_viewport_rt(GameViewportFrameData& frame_data, usize local_frame_index, ImageStore2& image_store, Vec2S32 game_viewport_size, u32 sample_count, ID3D12Device12* device, DescriptorPool& srv_pool, DescriptorPool& rtv_pool, DescriptorPool& dsv_pool, ID3D12Heap* msaa_heap, ID3D12Heap* heap, Vec2S32 client_size)
{
	const usize width = usize(math_maxS32(game_viewport_size.x, 1));
	const usize height = usize(math_maxS32(game_viewport_size.y, 1));

	{
		const usize bytes_per_texture = 8 * 4;
		const usize pixels_per_texture = usize(client_size.x) * usize(client_size.y);
		const usize aligned_texture_size = align_offset_forward(bytes_per_texture * pixels_per_texture, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);

		// const usize texture_count = 2;

		usize texture_index = 0;
		usize texture_offset = local_frame_index * (g_frames_in_flight - 1) + texture_index;
		usize texture_offset_bytes = texture_offset * aligned_texture_size;

		PAW_ASSERT(calc_alignment_offset(texture_offset_bytes, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT) == 0);

		const D3D12_CLEAR_VALUE color_clear_value{.Format = DXGI_FORMAT_R16G16B16A16_FLOAT, .Color = {0.33f, 0.33f, 0.33f, 1.0f}};
		frame_data.color_rt_msaa = image_store.create_placed(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, msaa_heap, texture_offset_bytes, device, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE, "Game Color RT MSAA"_str, sample_count, &color_clear_value);

		texture_index = 1;
		texture_offset = local_frame_index * (g_frames_in_flight - 1) + texture_index;
		texture_offset_bytes = texture_offset * aligned_texture_size;

		PAW_ASSERT(calc_alignment_offset(texture_offset_bytes, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT) == 0);

		// Reverse depth - clear to 0
		const D3D12_CLEAR_VALUE depth_clear_value{.Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = {.Depth = 0.0f}};
		frame_data.depth_rt_msaa = image_store.create_placed(width, height, DXGI_FORMAT_D32_FLOAT, msaa_heap, texture_offset_bytes, device, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE, "Game Depth RT MSAA"_str, sample_count, &depth_clear_value);
	}

	{
		// const usize texture_count = 1;
		const usize bytes_per_texture = 8;
		const usize pixels_per_texture = usize(client_size.x) * usize(client_size.y);
		const usize aligned_texture_size = align_offset_forward(bytes_per_texture * pixels_per_texture, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

		usize texture_index = 0;
		usize texture_offset = local_frame_index * (g_frames_in_flight - 1) + texture_index;
		usize texture_offset_bytes = texture_offset * aligned_texture_size;

		PAW_ASSERT(calc_alignment_offset(texture_offset_bytes, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT) == 0);

		frame_data.color_rt_resolved = image_store.create_placed(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, heap, texture_offset_bytes, device, D3D12_RESOURCE_FLAG_NONE, D3D12_BARRIER_LAYOUT_SHADER_RESOURCE, "Game Color RT Resolved"_str, 1);
	}
	image_store.create_rtv(frame_data.color_rt_msaa, rtv_pool, device);
	image_store.create_dsv(frame_data.depth_rt_msaa, dsv_pool, device);
	image_store.create_srv(frame_data.color_rt_resolved, srv_pool, device);
}

DX12Renderer& dx12_renderer_init(Platform& platform, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, void* graphics_adapter, GameStartupArgs const& startup_args, void* graphics_factory, PagePool* page_pool, PagePool* debug_page_pool, JobGraph* job_graph, Vec2S32 game_viewport_size)
{
	DX12Renderer& self = persistent_allocator.alloc_and_construct_single<DX12Renderer>();

	IDXGIAdapter1* adapter = reinterpret_cast<IDXGIAdapter1*>(graphics_adapter);
	IDXGIFactory7* factory = reinterpret_cast<IDXGIFactory7*>(graphics_factory);

	HWND window_handle = GetActiveWindow();

	HRESULT result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&self.device));
	PAW_ASSERT(SUCCEEDED(result));
	PAW_ASSERT_UNUSED(result);

	if (!startup_args.disable_gpu_debug)
	{
		ID3D12DebugDevice2* debug_device = nullptr;
		result = self.device->QueryInterface(&debug_device);
		PAW_ASSERT(SUCCEEDED(result));

		// debug_self.device->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);

		ID3D12InfoQueue1* info_queue = nullptr;
		result = self.device->QueryInterface(&info_queue);
		if (!SUCCEEDED(result))
		{
			PAW_LOG_ERROR("D3D: Failed to initialize info queue");
		}

		if (info_queue)
		{
			DWORD cookie = 0;
			result = info_queue->RegisterMessageCallback(
				&d3d12_message_callback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &cookie);
			PAW_ASSERT(SUCCEEDED(result));
		}
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS12 options_12{};
	result = self.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options_12, sizeof(options_12));
	PAW_ASSERT(SUCCEEDED(result));

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options_5{};
	result = self.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options_5, sizeof(options_5));
	PAW_ASSERT(SUCCEEDED(result));

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 options_7{};
	result = self.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options_7, sizeof(options_7));
	PAW_ASSERT(SUCCEEDED(result));

	D3D12_FEATURE_DATA_SHADER_MODEL shader_model{D3D_SHADER_MODEL_6_6};
	result = self.device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shader_model, sizeof(shader_model));
	PAW_ASSERT(SUCCEEDED(result));

	PAW_ASSERT(options_12.EnhancedBarriersSupported);

	for (u32 type = 0; type < QueueType_Count; ++type)
	{
		const D3D12_COMMAND_QUEUE_DESC queue_desc{
			.Type = g_queue_type_to_command_list_type[type],
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		};

		result = self.device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&self.command_queues[type]));
		PAW_ASSERT(SUCCEEDED(result));

		result = self.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&self.queue_fences[type]));
	}

	self.present_command_queue = self.command_queues[QueueType_Graphics];

	const Vec2S32 client_size = platform.get_drawable_size();

	self.hdr = startup_args.hdr.enabled;
	self.white_sdr_scale = startup_args.hdr.white_sdr_scale;

	self.swapchain_format = self.hdr ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
	self.swapchain_read_format = self.hdr ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	DXGI_SWAP_CHAIN_DESC1 swapchain_desc{
		.Width = static_cast<u32>(client_size.x),
		.Height = static_cast<u32>(client_size.y),
		.Format = self.swapchain_format,
		.SampleDesc =
			{
				.Count = 1,
			},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = g_frames_in_flight,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
	};

	IDXGISwapChain1* initial_swapchain = nullptr;
	// #TODO: Pass the actual window through or do this all in the platform layer
	result = factory->CreateSwapChainForHwnd(self.present_command_queue, window_handle, &swapchain_desc, nullptr, nullptr, &initial_swapchain);
	PAW_ASSERT(SUCCEEDED(result));

	result = initial_swapchain->QueryInterface(&self.swapchain);
	PAW_ASSERT(SUCCEEDED(result));

	self.swapchain->SetMaximumFrameLatency(2);
	self.swapchain_event = PlatformAutoResetEvent{u64(self.swapchain->GetFrameLatencyWaitableObject())};
	self.swapchain_event_waitable = job_graph_create_waitable(job_graph, platform.get_wait_handle(self.swapchain_event));
	result = self.swapchain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
	PAW_ASSERT(SUCCEEDED(result));

	self.device->SetStablePowerState(TRUE);

	factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER);

	self.cbv_srv_uav_descriptor_pool.init(self.device, persistent_allocator, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 32);
	self.sampler_descriptor_pool.init(self.device, persistent_allocator, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 8);
	self.rtv_descriptor_pool.init(self.device, persistent_allocator, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 32);
	self.dsv_descriptor_pool.init(self.device, persistent_allocator, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 32);

	for (u32 i = 0; i < g_frames_in_flight; ++i)
	{
		const DescriptorIndex descriptor_index = self.rtv_descriptor_pool.alloc();
		self.backbuffer_handles[i] = self.rtv_descriptor_pool.get_cpu(descriptor_index);
		result = self.swapchain->GetBuffer(i, IID_PPV_ARGS(&self.backbuffer_resources[i]));
		PAW_ASSERT(SUCCEEDED(result));

		set_debug_name(self.backbuffer_resources[i], "Backbuffer"_str);

		const D3D12_RENDER_TARGET_VIEW_DESC desc{
			.Format = self.swapchain_read_format,
			.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
		};

		self.device->CreateRenderTargetView(self.backbuffer_resources[i], &desc, self.backbuffer_handles[i]);
	}

	self.local_frame_index = self.swapchain->GetCurrentBackBufferIndex();

	result = self.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&self.present_fence));
	PAW_ASSERT(SUCCEEDED(result));
	self.present_fence_values[self.local_frame_index]++;

	self.present_fence_event = platform.create_auto_reset_event();
	self.present_fence_event_waitable = job_graph_create_waitable(job_graph, platform.get_wait_handle(self.present_fence_event));

	for (u32 type = 0; type < QueueType_Count; ++type)
	{
		const D3D12_COMMAND_LIST_TYPE d3d12_type = g_queue_type_to_command_list_type[type];

		for (u32 i = 0; i < g_frames_in_flight; ++i)
		{
			self.device->CreateCommandAllocator(d3d12_type, IID_PPV_ARGS(&self.command_allocators[type][i]));
		}

		self.command_list_pools[type].init(self.device, d3d12_type, persistent_allocator, 4);
	}

	self.image_store.init();

	result = CreateDirectory("temp", nullptr);
	PAW_ASSERT(SUCCEEDED(result) || result == ERROR_ALREADY_EXISTS);

	result = CreateDirectory("temp/shader_pdbs", nullptr);
	PAW_ASSERT(SUCCEEDED(result) || result == ERROR_ALREADY_EXISTS);

	ArenaAllocator shader_allocator{};
	shader_allocator.init(page_pool, "Shader Allocator"_str);

	{
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

		unsigned char* pixels;
		s32 width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		const GpuImageHandle font_texture_handle = self.image_store.create_and_allocate(usize(width), usize(height), DXGI_FORMAT_R8G8B8A8_UNORM, g_main_heap_props, self.device, D3D12_RESOURCE_FLAG_NONE, D3D12_BARRIER_LAYOUT_COPY_DEST, "ImGui Font Texture"_str);
		const D3D12_RESOURCE_DESC1& resource_desc = self.image_store.get_resource_desc(font_texture_handle);

		const u64 upload_pitch = (u64(width) * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
		const u64 upload_size = u64(height) * upload_pitch;
		const D3D12_RESOURCE_DESC font_upload_buffer_desc{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = 0,
			.Width = upload_size,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0,
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		ID3D12Resource* font_upload_buffer = nullptr;
		result = self.device->CreateCommittedResource(&g_upload_heap_props, D3D12_HEAP_FLAG_NONE, &font_upload_buffer_desc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, IID_PPV_ARGS(&font_upload_buffer));
		PAW_ASSERT(SUCCEEDED(result));

		set_debug_name(font_upload_buffer, "ImGui Font Upload Buffer"_str);

		void* mapped = nullptr;
		const D3D12_RANGE range{0, upload_size};
		result = font_upload_buffer->Map(0, &range, &mapped);
		PAW_ASSERT(SUCCEEDED(result));

		for (s32 y = 0; y < height; ++y)
		{
			memCopy(pixels + y * width * 4, (void*)((u64)mapped + y * upload_pitch), width * 4);
		}
		font_upload_buffer->Unmap(0, &range);

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		self.device->GetCopyableFootprints((D3D12_RESOURCE_DESC*)&resource_desc, 0, 1, 0, &footprint, nullptr, nullptr, nullptr);

		const D3D12_TEXTURE_COPY_LOCATION source_location{
			.pResource = font_upload_buffer,
			.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
			.PlacedFootprint = footprint,
		};

		const D3D12_TEXTURE_COPY_LOCATION dest_location{
			.pResource = self.image_store.get_resource(font_texture_handle),
			.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
			.SubresourceIndex = 0,
		};

		const QueueType queue_type = QueueType_Graphics;
		ID3D12CommandQueue* command_queue = self.command_queues[queue_type];
		ID3D12CommandAllocator* command_allocator = self.command_allocators[queue_type][self.local_frame_index];
		ID3D12GraphicsCommandList9* command_list;
		const SlotIndex command_list_index = self.command_list_pools[queue_type].allocGraphics(command_list);

		command_list->Reset(command_allocator, nullptr);

		command_list->CopyTextureRegion(&dest_location, 0, 0, 0, &source_location, nullptr);

		result = command_list->Close();
		PAW_ASSERT(SUCCEEDED(result));

		command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&command_list);

		self.command_list_pools[queue_type].free(command_list_index);

		wait_on_gpu(&self);

		font_upload_buffer->Release();

		self.imgui_font_image = font_texture_handle;

		self.imgui_font_index = self.image_store.create_srv(font_texture_handle, self.cbv_srv_uav_descriptor_pool, self.device);
		io.Fonts->SetTexID((ImTextureID)(u64)self.imgui_font_index.value);
	}

	{
		const D3D12_SAMPLER_DESC desc{
			.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		};

		self.imgui_sampler_index = self.sampler_descriptor_pool.alloc();

		self.device->CreateSampler(&desc, self.sampler_descriptor_pool.get_cpu(self.imgui_sampler_index));
	}

	{
		const D3D12_SAMPLER_DESC desc{
			.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
			.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		};

		self.reference_image_sampler_index = self.sampler_descriptor_pool.alloc();

		self.device->CreateSampler(&desc, self.sampler_descriptor_pool.get_cpu(self.reference_image_sampler_index));
	}

	{
		IDxcUtils* utils = nullptr;
		Defer utils_defer{[&utils]
						  { utils->Release(); }};
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));

		IDxcCompiler3* compiler = nullptr;
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		Defer compiler_defer{[compiler]
							 { compiler->Release(); }};

		IDxcIncludeHandler* include_handler = nullptr;
		Defer include_handler_defer{[&include_handler]
									{ include_handler->Release(); }};
		result = utils->CreateDefaultIncludeHandler(&include_handler);
		PAW_ASSERT(SUCCEEDED(result));

		{
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{
				.BlendState = {
					.RenderTarget = {{
						TRUE,
						FALSE,
						D3D12_BLEND_SRC_ALPHA,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_BLEND_ONE,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_LOGIC_OP_NOOP,
						D3D12_COLOR_WRITE_ENABLE_ALL,

					}},
				},

				.SampleMask = UINT_MAX,
				.RasterizerState = {
					.FillMode = D3D12_FILL_MODE_SOLID,
					.CullMode = D3D12_CULL_MODE_NONE,
				},
				.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
				.NumRenderTargets = 1,
				.RTVFormats = {self.game_color_format},
				.SampleDesc = {
					.Count = self.game_color_sample_count,
				},
			};

			self.triangle_pipeline_state = create_graphics_pipeline_state_object("source_data/shaders/triangle.hlsl"_str, "triangle"_str, desc, include_handler, compiler, utils, self.device, platform, shader_allocator);
		}
		{
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{
				.BlendState = {
					.RenderTarget = {{
						TRUE,
						FALSE,
						D3D12_BLEND_SRC_ALPHA,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_BLEND_ONE,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_LOGIC_OP_NOOP,
						D3D12_COLOR_WRITE_ENABLE_ALL,

					}},
				},

				.SampleMask = UINT_MAX,
				.RasterizerState = {
					.FillMode = D3D12_FILL_MODE_SOLID,
					.CullMode = D3D12_CULL_MODE_NONE,
				},
				.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
				.NumRenderTargets = 1,
				.RTVFormats = {self.hdr ? self.game_color_format : self.swapchain_read_format},
				.SampleDesc = {
					.Count = 1,
				},
			};

			self.imgui_pipeline_state = create_graphics_pipeline_state_object("source_data/shaders/imgui.hlsl"_str, "imgui"_str, desc, include_handler, compiler, utils, self.device, platform, shader_allocator);
		}

		{
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{
				.BlendState = {
					.RenderTarget = {{
						.BlendEnable = FALSE,
						.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
					}},
				},

				.SampleMask = UINT_MAX,
				.RasterizerState = {
					.FillMode = D3D12_FILL_MODE_SOLID,
					.CullMode = D3D12_CULL_MODE_NONE,
				},
				.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
				.NumRenderTargets = 1,
				.RTVFormats = {self.swapchain_read_format},
				.SampleDesc = {
					.Count = 1,
				},
			};

			self.oetf_pipeline_state = create_graphics_pipeline_state_object("source_data/shaders/oetf.hlsl"_str, "oetf"_str, desc, include_handler, compiler, utils, self.device, platform, shader_allocator);
		}

		{
			const CompiledShader compute_shader = compile_shader("source_data/shaders/display_map_sdr.hlsl"_str, ShaderType_Compute, include_handler, compiler, utils, shader_allocator, platform);

			D3D12_ROOT_PARAMETER root_param{
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants =
					{
						.ShaderRegister = 0,
						.RegisterSpace = 0,
						.Num32BitValues = compute_shader.num_constant_params,
					},
				.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
			};

			const D3D12_ROOT_SIGNATURE_DESC root_signature_desc{
				.NumParameters = compute_shader.num_constant_params > 0 ? 1u : 0,
				.pParameters = &root_param,
				.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
					D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED,
			};

			ID3DBlob* signature_blob = nullptr;
			ID3DBlob* signature_error_blob = nullptr;
			result = D3D12SerializeRootSignature(
				&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &signature_error_blob);
			PAW_ASSERT(SUCCEEDED(result));
			PAW_ASSERT(signature_error_blob == nullptr);

			// Defer defer_signature_blob{[signature_blob]
			//						   { signature_blob->Release(); }};
			// Defer defer_signature_error_blob{
			//	[signature_error_blob]
			//	{ signature_error_blob->Release(); },
			//	signature_error_blob != nullptr};

			ID3D12RootSignature* root_signature = nullptr;

			result = self.device->CreateRootSignature(
				0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));
			PAW_ASSERT(SUCCEEDED(result));

			const D3D12_COMPUTE_PIPELINE_STATE_DESC pso_desc{
				.pRootSignature = root_signature,
				.CS = {compute_shader.blob->GetBufferPointer(), compute_shader.blob->GetBufferSize()},
				.NodeMask = 0,
				.Flags = D3D12_PIPELINE_STATE_FLAG_NONE,
			};

			ID3D12PipelineState* pso = nullptr;
			result = self.device->CreateComputePipelineState(&pso_desc, IID_PPV_ARGS(&pso));
			PAW_ASSERT(SUCCEEDED(result));

			self.display_map_sdr_pipeline_state.pso = pso;
			self.display_map_sdr_pipeline_state.root_signature = root_signature;
			self.display_map_sdr_pipeline_state.num_threads_x = compute_shader.num_threads_x;
			self.display_map_sdr_pipeline_state.num_threads_y = compute_shader.num_threads_y;
			self.display_map_sdr_pipeline_state.num_threads_z = compute_shader.num_threads_z;
		}

		{
			const CompiledShader compute_shader = compile_shader("source_data/shaders/display_map_hdr.hlsl"_str, ShaderType_Compute, include_handler, compiler, utils, shader_allocator, platform);

			D3D12_ROOT_PARAMETER root_param{
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants =
					{
						.ShaderRegister = 0,
						.RegisterSpace = 0,
						.Num32BitValues = compute_shader.num_constant_params,
					},
				.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
			};

			const D3D12_ROOT_SIGNATURE_DESC root_signature_desc{
				.NumParameters = compute_shader.num_constant_params > 0 ? 1u : 0,
				.pParameters = &root_param,
				.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
					D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED,
			};

			ID3DBlob* signature_blob = nullptr;
			ID3DBlob* signature_error_blob = nullptr;
			result = D3D12SerializeRootSignature(
				&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &signature_error_blob);
			PAW_ASSERT(SUCCEEDED(result));
			PAW_ASSERT(signature_error_blob == nullptr);

			// Defer defer_signature_blob{[signature_blob]
			//						   { signature_blob->Release(); }};
			// Defer defer_signature_error_blob{
			//	[signature_error_blob]
			//	{ signature_error_blob->Release(); },
			//	signature_error_blob != nullptr};

			ID3D12RootSignature* root_signature = nullptr;

			result = self.device->CreateRootSignature(
				0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));
			PAW_ASSERT(SUCCEEDED(result));

			const D3D12_COMPUTE_PIPELINE_STATE_DESC pso_desc{
				.pRootSignature = root_signature,
				.CS = {compute_shader.blob->GetBufferPointer(), compute_shader.blob->GetBufferSize()},
				.NodeMask = 0,
				.Flags = D3D12_PIPELINE_STATE_FLAG_NONE,
			};

			ID3D12PipelineState* pso = nullptr;
			result = self.device->CreateComputePipelineState(&pso_desc, IID_PPV_ARGS(&pso));
			PAW_ASSERT(SUCCEEDED(result));

			self.display_map_hdr_pipeline_state.pso = pso;
			self.display_map_hdr_pipeline_state.root_signature = root_signature;
			self.display_map_hdr_pipeline_state.num_threads_x = compute_shader.num_threads_x;
			self.display_map_hdr_pipeline_state.num_threads_y = compute_shader.num_threads_y;
			self.display_map_hdr_pipeline_state.num_threads_z = compute_shader.num_threads_z;
		}

		{
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{
				.BlendState = {
					.RenderTarget = {{
						TRUE,
						FALSE,
						D3D12_BLEND_SRC_ALPHA,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_BLEND_ONE,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_LOGIC_OP_NOOP,
						D3D12_COLOR_WRITE_ENABLE_ALL,

					}},
				},

				.SampleMask = UINT_MAX,
				.RasterizerState = {
					.FillMode = D3D12_FILL_MODE_SOLID,
					.CullMode = D3D12_CULL_MODE_BACK,
					.FrontCounterClockwise = true,
				},
				.DepthStencilState = {
					.DepthEnable = TRUE,
					.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
					.DepthFunc = D3D12_COMPARISON_FUNC_GREATER,
				},
				.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
				.NumRenderTargets = 1,
				.RTVFormats = {self.game_color_format},
				.SampleDesc = {
					.Count = self.game_color_sample_count,
				},
			};

			self.debug_geom_pipeline_state = create_graphics_pipeline_state_object("source_data/shaders/debug_geom.hlsl"_str, "debug_geom"_str, desc, include_handler, compiler, utils, self.device, platform, shader_allocator);
		}

		{
			const CompiledShader mesh_shader = compile_shader("source_data/shaders/test_ms.hlsl"_str, ShaderType_Mesh, include_handler, compiler, utils, shader_allocator, platform);
			const CompiledShader amplification_shader = compile_shader("source_data/shaders/test_ms.hlsl"_str, ShaderType_Amplification, include_handler, compiler, utils, shader_allocator, platform);
			const CompiledShader pixel_shader = compile_shader("source_data/shaders/test_ms.hlsl"_str, ShaderType_Pixel, include_handler, compiler, utils, shader_allocator, platform);

			if (pixel_shader.num_constant_params > 0)
			{
				PAW_ASSERT(mesh_shader.num_constant_params == pixel_shader.num_constant_params);
			}

			const u32 max_num_constant_params = math_maxU32(mesh_shader.num_constant_params, math_maxU32(amplification_shader.num_constant_params, pixel_shader.num_constant_params));

			D3D12_ROOT_PARAMETER root_param{
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
				.Constants =
					{
						.ShaderRegister = 0,
						.RegisterSpace = 0,
						.Num32BitValues = max_num_constant_params,
					},
				.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
			};

			const D3D12_ROOT_SIGNATURE_DESC root_signature_desc{
				.NumParameters = max_num_constant_params > 0 ? 1u : 0,
				.pParameters = &root_param,
				.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
					D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED,
			};

			ID3DBlob* signature_blob = nullptr;
			ID3DBlob* signature_error_blob = nullptr;
			result = D3D12SerializeRootSignature(
				&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &signature_error_blob);
			PAW_ASSERT(SUCCEEDED(result));
			PAW_ASSERT(signature_error_blob == nullptr);

			// Defer defer_signature_blob{[signature_blob]
			//						   { signature_blob->Release(); }};
			// Defer defer_signature_error_blob{
			//	[signature_error_blob]
			//	{ signature_error_blob->Release(); },
			//	signature_error_blob != nullptr};

			ID3D12RootSignature* root_signature = nullptr;

			result = self.device->CreateRootSignature(
				0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));
			PAW_ASSERT(SUCCEEDED(result));

			const D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pso_desc{
				.pRootSignature = root_signature,
				.AS = {
					.pShaderBytecode = amplification_shader.blob->GetBufferPointer(),
					.BytecodeLength = amplification_shader.blob->GetBufferSize(),
				},
				.MS = {
					.pShaderBytecode = mesh_shader.blob->GetBufferPointer(),
					.BytecodeLength = mesh_shader.blob->GetBufferSize(),
				},

				.PS = {
					.pShaderBytecode = pixel_shader.blob->GetBufferPointer(),
					.BytecodeLength = pixel_shader.blob->GetBufferSize(),
				},

				.BlendState = {
					.RenderTarget = {{
						TRUE,
						FALSE,
						D3D12_BLEND_SRC_ALPHA,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_BLEND_ONE,
						D3D12_BLEND_INV_SRC_ALPHA,
						D3D12_BLEND_OP_ADD,
						D3D12_LOGIC_OP_NOOP,
						D3D12_COLOR_WRITE_ENABLE_ALL,

					}},
				},

				.SampleMask = UINT_MAX,
				.RasterizerState = {
					.FillMode = D3D12_FILL_MODE_SOLID,
					.CullMode = D3D12_CULL_MODE_BACK,
					.FrontCounterClockwise = true,
				},
				.DepthStencilState = {
					.DepthEnable = TRUE,
					.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
					.DepthFunc = D3D12_COMPARISON_FUNC_GREATER,
				},

				.NumRenderTargets = 2,
				.RTVFormats = {self.game_color_format, DXGI_FORMAT_D32_FLOAT},
				.SampleDesc = {
					.Count = self.game_color_sample_count,
				},
			};

			CD3DX12_PIPELINE_MESH_STATE_STREAM pso_stream{pso_desc};

			const D3D12_PIPELINE_STATE_STREAM_DESC pso_stream_desc{
				.SizeInBytes = sizeof(pso_stream),
				.pPipelineStateSubobjectStream = &pso_stream,
			};

			ID3D12PipelineState* pso = nullptr;
			result = self.device->CreatePipelineState(&pso_stream_desc, IID_PPV_ARGS(&pso));
			PAW_ASSERT(SUCCEEDED(result));

			mesh_shader.blob->Release();
			amplification_shader.blob->Release();
			pixel_shader.blob->Release();

			self.mesh_shader_pipeline_state.pso = pso;
			self.mesh_shader_pipeline_state.root_signature = root_signature;
		}
	}

	shader_allocator.reset();

	for (usize frame_index = 0; frame_index < g_frames_in_flight; ++frame_index)
	{
		PerFrameData& frame_data = self.per_frame_data[frame_index];
		const D3D12_CLEAR_VALUE clear_value{
			.Format = DXGI_FORMAT_R16G16B16A16_FLOAT,
			.Color = {0.33f, 0.33f, 0.33f, 1.0f},
		};
		frame_data.pre_oetf_rt = self.image_store.create_and_allocate(client_size.x, client_size.y, DXGI_FORMAT_R16G16B16A16_FLOAT, g_main_heap_props, self.device, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_BARRIER_LAYOUT_SHADER_RESOURCE, "Pre OETF RT"_str, 1, &clear_value);
		self.image_store.create_srv(frame_data.pre_oetf_rt, self.cbv_srv_uav_descriptor_pool, self.device);
		self.image_store.create_rtv(frame_data.pre_oetf_rt, self.rtv_descriptor_pool, self.device);
	}

	{
		const usize texture_count = 2;
		const usize bytes_per_texture = 8 * 4;
		const usize pixels_per_texture = usize(client_size.x) * usize(client_size.y);
		const usize aligned_texture_size = align_offset_forward(bytes_per_texture * pixels_per_texture, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);
		const usize total_size = texture_count * aligned_texture_size * g_frames_in_flight;
		PAW_ASSERT(calc_alignment_offset(total_size, D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT) == 0);
		const D3D12_HEAP_DESC heap_desc{
			.SizeInBytes = total_size,
			.Properties = g_main_heap_props,
			.Alignment = D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT,
			.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES,
		};

		result = self.device->CreateHeap(&heap_desc, IID_PPV_ARGS(&self.game_viewport_msaa_heap));
		PAW_ASSERT(SUCCEEDED(result));
	}

	{
		const usize texture_count = 1;
		const usize bytes_per_texture = 8;
		const usize pixels_per_texture = usize(client_size.x) * usize(client_size.y);
		const usize aligned_texture_size = align_offset_forward(bytes_per_texture * pixels_per_texture, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
		const usize total_size = texture_count * aligned_texture_size * g_frames_in_flight;
		PAW_ASSERT(calc_alignment_offset(total_size, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT) == 0);
		const D3D12_HEAP_DESC heap_desc{
			.SizeInBytes = total_size,
			.Properties = g_main_heap_props,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Flags = D3D12_HEAP_FLAG_NONE,
		};

		result = self.device->CreateHeap(&heap_desc, IID_PPV_ARGS(&self.game_viewport_normal_heap));
		PAW_ASSERT(SUCCEEDED(result));
	}

	for (usize frame_index = 0; frame_index < g_frames_in_flight; ++frame_index)
	{
		GameViewportFrameData& frame_data = self.game_viewport_frame_data[frame_index];
		create_game_viewport_rt(frame_data, self.local_frame_index, self.image_store, game_viewport_size, self.game_color_sample_count, self.device, self.cbv_srv_uav_descriptor_pool, self.rtv_descriptor_pool, self.dsv_descriptor_pool, self.game_viewport_msaa_heap, self.game_viewport_normal_heap, client_size);
	}
	self.profiler_ctx = TracyD3D12Context(self.device, self.present_command_queue);

	self.render_graph = render_graph::init(persistent_allocator, debug_persistent_allocator, *page_pool, *debug_page_pool, self.device, self.rtv_descriptor_pool, self.cbv_srv_uav_descriptor_pool, self.dsv_descriptor_pool);

	wait_on_gpu(&self);

	{
		PAW_PROFILER_SCOPE("Wait for swapchain event");
		// Wait for the previous present to complete. This should reduce composition latency
		WaitForSingleObjectEx(HANDLE(self.swapchain_event.handle), INFINITE, FALSE);
	}

	return self;
}

void dx12_renderer_deinit(DX12Renderer& self)
{
	self.swapchain->Release();
	self.device->Release();
}

// Based on https://vincent-p.github.io/posts/vulkan_perspective_matrix/
static Mat4 calc_camera_projection_matrix(f32 vertical_fov_radians, f32 viewport_width, f32 viewport_height, f32 near_z, f32 far_z)
{
	const f32 aspect_ratio = viewport_width / viewport_height;
	const f32 focal_length = 1.0f / math_tan(vertical_fov_radians * 0.5f);

	const f32 x = focal_length / aspect_ratio;
	const f32 y = focal_length;
	const f32 a = near_z / (far_z - near_z);
	const f32 b = far_z * a;

	return Mat4{{
		{x, 0.0f, 0.0f, 0.0f},
		{0.0f, y, 0.0f, 0.0f},
		{0.0f, 0.0f, a, -1.0f},
		{0.0f, 0.0f, b, 0.0},
	}};
}

static Mat4 calc_inverse_camera_projection_matrix(Mat4 const& projection)
{
	const f32 x = projection.data[0][0];
	const f32 y = projection.data[1][1];
	const f32 a = projection.data[2][2];
	const f32 b = projection.data[3][2];

	return Mat4{{
		{1.0f / x, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f / y, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f / b},
		{0.0f, 0.0f, -1.0f, a / b},
	}};
}

static void update(JobGraph*, JobHandle, DX12Renderer& self, Vec2S32 /*game_viewport_size*/, RendererFrameData const& /*frame_data*/, Platform& /*platform*/)
{
	HRESULT result;
	PAW_ASSERT_UNUSED(result);

	/*GameViewportFrameData& game_viewport_frame_data = self->game_viewport_frame_data[self->local_frame_index];

	const Vec2S32 client_size = platform->get_drawable_size();

	const Vec2S32 color_msaa_size = self->image_store.get_size(game_viewport_frame_data.color_rt_msaa);
	if (game_viewport_size != color_msaa_size)
	{
		self->image_store.remove_ref(game_viewport_frame_data.color_rt_msaa);
		self->image_store.remove_ref(game_viewport_frame_data.depth_rt_msaa);
		self->image_store.remove_ref(game_viewport_frame_data.color_rt_resolved);

		create_game_viewport_rt(game_viewport_frame_data, self->local_frame_index, self->image_store, game_viewport_size, self->game_color_sample_count, self->device, self->cbv_srv_uav_descriptor_pool, self->rtv_descriptor_pool, self->dsv_descriptor_pool, self->game_viewport_msaa_heap, self->game_viewport_normal_heap, client_size);

		game_viewport_frame_data.resized = true;
	}*/

	{
		// const Mat4 camera_projection = calc_camera_projection_matrix(math_toRadians(60.0f), (f32)game_viewport_size.x, (f32)game_viewport_size.y, 0.01f, 1000.0f) * frame_data->camera_mat;
	}

	render_graph::tick(self.render_graph);

	self.image_store.delete_pending_images(self.cbv_srv_uav_descriptor_pool, self.rtv_descriptor_pool, self.dsv_descriptor_pool, self.global_frame_index);

	// If the next frame is not ready to be rendered yet, wait until it is ready
	// if (self.present_fence->GetCompletedValue() < self.present_fence_values[self.local_frame_index])
	{
		result = self.present_fence->SetEventOnCompletion(self.present_fence_values[self.local_frame_index], HANDLE(self.present_fence_event.handle));
		PAW_ASSERT(SUCCEEDED(result));
		// WaitForSingleObjectEx(self.present_fence_event, INFINITE, FALSE);
	}
}

void render(JobGraph*, JobHandle, Platform& platform, DX12Renderer& self, FrameAllocator& /*frame_allocator*/, RendererFrameData const& frame_data, Vec2S32 game_viewport_size)
{
	const usize local_frame_index = self.local_frame_index;

	HRESULT result;
	PAW_ASSERT_UNUSED(result);

	TracyD3D12NewFrame(self.profiler_ctx);

	PerFrameData const& per_frame_data = self.per_frame_data[local_frame_index];

	self.image_store.add_frame_usage(per_frame_data.pre_oetf_rt, self.global_frame_index);

	PAW_ERROR_ON_PADDING_BEGIN
	struct DebugGeomLine
	{
		Vec4 color;
		Vec3 start;
		Vec3 end;
		f32 start_thickness;
		f32 end_thickness;
		f32 lifetime_seconds;
	};
	PAW_ERROR_ON_PADDING_END

	if (frame_data.game_state_debug_draw_state)
	{
		DebugCommandStore const& debug_command_store = frame_data.game_state_debug_draw_state->depth_tested_commands_3d;
		const usize total_needed_size = debug_command_store.commands_per_page * debug_command_store.page_count;
		if (self.debug_draw_3d_buffers[local_frame_index] == nullptr || self.debug_draw_3d_buffer_sizes[local_frame_index] < total_needed_size)
		{
			if (self.debug_draw_3d_buffers[local_frame_index] != nullptr)
			{
				self.debug_draw_3d_buffers[local_frame_index]->Release();
				self.cbv_srv_uav_descriptor_pool.free(self.debug_draw_3d_buffer_slots[local_frame_index]);
			}

			self.debug_draw_3d_buffer_sizes[local_frame_index] = total_needed_size;

			const D3D12_RESOURCE_DESC desc{
				.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
				.Width = total_needed_size * sizeof(DebugGeomLine),
				.Height = 1,
				.DepthOrArraySize = 1,
				.MipLevels = 1,
				.Format = DXGI_FORMAT_UNKNOWN,
				.SampleDesc = {.Count = 1},
				.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
				.Flags = D3D12_RESOURCE_FLAG_NONE,
			};

			result = self.device->CreateCommittedResource(&g_upload_heap_props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&self.debug_draw_3d_buffers[local_frame_index]));
			PAW_ASSERT(SUCCEEDED(result));
			self.debug_draw_3d_buffer_slots[local_frame_index] = self.cbv_srv_uav_descriptor_pool.alloc();

			const D3D12_SHADER_RESOURCE_VIEW_DESC buffer_view_desc{
				.Format = DXGI_FORMAT_R32_TYPELESS,
				.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Buffer = {
					.FirstElement = 0,
					.NumElements = ((u32)total_needed_size * sizeof(DebugGeomLine)) / 4,
					.Flags = D3D12_BUFFER_SRV_FLAG_RAW,
				},
			};

			self.device->CreateShaderResourceView(self.debug_draw_3d_buffers[local_frame_index], &buffer_view_desc, self.cbv_srv_uav_descriptor_pool.get_cpu(self.debug_draw_3d_buffer_slots[local_frame_index]));
		}

		{
			void* write_ptr_raw;
			result = self.debug_draw_3d_buffers[local_frame_index]->Map(0, nullptr, &write_ptr_raw);
			PAW_ASSERT(SUCCEEDED(result));
			byte* write_ptr = (byte*)write_ptr_raw;
			DebugCommandStore::PageHeader* page = debug_command_store.current_page;
			usize written_command_count = 0;
			while (page)
			{
				DebugDrawCommand3D* commands = page->getCommandsPtr();
				memCopy(commands, write_ptr + written_command_count * sizeof(DebugDrawCommand3D), sizeof(DebugDrawCommand3D) * page->count);
				written_command_count += page->count;
				page = page->prev;
			}

			self.debug_draw_3d_buffers[local_frame_index]->Unmap(0, nullptr);

			self.debug_draw_3d_command_counts[local_frame_index] = written_command_count;
		}
	}

	for (u32 i = 0; i < QueueType_Count; ++i)
	{
		self.command_allocators[i][local_frame_index]->Reset();
	}

	const QueueType queue_type = QueueType_Graphics;

	ID3D12CommandQueue* command_queue = self.command_queues[queue_type];

	ID3D12CommandAllocator* allocator = self.command_allocators[queue_type][self.local_frame_index];
	ID3D12CommandList* command_list_raw = nullptr;
	const SlotIndex command_list_slot = self.command_list_pools[queue_type].alloc(command_list_raw);
	ID3D12GraphicsCommandList9* command_list = nullptr;
	result = command_list_raw->QueryInterface(&command_list);
	PAW_ASSERT(SUCCEEDED(result));

	result = command_list->Reset(allocator, nullptr);
	PAW_ASSERT(SUCCEEDED(result));

	const Vec2S32 client_size = platform.get_drawable_size();

	Mat4 const camera_mat = calc_camera_projection_matrix(math_toRadians(60.0f), (f32)game_viewport_size.x, (f32)game_viewport_size.y, 0.01f, 1000.0f) * frame_data.camera_mat;
	self.temp_camera_mat = camera_mat;

	render_graph::render(self.render_graph, command_list, self.profiler_ctx, self.local_frame_index, self);

	{
		GameViewportFrameData& game_viewport_frame_data = self.game_viewport_frame_data[self.local_frame_index];

		const Vec2S32 color_msaa_size = self.image_store.get_size(game_viewport_frame_data.color_rt_msaa);
		if (game_viewport_size != color_msaa_size)
		{
			self.image_store.remove_ref(game_viewport_frame_data.color_rt_msaa);
			self.image_store.remove_ref(game_viewport_frame_data.depth_rt_msaa);
			self.image_store.remove_ref(game_viewport_frame_data.color_rt_resolved);

			self.image_store.delete_image(game_viewport_frame_data.color_rt_msaa, self.cbv_srv_uav_descriptor_pool, self.rtv_descriptor_pool, self.dsv_descriptor_pool, self.global_frame_index);
			self.image_store.delete_image(game_viewport_frame_data.depth_rt_msaa, self.cbv_srv_uav_descriptor_pool, self.rtv_descriptor_pool, self.dsv_descriptor_pool, self.global_frame_index);
			self.image_store.delete_image(game_viewport_frame_data.color_rt_resolved, self.cbv_srv_uav_descriptor_pool, self.rtv_descriptor_pool, self.dsv_descriptor_pool, self.global_frame_index);

			create_game_viewport_rt(game_viewport_frame_data, self.local_frame_index, self.image_store, game_viewport_size, self.game_color_sample_count, self.device, self.cbv_srv_uav_descriptor_pool, self.rtv_descriptor_pool, self.dsv_descriptor_pool, self.game_viewport_msaa_heap, self.game_viewport_normal_heap, client_size);

			game_viewport_frame_data.resized = true;
		}
	}

	GameViewportFrameData const& game_viewport_frame_data = self.game_viewport_frame_data[local_frame_index];

	{
		PIX_EVENT_SCOPE(command_list, "Game Viewport");
		TracyD3D12Zone(self.profiler_ctx, command_list, "Game Viewport");
		self.image_store.add_frame_usage(game_viewport_frame_data.color_rt_msaa, self.global_frame_index);
		self.image_store.add_frame_usage(game_viewport_frame_data.depth_rt_msaa, self.global_frame_index);
		self.image_store.add_frame_usage(game_viewport_frame_data.color_rt_resolved, self.global_frame_index);

		if (game_viewport_frame_data.resized)
		{
			/*const D3D12_GLOBAL_BARRIER barrier{

			};*/
		}

		{
			const D3D12_TEXTURE_BARRIER barrier{
				.SyncBefore = D3D12_BARRIER_SYNC_RESOLVE,
				.SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET,
				.AccessBefore = D3D12_BARRIER_ACCESS_RESOLVE_SOURCE,
				.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET,
				.LayoutBefore = D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE,
				.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET,
				.pResource = self.image_store.get_resource(game_viewport_frame_data.color_rt_msaa),
				.Subresources = {
					.IndexOrFirstMipLevel = 0,
					.NumMipLevels = 1,
					.FirstArraySlice = 0,
					.NumArraySlices = 1,
					.FirstPlane = 0,
					.NumPlanes = 1,
				},
				.Flags = D3D12_TEXTURE_BARRIER_FLAG_DISCARD,
			};

			const D3D12_BARRIER_GROUP group{
				.Type = D3D12_BARRIER_TYPE_TEXTURE,
				.NumBarriers = 1,
				.pTextureBarriers = &barrier,
			};

			command_list->Barrier(1, &group);
		}

		// const Vec2S32 game_viewport_size = self.image_store.get_size(game_viewport_frame_data.color_rt_msaa);

		const D3D12_VIEWPORT viewport{
			.TopLeftX = 0.0f,
			.TopLeftY = 0.0f,
			.Width = (f32)game_viewport_size.x,
			.Height = (f32)game_viewport_size.y,
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f,
		};

		const D3D12_RECT scissor_rect{
			.left = 0,
			.top = 0,
			.right = game_viewport_size.x,
			.bottom = game_viewport_size.y,
		};

		command_list->RSSetViewports(1, &viewport);
		command_list->RSSetScissorRects(1, &scissor_rect);

		const D3D12_CPU_DESCRIPTOR_HANDLE game_color_rt_msaa_cpu_handle = self.rtv_descriptor_pool.get_cpu(self.image_store.get_rtv_descriptor_index(game_viewport_frame_data.color_rt_msaa));
		const D3D12_CPU_DESCRIPTOR_HANDLE game_depth_rt_msaa_cpu_handle = self.dsv_descriptor_pool.get_cpu(self.image_store.get_dsv_descriptor_index(game_viewport_frame_data.depth_rt_msaa));

		const FLOAT clear_color[4]{0.33f, 0.33f, 0.33f, 1.0f};
		command_list->OMSetRenderTargets(1, &game_color_rt_msaa_cpu_handle, false, &game_depth_rt_msaa_cpu_handle);
		command_list->ClearRenderTargetView(game_color_rt_msaa_cpu_handle, clear_color, 0, nullptr);
		// Reverse depth - clear to 0
		command_list->ClearDepthStencilView(game_depth_rt_msaa_cpu_handle, D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);

		draw_triangle_node(self, command_list, camera_mat);

		ID3D12Resource* color_rt_msaa_resource = self.image_store.get_resource(game_viewport_frame_data.color_rt_msaa);
		ID3D12Resource* color_rt_resolved_resource = self.image_store.get_resource(game_viewport_frame_data.color_rt_resolved);

		{
			const D3D12_TEXTURE_BARRIER rt_to_resolve_dest_barrier{
				.SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET,
				.SyncAfter = D3D12_BARRIER_SYNC_RESOLVE,
				.AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET,
				.AccessAfter = D3D12_BARRIER_ACCESS_RESOLVE_SOURCE,
				.LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET,
				.LayoutAfter = D3D12_BARRIER_LAYOUT_RESOLVE_SOURCE,
				.pResource = color_rt_msaa_resource,
				.Subresources = {
					.IndexOrFirstMipLevel = 0,
					.NumMipLevels = 1,
					.FirstArraySlice = 0,
					.NumArraySlices = 1,
					.FirstPlane = 0,
					.NumPlanes = 1,
				},
				.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE,
			};

			const D3D12_TEXTURE_BARRIER shader_resource_to_resolve_src_barrier{
				.SyncBefore = D3D12_BARRIER_SYNC_PIXEL_SHADING,
				.SyncAfter = D3D12_BARRIER_SYNC_RESOLVE,
				.AccessBefore = D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
				.AccessAfter = D3D12_BARRIER_ACCESS_RESOLVE_DEST,
				.LayoutBefore = D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
				.LayoutAfter = D3D12_BARRIER_LAYOUT_RESOLVE_DEST,
				.pResource = color_rt_resolved_resource,
				.Subresources = {
					.IndexOrFirstMipLevel = 0,
					.NumMipLevels = 1,
					.FirstArraySlice = 0,
					.NumArraySlices = 1,
					.FirstPlane = 0,
					.NumPlanes = 1,
				},
				.Flags = D3D12_TEXTURE_BARRIER_FLAG_DISCARD,
			};

			const D3D12_TEXTURE_BARRIER barriers[]{rt_to_resolve_dest_barrier, shader_resource_to_resolve_src_barrier};

			const D3D12_BARRIER_GROUP group{
				.Type = D3D12_BARRIER_TYPE_TEXTURE,
				.NumBarriers = PAW_ARRAY_COUNT(barriers),
				.pTextureBarriers = barriers,
			};

			command_list->Barrier(1, &group);
		}

		command_list->ResolveSubresource(color_rt_resolved_resource, 0, color_rt_msaa_resource, 0, self.image_store.get_resource_desc(game_viewport_frame_data.color_rt_resolved).Format);

		{
			const D3D12_TEXTURE_BARRIER resolve_dst_to_shader_resource_src_barrier{
				.SyncBefore = D3D12_BARRIER_SYNC_RESOLVE,
				.SyncAfter = D3D12_BARRIER_SYNC_COMPUTE_SHADING,
				.AccessBefore = D3D12_BARRIER_ACCESS_RESOLVE_DEST,
				.AccessAfter = D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
				.LayoutBefore = D3D12_BARRIER_LAYOUT_RESOLVE_DEST,
				.LayoutAfter = D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
				.pResource = color_rt_resolved_resource,
				.Subresources = {
					.IndexOrFirstMipLevel = 0,
					.NumMipLevels = 1,
					.FirstArraySlice = 0,
					.NumArraySlices = 1,
					.FirstPlane = 0,
					.NumPlanes = 1,
				},
				.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE,
			};

			const D3D12_TEXTURE_BARRIER barriers[]{resolve_dst_to_shader_resource_src_barrier};

			const D3D12_BARRIER_GROUP group{
				.Type = D3D12_BARRIER_TYPE_TEXTURE,
				.NumBarriers = PAW_ARRAY_COUNT(barriers),
				.pTextureBarriers = barriers,
			};

			command_list->Barrier(1, &group);
		}

		if (self.hdr)
		{
			// display_map_hdr(self, game_viewport_frame_data, command_list);
		}
		else
		{
			display_map_sdr(self, game_viewport_frame_data, command_list);
		}
	}

	const D3D12_VIEWPORT viewport{
		.TopLeftX = 0.0f,
		.TopLeftY = 0.0f,
		.Width = (f32)client_size.x,
		.Height = (f32)client_size.y,
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};

	const D3D12_RECT scissor_rect{
		.left = 0,
		.top = 0,
		.right = client_size.x,
		.bottom = client_size.y,
	};

	command_list->RSSetViewports(1, &viewport);
	command_list->RSSetScissorRects(1, &scissor_rect);

	{
		const D3D12_TEXTURE_BARRIER barrier{
			.SyncBefore = D3D12_BARRIER_SYNC_NONE,
			.SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET,
			.AccessBefore = D3D12_BARRIER_ACCESS_NO_ACCESS,
			.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET,
			.LayoutBefore = D3D12_BARRIER_LAYOUT_PRESENT,
			.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET,
			.pResource = self.backbuffer_resources[self.local_frame_index],
			.Subresources = {
				.IndexOrFirstMipLevel = 0,
				.NumMipLevels = 1,
				.FirstArraySlice = 0,
				.NumArraySlices = 1,
				.FirstPlane = 0,
				.NumPlanes = 1,
			},
			.Flags = D3D12_TEXTURE_BARRIER_FLAG_DISCARD,
		};

		const D3D12_BARRIER_GROUP group{
			.Type = D3D12_BARRIER_TYPE_TEXTURE,
			.NumBarriers = 1,
			.pTextureBarriers = &barrier,
		};

		command_list->Barrier(1, &group);
	}

	if (self.hdr)
	{

		{
			const D3D12_TEXTURE_BARRIER barrier{
				.SyncBefore = D3D12_BARRIER_SYNC_PIXEL_SHADING,
				.SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET,
				.AccessBefore = D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
				.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET,
				.LayoutBefore = D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
				.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET,
				.pResource = self.image_store.get_resource(per_frame_data.pre_oetf_rt),
				.Subresources = {
					.IndexOrFirstMipLevel = 0,
					.NumMipLevels = 1,
					.FirstArraySlice = 0,
					.NumArraySlices = 1,
					.FirstPlane = 0,
					.NumPlanes = 1,
				},
				.Flags = D3D12_TEXTURE_BARRIER_FLAG_DISCARD,
			};

			const D3D12_BARRIER_GROUP group{
				.Type = D3D12_BARRIER_TYPE_TEXTURE,
				.NumBarriers = 1,
				.pTextureBarriers = &barrier,
			};

			command_list->Barrier(1, &group);
		}

		const DescriptorIndex pre_oetf_rt_rtv = self.image_store.get_rtv_descriptor_index(per_frame_data.pre_oetf_rt);
		const D3D12_CPU_DESCRIPTOR_HANDLE pre_oetf_rt_rtv_handle = self.rtv_descriptor_pool.get_cpu(pre_oetf_rt_rtv);

		const FLOAT clear_color[4]{0.33f, 0.33f, 0.33f, 1.0f};
		command_list->OMSetRenderTargets(1, &pre_oetf_rt_rtv_handle, false, nullptr);
		command_list->ClearRenderTargetView(pre_oetf_rt_rtv_handle, clear_color, 0, nullptr);

		draw_imgui(self, command_list);

		command_list->RSSetViewports(1, &viewport);
		command_list->RSSetScissorRects(1, &scissor_rect);

		command_list->OMSetRenderTargets(1, &self.backbuffer_handles[self.local_frame_index], false, nullptr);

		{
			const D3D12_TEXTURE_BARRIER barrier{
				.SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET,
				.SyncAfter = D3D12_BARRIER_SYNC_PIXEL_SHADING,
				.AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET,
				.AccessAfter = D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
				.LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET,
				.LayoutAfter = D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
				.pResource = self.image_store.get_resource(per_frame_data.pre_oetf_rt),
				.Subresources = {
					.IndexOrFirstMipLevel = 0,
					.NumMipLevels = 1,
					.FirstArraySlice = 0,
					.NumArraySlices = 1,
					.FirstPlane = 0,
					.NumPlanes = 1,
				},
				.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE,
			};

			const D3D12_BARRIER_GROUP group{
				.Type = D3D12_BARRIER_TYPE_TEXTURE,
				.NumBarriers = 1,
				.pTextureBarriers = &barrier,
			};

			command_list->Barrier(1, &group);
		}

		oetf(self, command_list, self.image_store.get_srv_descriptor_index(per_frame_data.pre_oetf_rt));
	}
	else
	{
		const FLOAT clear_color[4]{0.33f, 0.33f, 0.33f, 1.0f};
		command_list->OMSetRenderTargets(1, &self.backbuffer_handles[local_frame_index], false, nullptr);
		command_list->ClearRenderTargetView(self.backbuffer_handles[local_frame_index], clear_color, 0, nullptr);
		draw_imgui(self, command_list);
	}

	{
		const D3D12_TEXTURE_BARRIER barrier{
			.SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET,
			.SyncAfter = D3D12_BARRIER_SYNC_NONE,
			.AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET,
			.AccessAfter = D3D12_BARRIER_ACCESS_NO_ACCESS,
			.LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET,
			.LayoutAfter = D3D12_BARRIER_LAYOUT_PRESENT,
			.pResource = self.backbuffer_resources[self.local_frame_index],
			.Subresources = {
				.IndexOrFirstMipLevel = 0,
				.NumMipLevels = 1,
				.FirstArraySlice = 0,
				.NumArraySlices = 1,
				.FirstPlane = 0,
				.NumPlanes = 1,
			},
			.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE,
		};

		const D3D12_BARRIER_GROUP group{
			.Type = D3D12_BARRIER_TYPE_TEXTURE,
			.NumBarriers = 1,
			.pTextureBarriers = &barrier,
		};

		command_list->Barrier(1, &group);
	}

	result = command_list->Close();
	PAW_ASSERT(SUCCEEDED(result));

	{
		PAW_PROFILER_SCOPE("ExecuteCommandLists");
		command_queue->ExecuteCommandLists(1, &command_list_raw);
	}

	self.command_list_pools[queue_type].free(command_list_slot);

	{
		PAW_PROFILER_SCOPE("Present");
		result = self.swapchain->Present(1, 0);
		PAW_ASSERT(SUCCEEDED(result));
	}

	TracyD3D12Collect(self.profiler_ctx);

	self.fence_value++;

	// Schedule a signal command in the queue
	result = self.present_command_queue->Signal(self.present_fence, self.fence_value);
	PAW_ASSERT_UNUSED(result);

	self.present_fence_values[self.local_frame_index] = self.fence_value;

	self.local_frame_index = self.swapchain->GetCurrentBackBufferIndex();
	PAW_ASSERT(self.local_frame_index < g_frames_in_flight);

	self.global_frame_index++;
}

JobHandle dx12_renderer_get_graph(JobGraph* graph, JobHandle const& root_job, DX12Renderer& self, Vec2S32 game_viewport_size, RendererFrameData const& frame_data, Platform& platform, FrameAllocator& frame_allocator)
{
	PAW_PROFILER_FUNC();
	const JobHandle swapchain_wait_handle = job_graph_add_wait(graph, JOB_SRC, "Swapchain wait"_str, {root_job}, self.swapchain_event_waitable);
	const JobHandle update_handle = job_graph_add_job(graph, JOB_SRC, "DX12Renderer update"_str, {swapchain_wait_handle}, update, self, game_viewport_size, frame_data, platform);
	const JobHandle wait_handle = job_graph_add_wait(graph, JOB_SRC, "Wait For Present"_str, {update_handle}, self.present_fence_event_waitable);
	const JobHandle render_handle = job_graph_add_job(graph, JOB_SRC, "DX12Renderer render"_str, {wait_handle}, render, platform, self, frame_allocator, frame_data, game_viewport_size);
	return render_handle;
}

DescriptorIndex get_game_viewport_descriptor_index_for_current_frame(DX12Renderer const& self)
{
	return self.image_store.get_srv_descriptor_index(self.game_viewport_frame_data[self.local_frame_index].color_rt_resolved);
}

CompiledShader compile_shader(UTF8StringView const& shader_path, ShaderType type, IDxcIncludeHandler* include_handler, IDxcCompiler3* compiler, IDxcUtils* utils, ArenaAllocator& allocator, Platform& platform)
{
	Slice<byte> const test_shader_file = platform.DEBUG_load_file_blocking(shader_path, allocator);

	HRESULT result = S_OK;
	PAW_ASSERT_UNUSED(result);
	const DxcBuffer source{
		.Ptr = test_shader_file.ptr,
		.Size = test_shader_file.size,
		.Encoding = DXC_CP_UTF8,
	};

	static constexpr LPCWSTR shader_entry_points[ShaderType_Count]{
		L"VSMain",
		L"PSMain",
		L"ASMain",
		L"MSMain",
		L"CSMain",
	};

	static constexpr LPCWSTR shader_versions[ShaderType_Count]{
		L"vs_6_6",
		L"ps_6_6",
		L"as_6_6",
		L"ms_6_6",
		L"cs_6_6",
	};

	IDxcBlob* shader_blob = nullptr;

	LPCWSTR compiler_args[]{
		L"-E",
		shader_entry_points[type],
		L"-Fd",
		L"temp\\shader_pdbs\\",
		L"-T",
		shader_versions[type],
		L"-HV",
		L"2021",
		L"-O1",
		L"-Zi",
		L"-Qstrip_reflect",
	};

	IDxcResult* compile_result = nullptr;
	result = compiler->Compile(
		&source, compiler_args, PAW_ARRAY_COUNT(compiler_args), include_handler, IID_PPV_ARGS(&compile_result));
	PAW_ASSERT(SUCCEEDED(result));
	/*Defer defer_compile_result{[compile_result]
							   { compile_result->Release(); }};*/

	IDxcBlobUtf8* errors = nullptr;
	result = compile_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
	PAW_ASSERT(SUCCEEDED(result));
	Defer defer_errors{[errors]
					   { errors->Release(); }};

	if (errors->GetStringLength() != 0)
	{
		char const* error_str = errors->GetStringPointer();
		const usize error_str_len = errors->GetStringLength();
		PAW_LOG_INFO("Shader (%s): %.*s", shader_path.ptr, static_cast<int>(error_str_len), error_str);
	}

	compile_result->GetStatus(&result);
	if (FAILED(result))
	{
		PAW_LOG_ERROR("Failed to compile %s", shader_path.ptr);
	}

	result = compile_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader_blob), nullptr);
	PAW_ASSERT(SUCCEEDED(result));

	//{
	//	FILE* file = nullptr;
	//	fopen_s(&file, "compiled_data/shaders/test.vertex.bin", "wb");
	//	fwrite(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), 1, file);
	//	fclose(file);
	//}

	IDxcBlob* reflection_data_blob = nullptr;
	result = compile_result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflection_data_blob), nullptr);
	PAW_ASSERT(SUCCEEDED(result));
	Defer defer_reflection_data_blob{[reflection_data_blob]
									 { reflection_data_blob->Release(); }};
	const DxcBuffer reflection_data{
		.Ptr = reflection_data_blob->GetBufferPointer(),
		.Size = reflection_data_blob->GetBufferSize(),
		.Encoding = DXC_CP_ACP,
	};

	ID3D12ShaderReflection* reflection = nullptr;
	utils->CreateReflection(&reflection_data, IID_PPV_ARGS(&reflection));
	Defer defer_reflection{[reflection]
						   { reflection->Release(); }};

	D3D12_SHADER_DESC shader_desc{};
	reflection->GetDesc(&shader_desc);

	u32 num_constant_params = 0;
	if (shader_desc.ConstantBuffers > 0)
	{
		ID3D12ShaderReflectionConstantBuffer* constants = reflection->GetConstantBufferByIndex(0);
		if (constants)
		{
			D3D12_SHADER_BUFFER_DESC constants_desc{};
			result = constants->GetDesc(&constants_desc);
			PAW_ASSERT(SUCCEEDED(result));
			num_constant_params = constants_desc.Size / 4;
		}
	}

	UINT num_threads_x = 0;
	UINT num_threads_y = 0;
	UINT num_threads_z = 0;

	reflection->GetThreadGroupSize(&num_threads_x, &num_threads_y, &num_threads_z);

	// TODO: Use this once renderdoc supports d3d12 shader debugging
	IDxcBlob* pdb_blob = nullptr;
	IDxcBlobWide* pdb_name_blob = nullptr;
	result = compile_result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb_blob), &pdb_name_blob);
	PAW_ASSERT(SUCCEEDED(result));
	Defer defer_pdb_blob{[&pdb_blob]
						 { pdb_blob->Release(); }};
	Defer defer_pdb_name_blob{[&pdb_name_blob]
							  { pdb_name_blob->Release(); }};
	{
		wchar_t path[MAX_PATH];
		LPCWSTR filename = pdb_name_blob->GetStringPointer();
		wsprintfW(path, L"temp\\shader_pdbs\\%s", filename);
		FILE* file = nullptr;
		_wfopen_s(&file, path, L"wb");
		fwrite(pdb_blob->GetBufferPointer(), pdb_blob->GetBufferSize(), 1, file);
		fclose(file);
	}

	return CompiledShader{
		.blob = shader_blob,
		.num_constant_params = num_constant_params,
		.num_threads_x = num_threads_x,
		.num_threads_y = num_threads_y,
		.num_threads_z = num_threads_z,
	};
}

GraphicsPipelineState create_graphics_pipeline_state_object(
	UTF8StringView const& shader_path, UTF8StringView const& /*name*/, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, IDxcIncludeHandler* include_handler, IDxcCompiler3* compiler, IDxcUtils* utils, ID3D12Device* device, Platform& platform, ArenaAllocator& allocator)
{
	const CompiledShader vertex_shader = compile_shader(shader_path, ShaderType_Vertex, include_handler, compiler, utils, allocator, platform);
	const CompiledShader pixel_shader = compile_shader(shader_path, ShaderType_Pixel, include_handler, compiler, utils, allocator, platform);

	const u32 num_constant_params = math_maxU32(vertex_shader.num_constant_params, pixel_shader.num_constant_params);

	D3D12_ROOT_PARAMETER root_param{
		.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
		.Constants =
			{
				.ShaderRegister = 0,
				.RegisterSpace = 0,
				.Num32BitValues = num_constant_params,
			},
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
	};

	const D3D12_ROOT_SIGNATURE_DESC root_signature_desc{
		.NumParameters = num_constant_params > 0 ? 1u : 0,
		.pParameters = &root_param,
		.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
			D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED,
	};

	ID3DBlob* signature_blob = nullptr;
	ID3DBlob* signature_error_blob = nullptr;
	HRESULT result = D3D12SerializeRootSignature(
		&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &signature_error_blob);
	PAW_ASSERT(SUCCEEDED(result));
	PAW_ASSERT_UNUSED(result);

	Defer defer_signature_blob{[signature_blob]
							   { signature_blob->Release(); }};
	Defer defer_signature_error_blob{
		[signature_error_blob]
		{ signature_error_blob->Release(); },
		signature_error_blob != nullptr};

	ID3D12RootSignature* root_signature = nullptr;

	result = device->CreateRootSignature(
		0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));
	PAW_ASSERT(SUCCEEDED(result));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = desc;
	pso_desc.pRootSignature = root_signature;
	pso_desc.VS = {
		.pShaderBytecode = vertex_shader.blob->GetBufferPointer(),
		.BytecodeLength = vertex_shader.blob->GetBufferSize(),
	};
	pso_desc.PS = {
		.pShaderBytecode = pixel_shader.blob->GetBufferPointer(),
		.BytecodeLength = pixel_shader.blob->GetBufferSize(),
	};
	ID3D12PipelineState* pso = nullptr;
	result = device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pso));
	PAW_ASSERT(SUCCEEDED(result));

	vertex_shader.blob->Release();
	pixel_shader.blob->Release();

	return {pso, root_signature};
}
