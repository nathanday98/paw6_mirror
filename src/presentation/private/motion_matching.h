#pragma once

#include <shared/std.h>
#include <shared/math.h>
#include <shared/slice.h>

struct GameState;
struct FrameAllocator;

enum MotionMatchFeature
{
	MotionMatchFeature_TrajectoryPosition,
	MotionMatchFeature_TrajectoryDirection,
	MotionMatchFeature_LeftFootPosition,
	MotionMatchFeature_LeftFootVelocity,
	MotionMatchFeature_RightFootPosition,
	MotionMatchFeature_RightFootVelocity,
	MotionMatchFeature_HipVelocity,
	MotionMatchFeature_Count,
};

extern usize g_motion_match_feature_sizes[MotionMatchFeature_Count];
extern usize g_motion_match_feature_offsets[MotionMatchFeature_Count];
extern f32 g_motion_match_feature_weights[MotionMatchFeature_Count];

PAW_ERROR_ON_PADDING_BEGIN

union AnimDBFeatures
{
	static constexpr usize trajectory_point_count = 3;
	struct Data
	{
		Vec2 trajectory_positions[trajectory_point_count];
		Vec2 trajectory_directions[trajectory_point_count];
		Vec3 left_foot_position;
		Vec3 left_foot_velocity;
		Vec3 right_foot_position;
		Vec3 right_foot_velocity;
		Vec3 hip_velocity;
	} data;

	static constexpr usize f32_count = sizeof(Data) / 4;

	f32 raw_data[f32_count];

	static_assert(sizeof(Data) % 4 == 0);
};

struct AnimDBJoint
{
	Vec3 position;
	Quat rotation;
	Vec3 velocity;
	Vec3 angular_velocity;
};

PAW_ERROR_ON_PADDING_END

struct AnimDB
{
	Slice<AnimDBFeatures> feature_data;
	Slice<AnimDBJoint> poses;
	Slice<AnimDBJoint> root_motion;
	usize joints_per_pose;

	Slice<s32> parent_indices;

	Mat4 temp_instance_transform;

	f32 feature_norm_offsets[AnimDBFeatures::f32_count];
	f32 feature_norm_scales[AnimDBFeatures::f32_count];

	usize left_foot_index;
	usize right_foot_index;
	usize hip_index;

	usize query(const AnimDBFeatures& denormalized_features) const;

	void debugQuery(const GameState& game_state, FrameAllocator& frame_allocator);
};