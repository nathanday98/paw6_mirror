#pragma once

#include <shared/std.h>
#include <shared/math.h>
#include <shared/slice.h>

struct Platform;
struct PlatformEvent;
struct UTF8StringView;
struct GameStartupArgs;
struct EditorInitialRenderData;
struct EditorMesh;
struct Allocator;
struct DebugDrawState;
struct PagePool;

struct GpuBufferView
{
	u64 handle;
	u64 memory_handle;
	usize size;
};

enum GpuBufferType
{
	GpuBufferType_Storage,
	GpuBufferType_Index,
	GpuBufferType_Count,
};

enum GpuTextureFormat
{
	GpuTextureFormat_R8G8B8A8Unorm,
};

enum GpuBufferMemoryProps
{
	GpuBufferMemoryProps_CPUVisible = 1 << 0,
	GpuBufferMemoryProps_GPUCPUSync = 1 << 1,
	GpuBufferMemoryProps_GPULocal = 1 << 2
};

struct StaticMesh
{
	Mat4 transform;
	u32 vertex_offset;
	u32 vertex_count;
};

struct StaticSceneVertex
{
	Vec3 position;
	Vec3 normal;
};

struct StaticScene
{
	u32 vertex_buffer_slot;
	Slice<StaticMesh> meshes;
};

struct RendererFrameData;

typedef u64 GpuImageHandle;

void rendererInit(Platform& platform, GameStartupArgs const& startup_args, Allocator& persistent_allocator, PagePool& page_pool);
void rendererDeinit();
void rendererProcessEvent(PlatformEvent const& event);
void rendererBeginFrame();
void rendererRender(Platform& platform, RendererFrameData const& render_frame_data);
void rendererOnShaderModification(Platform& platform, UTF8StringView const& path);

void rendererPushStaticScene(StaticScene const& scene);

GpuBufferView rendererCreateAndAllocateBuffer(GpuBufferType type, usize size, GpuBufferMemoryProps memory_props);
void rendererDestroyAndFreeBuffer(GpuBufferView buffer);
void* rendererMapBuffer(GpuBufferView buffer, usize offset, usize size);
void rendererUnmapBuffer(GpuBufferView buffer);
GpuBufferView rendererCreateAndUploadBuffer(Slice<byte const> const& data, GpuBufferType type);
u32 rendererPushBufferToShader(GpuBufferView const& buffer);
GpuImageHandle rendererCreateAndUploadTexture(Slice<byte const> data, GpuTextureFormat format, usize width, usize height, UTF8StringView const& debug_name);
u32 rendererPushTextureToShader(GpuImageHandle handle);
void rendererTextureRemoveRef(GpuImageHandle handle);

GpuImageHandle rendererGameViewportGetImage();
void rendererGameViewportResize(usize new_width, usize new_height);
void rendererGameViewportAddDebugLine2D(Vec2 const& start, Vec2 const& end, LinearColor const& color, f32 thickness, Vec2 const& min_uv, Vec2 const& max_uv, u32 texture_index);
Mat4 rendererGetCameraProjectionMat(f32 vertical_fov_radians, f32 viewport_width, f32 viewport_height, f32 near, f32 far);
Mat4 rendererGetInverseCameraProjectionMat(Mat4 const& projection);
