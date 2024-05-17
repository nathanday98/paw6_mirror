#pragma once

#include <shared/std.h>
#include <shared/math.h>
#include <shared/slice.h>

#include "new_renderer.h"

struct Allocator;

PAW_ERROR_ON_PADDING_BEGIN
struct SwfVertex
{
	Vec2 position;
	Vec2 uv;
	Vec4 color_data;
};
PAW_ERROR_ON_PADDING_END

struct SwfShape
{
	u32 index_offset;
	u32 index_count;
	u32 vertex_offset;
	f32 min_x;
	f32 min_y;
	f32 max_x;
	f32 max_y;
};

struct SwfStore
{
	u32 vertex_buffer_slot;
	GpuBufferView index_buffer;
	u32 gradient_texture_slot;
	GpuImageHandle gradient_image;
	u32 gradient_count;
	Slice<SwfShape> shapes;
	usize shape_count;
	f32 min_x;
	f32 min_y;
	f32 max_x;
	f32 max_y;
};

SwfStore loadSwfFromMemory(const Slice<byte>& data, Allocator& allocator, bool debug, Allocator& persistent_allocator);
void unloadSwf(SwfStore* swf);