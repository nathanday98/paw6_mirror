#pragma once

#include <shared/std.h>
#include <shared/math.h>

struct SkinVertex
{
	u32 bone_indices[4];
	f32 bone_weights[4];
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
	f32 padding;
};

struct StaticVertex
{
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};

struct SkeletalMesh
{
	u32 vertex_buffer_slot;
	usize vertex_count;
	usize index_count;
};