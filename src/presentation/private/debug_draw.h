#pragma once

#include <shared/std.h>
#include <shared/math.h>

#include <float.h>

struct Transform;
struct UTF8StringView;
struct Platform;
struct Allocator;
struct PagePool;
template <typename T>
struct Slice;

struct DebugDrawCommand3D
{
	Vec4 color;
	Vec3 start;
	Vec3 end;
	f32 start_thickness;
	f32 end_thickness;
	f32 remaining_time_seconds;
};

#define PAW_LIFETIME_INFINITE FLT_MAX

struct DebugCommandStore
{
	struct PageHeader
	{
		PageHeader* prev;
		usize count;

		const DebugDrawCommand3D* getCommandsPtr() const;
		DebugDrawCommand3D* getCommandsPtr();
	};

	void init(PagePool* debug_page_pool);
	void deinit();
	void push(const Slice<const DebugDrawCommand3D>& commands);
	void tickLifetimes(f32 delta_time_seconds);
	void cloneFrom(const DebugCommandStore& other);

	PagePool* page_pool;
	usize page_size;
	usize commands_per_page;
	PageHeader* current_page;
	usize page_count;
};

struct DebugDrawState
{
	DebugCommandStore depth_tested_commands_3d;
	DebugCommandStore commands_3d;
};

void debugDrawInit(Platform& platform, Allocator& persistent_allocator, PagePool* debug_page_pool);
void debugDrawDeinit();
void debugDrawTickLifetimes(f32 delta_time_seconds);
void debugDrawSetState(DebugDrawState* state);

void debugDrawLine(const Vec3& start, const Vec3& end, bool depth = true, const Vec4& color = Vec4{1.0f, 1.0f, 1.0f, 1.0f}, f32 thickness = 0.025f, f32 lifetime_seconds = 0.0f);
void debugDrawCommands(const Slice<const DebugDrawCommand3D>& commands, bool depth = true);
void debugDrawTransform(const Transform& transform, bool depth = true, f32 thickness = 0.025f, f32 lifetime_seconds = 0.0f);
void debugDrawTransformFromMat(const Mat4& transform, bool depth = true, f32 thickness = 0.025f, f32 lifetime_seconds = 0.0f);
void debugDrawDisc(const Vec3& center, f32 radius, const Vec3& up, const Vec3& right, bool depth = true, const Vec4& color = Vec4{1.0f, 1.0f, 1.0f, 1.0f}, f32 thickness = 0.025f, f32 lifetime_seconds = 0.0f);
void debugDrawLineXZ(const Vec2& start, const Vec2& end, bool depth = true, const Vec4& color = Vec4{1.0f, 1.0f, 1.0f, 1.0f}, f32 thickness = 0.025f, f32 lifetime_seconds = 0.0f);
void debugDrawDiscXZ(const Vec2& center, f32 radius, bool depth = true, const Vec4& color = Vec4{1.0f, 1.0f, 1.0f, 1.0f}, f32 thickness = 0.025f, f32 lifetime_seconds = 0.0f);

void debugDrawLine2D(const Vec2& start, const Vec2& end, const LinearColor& color = LinearColor{1.0f, 1.0f, 1.0f, 1.0f}, f32 thickness = 4.0f);
void debugDrawRect2D(const Vec2& min, const Vec2& max, const LinearColor& color = LinearColor{1.0f, 1.0f, 1.0f, 1.0f}, f32 thickness = 4.0f);
void debugDrawText2D(const Vec2& position, const UTF8StringView& text, const LinearColor& color = LinearColor{1.0f, 1.0f, 1.0f, 1.0f});