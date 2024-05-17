#pragma once

#include <shared/std.h>
#include <shared/math.h>

#include "motion_matching.h"

struct Platform;
struct ArenaAllocator;
struct FrameAllocator;
struct PagePool;

struct GamepadInput
{
	Vec2 left_stick;
	Vec2 right_stick;
};

struct GameState
{
	void init(Platform& platform, ArenaAllocator& persistent_allocator, PagePool& page_pool, const AnimDB& anim_db);
	void deinit();
	void tick(Platform& platform, f32 delta_time, FrameAllocator& frame_allocator, const GamepadInput& gamepad, const AnimDB& anim_db);

	static constexpr s32 trajectory_past_point_count = 4;
	static constexpr s32 trajectory_future_point_count = 4;
	static constexpr s32 trajectory_total_point_count = trajectory_future_point_count;
	static constexpr s32 trajectory_future_write_index = 0;

	f32 time_since_last_past_write = 0.0f;

	Quat simulation_rotation;
	Vec3 simulation_position;
	Vec3 simulation_velocity;
	Vec3 simulation_acceleration;
	Vec3 simulation_angular_velocity;

	Vec3 trajectory_desired_velocities[trajectory_total_point_count];
	Quat trajectory_desired_rotations[trajectory_total_point_count];
	Vec3 trajectory_positions[trajectory_total_point_count];
	Vec3 trajectory_velocities[trajectory_total_point_count];
	Vec3 trajectory_accelerations[trajectory_total_point_count];
	Quat trajectory_rotations[trajectory_total_point_count];
	Vec3 trajectory_angular_velocities[trajectory_total_point_count];

	usize anim_query_start_frame;
	AnimDBFeatures anim_db_query;

	Vec3 entity_position;
	Quat entity_rotation;
	Vec3 entity_velocity;
	Vec3 entity_angular_velocity;

	usize entity_anim_index_start;
	usize entity_anim_pose_index;
	Vec3 entity_anim_start_position;
	Quat entity_anim_start_rotation;
	bool entity_animating = true;

	usize anim_db_debug_index = 0;

	Slice<AnimDBJoint> pose_offset;
	AnimDBJoint root_motion_offset;
	Vec3 transition_src_position;
	Quat transition_src_rotation;
	Vec3 transition_dst_position;
	Quat transition_dst_rotation;

	f32 blending_halflife;

	// usize current_anim_index;
	// usize current_anim_index_start = 0;
};