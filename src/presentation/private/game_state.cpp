#include "game_state.h"

#include <shared/slice.h>
#include <shared/assert.h>
#include <shared/log.h>
#include <shared/math.inl>
#include <shared/optional.h>
#include <shared/memory.h>
#include <shared/hash.h>

#include <platform/platform.h>

#include "input.h"
#include "job_graph.h"
#include "allocators/frame_allocator.h"
#include "allocators/arena_allocator.h"
#include "profiler.h"
#include "debug_draw.h"
#include "transform.h"
#include "assets.h"
#include "frame_times.h"
#include "frame_times.h"
#include "allocators/page_pool.h"
#include "new_renderer.h"
#include "imgui_wrapper.h"
#include "allocators/frame_allocator.h"

static constexpr f32 g_anim_blend_time_seconds = 0.2f;

void GameState::init(Platform& /*platform*/, ArenaAllocator& persistent_allocator, PagePool& /*page_pool*/, const AnimDB& anim_db)
{
	PAW_PROFILER_FUNC();
	Assets::loadAll();

	pose_offset = persistent_allocator.alloc<AnimDBJoint>(anim_db.joints_per_pose);
	for (usize i = 0; i < pose_offset.size; i++)
	{
		pose_offset[i] = AnimDBJoint{};
	}

	blending_halflife = 0.1f;
}

void GameState::deinit()
{
}

#define LN2f 0.69314718056f

static inline float fast_negexpf(float x)
{
	return 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
}

static inline f32 halflife_to_damping(f32 halflife, f32 eps = 1e-5f)
{
	return (4.0f * LN2f) / (halflife + eps);
}

static inline void simple_spring_damper_exact(float& x, float& v, const float x_goal, const float halflife, const float dt)
{
	float y = halflife_to_damping(halflife) / 2.0f;
	float j0 = x - x_goal;
	float j1 = v + j0 * y;
	float eydt = fast_negexpf(y * dt);

	x = eydt * (j0 + j1 * dt) + x_goal;
	v = eydt * (v - j1 * y * dt);
}

static inline void simple_spring_damper_exact(Quat& x, Vec3& v, const Quat x_goal, const f32 halflife, const f32 dt)
{
	const f32 y = halflife_to_damping(halflife) / 2.0f;
	const Vec3 j0 = toScaledAngleAxis(abs(x * inverse(x_goal)));
	const Vec3 j1 = v + j0 * y;

	const f32 eydt = fast_negexpf(y * dt);

	x = fromScaledAngleAxis(eydt * (j0 + j1 * dt)) * x_goal;
	v = eydt * (v - j1 * y * dt);
}

static void simulationPositionsUpdate(Vec3& position, Vec3& velocity, Vec3& acceleration, Vec3 desired_velocity, f32 halflife, f32 delta_time_seconds)
{
	f32 y = halflife_to_damping(halflife) / 2.0f;
	Vec3 j0 = velocity - desired_velocity;
	Vec3 j1 = acceleration + j0 * y;
	f32 eydt = fast_negexpf(y * delta_time_seconds);

	position = eydt * (((-j1) / (y * y)) + ((-j0 - j1 * delta_time_seconds) / y)) +
		(j1 / (y * y)) + j0 / y + desired_velocity * delta_time_seconds + position;
	velocity = eydt * (j0 + j1 * delta_time_seconds) + desired_velocity;
	acceleration = eydt * (acceleration - j1 * y * delta_time_seconds);
}

static void simulationRotationsUpdate(Quat& rotation, Vec3& angular_velocity, Quat desired_rotation, f32 halflife, f32 delta_time_seconds)
{
	simple_spring_damper_exact(rotation, angular_velocity, desired_rotation, halflife, delta_time_seconds);
}

static Vec3 desiredVelocityUpdate(Vec3 gamepad_left, f32 camera_azimuth, Quat simulation_rotation, f32 forward_speed, f32 side_speed, f32 back_speed)
{
	// Find stick position in world space by rotating using camera azimuth
	const Vec3 global_stick_direction = rotateVec3(fromAngleAxis(math_world_up, camera_azimuth), gamepad_left);

	// Find stick position local to current facing direction
	const Vec3 local_stick_direction = rotateVec3(inverse(simulation_rotation), global_stick_direction);

	// Scale stick by forward, sideways and backwards speeds
	const Vec3 local_desired_velocity = local_stick_direction.z < 0.0f ? Vec3{side_speed, 0.0f, forward_speed} * local_stick_direction : Vec3{side_speed, 0.0f, back_speed} * local_stick_direction;

	const Vec3 result = rotateVec3(simulation_rotation, local_desired_velocity);
	return result;
}

static Quat desiredRotationUpdate(Quat desired_rotation, Vec3 gamepad_left, Vec3 desired_velocity)
{
	if (length(gamepad_left) > 0.01f)
	{
		const Vec3 desired_direction = normalizeSafely(desired_velocity);
		// const f32 rotation = angleBetween(fromAngleAxis(desired_direction, 0.0f), fromAngleAxis(math_world_up, 0.0f));
		const f32 rotation = math_atan2(-desired_direction.z, -desired_direction.x);
		return fromAngleAxis(math_world_up, rotation);
		// return fromAngleAxis(desired_direction, 0.0f);
	}
	else
	{
		return desired_rotation;
	}
}

static void drawAnimTrajectory(const AnimDBFeatures& denormalized_features, Vec3 draw_position, Quat draw_rotation, const Vec4& color)
{
	for (usize i = 0; i < AnimDBFeatures::trajectory_point_count; i++)
	{
		const Vec2 trajectory_position = denormalized_features.data.trajectory_positions[i];

		const Vec2& trajectory_direction = denormalized_features.data.trajectory_directions[i];

		const Vec3& position = draw_position + rotateVec3(draw_rotation, fromXZ(trajectory_position));
		const Vec3& direction = rotateVec3(draw_rotation, fromXZ(trajectory_direction)) * 0.1f;

		const f32 arrow_t = 0.1f;

		if (i < AnimDBFeatures::trajectory_point_count - 1)
		{
			const Vec2 next_trajectory_position = denormalized_features.data.trajectory_positions[i + 1];

			const Vec3& next_position = draw_position + rotateVec3(draw_rotation, fromXZ(next_trajectory_position));

			const Vec3 pos_dir = next_position - position;

			debugDrawCommands(
				{
					DebugDrawCommand3D{
						.color = color,
						.start = position,
						.end = position + pos_dir * (1.0f - arrow_t),
						.start_thickness = 0.005f,
						.end_thickness = 0.005f,
					},
					DebugDrawCommand3D{
						.color = color,
						.start = position + pos_dir * (1.0f - arrow_t),
						.end = next_position,
						.start_thickness = 0.02f,
						.end_thickness = 0.0f,
					},
				});
		}

		debugDrawCommands({

			DebugDrawCommand3D{
				.color = Vec4{1.0f, 0.0f, 0.0f, 1.0f},
				.start = position,
				.end = position + direction * (1.0f - arrow_t),
				.start_thickness = 0.005f,
				.end_thickness = 0.005f,
			},
			DebugDrawCommand3D{
				.color = Vec4{1.0f, 0.0f, 0.0f, 1.0f},
				.start = position + direction * (1.0f - arrow_t),
				.end = position + direction,
				.start_thickness = 0.02f,
				.end_thickness = 0.0f,
			}});
	}
}

static void drawAnimPose(const AnimDB& anim_db, usize pose_index, const AnimDBFeatures& denormalized_row, Vec3 draw_position, Quat draw_rotation, FrameAllocator& frame_allocator, bool draw_velocities = false, Vec4 color = Vec4{1.0f, 1.0f, 1.0f, 1.0f})
{
	Slice<Mat4> joint_world_mats = frame_allocator.alloc<Mat4>(anim_db.joints_per_pose + 1);
	const Slice<const AnimDBJoint> pose = anim_db.poses.const_sub_slice(pose_index * anim_db.joints_per_pose, anim_db.joints_per_pose);
	// const AnimDBJoint& sim_joint = pose[0];

	// const Quat inverse_draw_rotation = inverse(draw_rotation);

	const Transform sim_transform{
		//.rotation = inverse(pose[0].rotation),
		//.translation = -pose[0].position,
		//.translation = draw_position - fromXZ(anim_db.debug_sim_bones[pose_index].position),
		//.scale = 0.01f,
		.rotation = draw_rotation,
		.translation = draw_position,
	};

	// const Quat inverse_sim_rotation = inverse(sim_joint.rotation);

	const Mat4 sim_transform_mat = toMat4(sim_transform);
	joint_world_mats[0] = sim_transform_mat;
	for (usize joint_index = 0; joint_index < pose.size; ++joint_index)
	{
		const AnimDBJoint& joint = pose[joint_index];
		const Transform local_transform{
			.rotation = joint.rotation,
			.translation = joint.position,
		};

		const s32 parent_index = anim_db.parent_indices[joint_index];
		Mat4 parent_world_mat = joint_world_mats[parent_index + 1];
		;
		// if (parent_index > -1)
		{
			// parent_world_mat = joint_world_mats[parent_index];
		}

		Mat4 world_mat = parent_world_mat * toMat4(local_transform);
		// debugDrawTransformFromMat(parent_world_mat, true, 0.001f);
		joint_world_mats[joint_index + 1] = world_mat;

		const Vec3 world_pos = world_mat.transformPoint({});
		debugDrawLine(world_pos, parent_world_mat.transformPoint({}), true, color, 0.005f);

		if (draw_velocities && joint_index == 0)
		{
			debugDrawLine(world_pos, world_pos + denormalized_row.data.hip_velocity);
		}
	}

	const Vec3 left_foot = rotateVec3(draw_rotation, denormalized_row.data.left_foot_position) + draw_position;
	const Vec3 right_foot = rotateVec3(draw_rotation, denormalized_row.data.right_foot_position) + draw_position;

	if (draw_velocities)
	{
		debugDrawLine(left_foot, left_foot + denormalized_row.data.left_foot_velocity);
		debugDrawLine(right_foot, right_foot + denormalized_row.data.left_foot_velocity);
	}

	debugDrawLine(left_foot - Vec3{0.0f, 0.05f, 0.0f}, left_foot + Vec3{0.0f, 0.05f, 0.0f}, true, Vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.1f);
	debugDrawLine(right_foot - Vec3{0.0f, 0.05f, 0.0f}, right_foot + Vec3{0.0f, 0.05f, 0.0f}, true, Vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.1f);
}

static void drawAnimPose(const AnimDB& anim_db, const Slice<const AnimDBJoint>& pose, Vec3 draw_position, Quat draw_rotation, FrameAllocator& frame_allocator, Vec4 color = Vec4{1.0f, 1.0f, 1.0f, 1.0f})
{
	Slice<Mat4> joint_world_mats = frame_allocator.alloc<Mat4>(anim_db.joints_per_pose + 1);
	// const AnimDBJoint& sim_joint = pose[0];

	// const Quat inverse_draw_rotation = inverse(draw_rotation);

	const Transform sim_transform{
		//.rotation = inverse(pose[0].rotation),
		//.translation = -pose[0].position,
		//.translation = draw_position - fromXZ(anim_db.debug_sim_bones[pose_index].position),
		//.scale = 0.01f,
		.rotation = draw_rotation,
		.translation = draw_position,
	};

	// const Quat inverse_sim_rotation = inverse(sim_joint.rotation);

	const Mat4 sim_transform_mat = toMat4(sim_transform);
	joint_world_mats[0] = sim_transform_mat;
	for (usize joint_index = 0; joint_index < pose.size; ++joint_index)
	{
		const AnimDBJoint& joint = pose[joint_index];
		const Transform local_transform{
			.rotation = joint.rotation,
			.translation = joint.position,
		};

		const s32 parent_index = anim_db.parent_indices[joint_index];
		Mat4 parent_world_mat = joint_world_mats[parent_index + 1];
		;
		// if (parent_index > -1)
		{
			// parent_world_mat = joint_world_mats[parent_index];
		}

		Mat4 world_mat = parent_world_mat * toMat4(local_transform);
		// debugDrawTransformFromMat(parent_world_mat, true, 0.001f);
		joint_world_mats[joint_index + 1] = world_mat;

		const Vec3 world_pos = world_mat.transformPoint({});
		debugDrawLine(world_pos, parent_world_mat.transformPoint({}), true, color, 0.005f);
	}

	const Vec3 left_foot = joint_world_mats[anim_db.left_foot_index + 1].transformPoint({});
	const Vec3 right_foot = joint_world_mats[anim_db.right_foot_index + 1].transformPoint({});

	debugDrawLine(left_foot - Vec3{0.0f, 0.05f, 0.0f}, left_foot + Vec3{0.0f, 0.05f, 0.0f}, true, Vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.1f);
	debugDrawLine(right_foot - Vec3{0.0f, 0.05f, 0.0f}, right_foot + Vec3{0.0f, 0.05f, 0.0f}, true, Vec4{1.0f, 1.0f, 0.0f, 1.0f}, 0.1f);
}

static inline void decay_spring_damper_exact(
	Vec3& x,
	Vec3& v,
	const f32 halflife,
	const f32 dt)
{
	f32 y = halflife_to_damping(halflife) / 2.0f;
	Vec3 j1 = v + x * y;
	f32 eydt = fast_negexpf(y * dt);

	x = eydt * (x + j1 * dt);
	v = eydt * (v - j1 * y * dt);
}

static inline void decay_spring_damper_exact(
	Quat& x,
	Vec3& v,
	const float halflife,
	const float dt)
{
	float y = halflife_to_damping(halflife) / 2.0f;

	Vec3 j0 = toScaledAngleAxis(x);
	Vec3 j1 = v + j0 * y;

	float eydt = fast_negexpf(y * dt);

	x = fromScaledAngleAxis(eydt * (j0 + j1 * dt));
	v = eydt * (v - j1 * y * dt);
}

void GameState::tick(Platform& /*platform*/, f32 delta_time, FrameAllocator& frame_allocator, const GamepadInput& gamepad, const AnimDB& anim_db)
{
	PAW_PROFILER_FUNC();
	const f32 grid_size = 50.0f;
	const f32 half_grid_size = grid_size * 0.5f;
	for (f32 i = 0; i <= grid_size; i++)
	{
		const Vec2 start_postion = {
			.x = i - half_grid_size,
			.y = -half_grid_size,
		};

		const Vec2 end_postion = start_postion + Vec2{0.0f, grid_size};
		debugDrawLineXZ(start_postion, end_postion, true, Vec4{0.5f, 0.5f, 0.5f, 0.5f}, 0.01f);
	}

	for (f32 i = 0; i <= grid_size; i++)
	{
		const Vec2 start_postion = {
			.x = -half_grid_size,
			.y = i - half_grid_size,
		};

		const Vec2 end_postion = start_postion + Vec2{grid_size, 0.0f};
		debugDrawLineXZ(start_postion, end_postion, true, Vec4{0.5f, 0.5f, 0.5f, 0.5f}, 0.01f);
	}

	debugDrawTransform({});

	const f32 forward_speed = 1.75f;
	const f32 side_speed = 1.5f;
	const f32 back_speed = 1.25f;

	const f32 simulation_rotation_halflife = 0.3f;
	const f32 simulation_velocity_halflife = 0.3f;

	const f32 camera_azimuth = 0.0f;

	const Vec3 left = fromXZ(gamepad.left_stick) * Vec3{1.0f, 0.0f, -1.0f};
	/*(void)gamepad;
	const Vec3 left = math_world_forward;*/

	const Vec3 desired_velocity_current = desiredVelocityUpdate(left, camera_azimuth, simulation_rotation, forward_speed, side_speed, back_speed);

	const Quat desired_rotation_current = desiredRotationUpdate(simulation_rotation, left, desired_velocity_current);

	const f32 trajectory_step = 20.0f * delta_time;

	// const usize next_anim_index = math_minUsize(current_anim_index + 1, anim_db.feature_data.size - 1);

	// Vec3 blend_root_motion_position_delta;
	// Quat blend_root_motion_rotation_delta;

	const bool blending = true;

	if (blending)
	{

		// const AnimDBJoint& start_root_motion = anim_db.root_motion[outgoing_blend.start_pose_index];
		// const AnimDBJoint& next_root_motion = anim_db.root_motion[outgoing_blend.current_pose_index];
		// const Quat inverse_start_rotation = inverse(start_root_motion.rotation);

		//// Local to animation
		// const Vec3 next_position = rotateVec3(outgoing_blend.anim_start_rotation, rotateVec3(inverse_start_rotation, next_root_motion.position - start_root_motion.position));
		// const Quat next_rotation = inverse_start_rotation * next_root_motion.rotation;

		// const Vec3 prev_position = rotateVec3(outgoing_blend.anim_start_rotation, rotateVec3(inverse_start_rotation, prev_root_motion.position - start_root_motion.position));
		// const Quat prev_rotation = inverse_start_rotation * prev_root_motion.rotation;

		// blend_root_motion_position_delta = next_position - prev_position;
		// blend_root_motion_rotation_delta = inverse(prev_rotation) * next_rotation;
	}

	// Vec3 root_motion_position_delta;
	// Quat root_motion_rotation_delta;

	//{
	//	const AnimDBJoint& prev_root_motion = anim_db.root_motion[entity_anim_pose_index];

	entity_anim_pose_index = math_minUsize(entity_anim_pose_index + 1, anim_db.feature_data.size - 1);

	//	const AnimDBJoint& start_root_motion = anim_db.root_motion[entity_anim_index_start];
	//	const AnimDBJoint& next_root_motion = anim_db.root_motion[entity_anim_pose_index];
	//	const Quat inverse_start_rotation = inverse(start_root_motion.rotation);

	//	// Local to animation
	//	const Vec3 next_position = rotateVec3(entity_anim_start_rotation, rotateVec3(inverse_start_rotation, next_root_motion.position - start_root_motion.position));
	//	const Quat next_rotation = inverse_start_rotation * next_root_motion.rotation;

	//	const Vec3 prev_position = rotateVec3(entity_anim_start_rotation, rotateVec3(inverse_start_rotation, prev_root_motion.position - start_root_motion.position));
	//	const Quat prev_rotation = inverse_start_rotation * prev_root_motion.rotation;

	//	root_motion_position_delta = next_position - prev_position;
	//	root_motion_rotation_delta = inverse(prev_rotation) * next_rotation;
	//}

	// if (blending)
	//{
	//	// entity_position += slerp(blend_root_motion_position_delta, root_motion_position_delta, blend_amount);
	//	// entity_rotation *= slerp(blend_root_motion_rotation_delta, root_motion_rotation_delta, blend_amount);

	//	entity_position += root_motion_position_delta;
	//	entity_rotation *= root_motion_rotation_delta;
	//}
	// else
	//{
	//	entity_position += root_motion_position_delta;
	//	entity_rotation *= root_motion_rotation_delta;
	//}

	const Slice<Quat> global_rotations = frame_allocator.alloc<Quat>(anim_db.joints_per_pose);
	const Slice<Vec3> global_positions = frame_allocator.alloc<Vec3>(anim_db.joints_per_pose);
	const Slice<Vec3> global_velocities = frame_allocator.alloc<Vec3>(anim_db.joints_per_pose);
	const Slice<Vec3> global_angular_velocities = frame_allocator.alloc<Vec3>(anim_db.joints_per_pose);
	const Slice<const AnimDBJoint> current_pose = anim_db.poses.const_sub_slice(anim_db.joints_per_pose * entity_anim_pose_index, anim_db.joints_per_pose);
	const Slice<AnimDBJoint> blended_pose = frame_allocator.alloc<AnimDBJoint>(anim_db.joints_per_pose);

	if (blending)
	{
		for (usize joint_index = 0; joint_index < anim_db.joints_per_pose; ++joint_index)
		{
			const AnimDBJoint& end_joint = current_pose[joint_index];
			AnimDBJoint& offset = pose_offset[joint_index];

			AnimDBJoint& out_joint = blended_pose[joint_index];

			decay_spring_damper_exact(offset.position, offset.velocity, blending_halflife, delta_time);
			out_joint.position = end_joint.position + offset.position;
			out_joint.velocity = end_joint.velocity + offset.velocity;

			decay_spring_damper_exact(offset.rotation, offset.angular_velocity, blending_halflife, delta_time);
			out_joint.rotation = offset.rotation * end_joint.rotation;
			out_joint.angular_velocity = offset.angular_velocity + rotateVec3(offset.rotation, end_joint.angular_velocity);
		}

		const AnimDBJoint& current_root_motion = anim_db.root_motion[entity_anim_pose_index];

		const Vec3 world_space_position = rotateVec3(transition_dst_rotation, rotateVec3(inverse(transition_src_rotation), current_root_motion.position - transition_src_position)) + transition_dst_position;
		const Vec3 world_space_velocity = rotateVec3(transition_dst_rotation, rotateVec3(inverse(transition_src_rotation), current_root_motion.velocity));

		const Quat world_space_rotation = normalize(transition_dst_rotation * (inverse(transition_src_rotation) * current_root_motion.rotation));
		const Vec3 world_space_angular_velocity = rotateVec3(transition_dst_rotation, rotateVec3(inverse(transition_src_rotation), current_root_motion.angular_velocity));

		{
			decay_spring_damper_exact(root_motion_offset.position, root_motion_offset.velocity, blending_halflife, delta_time);
			entity_position = world_space_position + root_motion_offset.position;
			entity_velocity = world_space_velocity + root_motion_offset.velocity;

			decay_spring_damper_exact(root_motion_offset.rotation, root_motion_offset.angular_velocity, blending_halflife, delta_time);
			entity_rotation = root_motion_offset.rotation * world_space_rotation;
			entity_angular_velocity = root_motion_offset.angular_velocity + rotateVec3(root_motion_offset.rotation, world_space_angular_velocity);
		}
	}
	else
	{
		for (usize joint_index = 0; joint_index < anim_db.joints_per_pose; ++joint_index)
		{
			blended_pose[joint_index] = current_pose[joint_index];
		}
	}

	for (usize joint_index = 0; joint_index < anim_db.joints_per_pose; ++joint_index)
	{
		const AnimDBJoint& joint = blended_pose[joint_index];

		const Vec3 local_position = joint.position;
		const Quat local_rotation = joint.rotation;
		const Vec3 local_velocity = joint.velocity;
		const Vec3 local_angular_velocity = joint.angular_velocity;

		const s32 parent_index = anim_db.parent_indices[joint_index];
		Vec3 global_position;
		Quat global_rotation;
		Vec3 global_velocity;
		Vec3 global_angular_velocity;
		if (parent_index > -1)
		{
			const Vec3& parent_position = global_positions[parent_index];
			const Quat& parent_rotation = global_rotations[parent_index];
			const Vec3& parent_velocity = global_velocities[parent_index];
			const Vec3& parent_angular_velocity = global_angular_velocities[parent_index];

			global_position = rotateVec3(parent_rotation, local_position) + parent_position;
			global_rotation = parent_rotation * local_rotation;
			global_velocity = parent_velocity + rotateVec3(parent_rotation, local_velocity) + cross(parent_angular_velocity, rotateVec3(parent_rotation, local_position));
			global_angular_velocity = rotateVec3(parent_rotation, local_angular_velocity) + parent_angular_velocity;
		}
		else
		{
			global_position = local_position;
			global_rotation = local_rotation;
			global_velocity = local_velocity;
			global_angular_velocity = local_angular_velocity;
		}

		global_positions[joint_index] = global_position;
		global_rotations[joint_index] = global_rotation;
		global_velocities[joint_index] = global_velocity;
		global_angular_velocities[joint_index] = global_angular_velocity;
	}

	// entity_rotation *= inverse(start_pose[0].rotation) * next_pose[0].rotation;
	// entity_position += rotateVec3(entity_rotation, rotateVec3(inverse(next_pose[0].rotation), next_pose[0].velocity)) * delta_time;

	{
		AnimDBFeatures row = anim_db.feature_data[entity_anim_index_start];
		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			row.raw_data[i] = (row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
		}
		drawAnimTrajectory(row, entity_anim_start_position, entity_anim_start_rotation, Vec4{0.0f, 0.0f, 1.0f, 1.0f});
	}

	if (!entity_animating)
	{
		AnimDBFeatures row = anim_db.feature_data[anim_db_debug_index];
		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			row.raw_data[i] = (row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
		}
		drawAnimTrajectory(row, entity_position, entity_rotation, Vec4{0.0f, 1.0f, 0.0f, 1.0f});
	}

	/*entity_position = simulation_position;
	entity_rotation = simulation_rotation;*/
	// current_anim_index = next_anim_index;

	// time_since_last_past_write += delta_time_seconds;

	// if (time_since_last_past_write >= trajectory_step)
	//{
	//	for (s32 i = 0; i < trajectory_past_point_count; i++)
	//	{
	//		trajectory_positions[i] = trajectory_positions[i + 1];
	//		trajectory_rotations[i] = trajectory_rotations[i + 1];
	//	}
	//	time_since_last_past_write = 0.0f;
	// }

	// Predict future trajectory

	// Predict desired rotations
	trajectory_desired_rotations[trajectory_future_write_index] = desired_rotation_current;
	for (s32 i = trajectory_future_write_index + 1; i < trajectory_total_point_count; i++)
	{
		trajectory_desired_rotations[i] = desiredRotationUpdate(trajectory_desired_rotations[i - 1], left, trajectory_desired_velocities[i]);
	}

	// Predict rotations
	trajectory_rotations[trajectory_future_write_index] = simulation_rotation;
	trajectory_angular_velocities[trajectory_future_write_index] = simulation_angular_velocity;

	for (s32 i = 1; i < trajectory_future_point_count; i++)
	{
		trajectory_rotations[i + trajectory_future_write_index] = simulation_rotation;
		trajectory_angular_velocities[i + trajectory_future_write_index] = simulation_angular_velocity;
		simulationRotationsUpdate(trajectory_rotations[i + trajectory_future_write_index], trajectory_angular_velocities[i + trajectory_future_write_index], trajectory_desired_rotations[i + trajectory_future_write_index], simulation_rotation_halflife, i * trajectory_step);
	}

	// Predict desired velocities
	trajectory_desired_velocities[trajectory_future_write_index] = desired_velocity_current;
	for (s32 i = trajectory_future_write_index + 1; i < trajectory_total_point_count; i++)
	{
		trajectory_desired_velocities[i] = desiredVelocityUpdate(left, camera_azimuth, trajectory_rotations[i], forward_speed, side_speed, back_speed);
	}

	// Predict positions
	trajectory_positions[trajectory_future_write_index] = simulation_position;
	trajectory_velocities[trajectory_future_write_index] = simulation_velocity;
	trajectory_accelerations[trajectory_future_write_index] = simulation_acceleration;
	for (s32 i = trajectory_future_write_index + 1; i < trajectory_total_point_count; i++)
	{
		trajectory_positions[i] = trajectory_positions[i - 1];
		trajectory_velocities[i] = trajectory_velocities[i - 1];
		trajectory_accelerations[i] = trajectory_accelerations[i - 1];

		simulationPositionsUpdate(trajectory_positions[i], trajectory_velocities[i], trajectory_accelerations[i], trajectory_desired_velocities[i], simulation_velocity_halflife, trajectory_step);
	}

	simulationPositionsUpdate(simulation_position, simulation_velocity, simulation_acceleration, desired_velocity_current, simulation_velocity_halflife, delta_time);

	simulationRotationsUpdate(simulation_rotation, simulation_angular_velocity, desired_rotation_current, simulation_rotation_halflife, delta_time);

	debugDrawDiscXZ(xz(simulation_position), 0.25f, true, Vec4{1.0f, 0.0f, 0.0f, 1.0f});

	AnimDBFeatures current_anim_row = anim_db.feature_data[entity_anim_pose_index];
	// Denormalize row
	for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
	{
		current_anim_row.raw_data[i] = (current_anim_row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
	}

	anim_query_start_frame = entity_anim_pose_index;

	anim_db_query.data.left_foot_position = current_anim_row.data.left_foot_position;
	anim_db_query.data.left_foot_velocity = current_anim_row.data.left_foot_velocity;
	anim_db_query.data.right_foot_position = current_anim_row.data.right_foot_position;
	anim_db_query.data.right_foot_velocity = current_anim_row.data.right_foot_velocity;
	anim_db_query.data.hip_velocity = current_anim_row.data.hip_velocity;

	const Vec3& root_position = entity_position;
	const Quat& root_rotation = entity_rotation;
	const Vec3& root_velocity = entity_velocity;
	const Vec3& root_angular_velocity = entity_angular_velocity;

	const Quat inverse_root_rotation = inverse(root_rotation);

	for (usize i = 0; i < AnimDBFeatures::trajectory_point_count; i++)
	{
		anim_db_query.data.trajectory_positions[i] = xz(rotateVec3(inverse_root_rotation, trajectory_positions[i + 1] - root_position));
		anim_db_query.data.trajectory_directions[i] = xz(rotateVec3(inverse_root_rotation, rotateVec3(trajectory_rotations[i + 1], math_world_forward)));
	}

	drawAnimTrajectory(anim_db_query, entity_position, entity_rotation, Vec4{0.0f, 1.0f, 0.0f, 1.0f});

	const usize query_result_index = anim_db.query(anim_db_query);
	const usize query_epsilon = 12;
	if (query_result_index < entity_anim_pose_index - query_epsilon || query_result_index > entity_anim_pose_index + query_epsilon)
	{
		const Slice<const AnimDBJoint> start_pose = anim_db.poses.const_sub_slice(anim_db.joints_per_pose * entity_anim_pose_index, anim_db.joints_per_pose);
		const Slice<const AnimDBJoint> end_pose = anim_db.poses.const_sub_slice(anim_db.joints_per_pose * query_result_index, anim_db.joints_per_pose);

		for (usize i = 0; i < pose_offset.size; ++i)
		{
			pose_offset[i].position = (start_pose[i].position + pose_offset[i].position) - end_pose[i].position;
			pose_offset[i].velocity = (start_pose[i].velocity + pose_offset[i].velocity) - end_pose[i].velocity;
			pose_offset[i].angular_velocity = (start_pose[i].angular_velocity + pose_offset[i].angular_velocity) - end_pose[i].angular_velocity;
			pose_offset[i].rotation = abs((pose_offset[i].rotation * start_pose[i].rotation) * inverse(end_pose[i].rotation));
		}

		const AnimDBJoint& end_root_motion = anim_db.root_motion[query_result_index];

		transition_dst_position = root_position;
		transition_dst_rotation = root_rotation;
		transition_src_position = end_root_motion.position;
		transition_src_rotation = end_root_motion.rotation;

		const Vec3 world_space_dst_velocity = rotateVec3(transition_dst_rotation, rotateVec3(inverse(transition_src_rotation), end_root_motion.velocity));
		const Vec3 world_space_dst_angular_velocity = rotateVec3(transition_dst_rotation, rotateVec3(inverse(transition_src_rotation), end_root_motion.angular_velocity));

		{
			root_motion_offset.position = (root_position + root_motion_offset.position) - root_position;
			root_motion_offset.velocity = (root_velocity + root_motion_offset.velocity) - world_space_dst_velocity;

			root_motion_offset.angular_velocity = (root_angular_velocity + root_motion_offset.angular_velocity) - world_space_dst_angular_velocity;
			root_motion_offset.rotation = abs((root_motion_offset.rotation * root_rotation) * inverse(root_rotation));
		}

		entity_anim_index_start = query_result_index;
		entity_anim_pose_index = query_result_index;
		entity_anim_start_position = entity_position;
		entity_anim_start_rotation = entity_rotation;
	}

	AnimDBFeatures winning_anim_row = anim_db.feature_data[entity_anim_pose_index];
	// Denormalize row
	for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
	{
		winning_anim_row.raw_data[i] = (winning_anim_row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
	}

	AnimDBFeatures debug_anim_row = anim_db.feature_data[anim_db_debug_index];
	// Denormalize row
	for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
	{
		debug_anim_row.raw_data[i] = (debug_anim_row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
	}

	const Slice<const AnimDBJoint> winning_pose = anim_db.poses.const_sub_slice(entity_anim_pose_index * anim_db.joints_per_pose, anim_db.joints_per_pose);

	debugDrawDiscXZ(xz(entity_position), 0.25f, true, Vec4{0.0f, 1.0f, 0.0f, 1.0f});

	// drawAnimPose(anim_db, entity_anim_pose_index, winning_anim_row, entity_position, entity_rotation, frame_allocator);
	drawAnimPose(anim_db, blended_pose.to_const_slice(), entity_position, entity_rotation, frame_allocator, blending ? Vec4{1.0f, 0.0f, 0.0f, 1.0f} : Vec4{1.0f, 1.0f, 1.0f, 1.0f});

	{
		const AnimDBJoint& debug_sim_joint = anim_db.root_motion[anim_db_debug_index];
		drawAnimPose(anim_db, anim_db_debug_index, debug_anim_row, debug_sim_joint.position, debug_sim_joint.rotation, frame_allocator, false);
		drawAnimTrajectory(debug_anim_row, debug_sim_joint.position, debug_sim_joint.rotation, Vec4{0.0f, 0.0f, 1.0f, 1.0f});
	}

#ifdef PAW_ENABLE_IMGUI

	ImGui::Begin("Anim DB Slider");
	const usize slider_min = 0;
	const usize slider_max = anim_db.feature_data.size - 1;
	ImGui::Text("Anim Debug Frame");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::SliderScalar("##AnimDebugFrame", ImGuiDataType_U64, &anim_db_debug_index, &slider_min, &slider_max);
	ImGui::Separator();
	{
		static usize blend_start_index = 1300;
		static usize blend_end_index = 1443;

		ImGui::Text("Blend Start Frame");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderScalar("##BlendStartFrame", ImGuiDataType_U64, &blend_start_index, &slider_min, &slider_max);

		ImGui::Text("Blend End Frame");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::SliderScalar("##BlendEndFrame", ImGuiDataType_U64, &blend_end_index, &slider_min, &slider_max);

		const Vec4 start_color = Vec4{1.0f, 0.0f, 0.0f, 1.0f};
		const Vec4 end_color = Vec4{0.0f, 0.0f, 1.0f, 1.0f};

		AnimDBFeatures start_anim_row = anim_db.feature_data[blend_start_index];
		// Denormalize row
		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			start_anim_row.raw_data[i] = (start_anim_row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
		}

		const Vec3 start_position{5.0f, 0.0f, 0.0f};
		const Vec3 end_position{9.0f, 0.0f, 0.0f};
		const Quat start_rotation{};
		const Quat end_rotation{};

		// const AnimDBJoint& blend_start_root_motion = anim_db.root_motion[blend_start_index];
		drawAnimPose(anim_db, blend_start_index, start_anim_row, start_position, start_rotation, frame_allocator, false, start_color);
		drawAnimTrajectory(start_anim_row, start_position, start_rotation, start_color);

		AnimDBFeatures end_anim_row = anim_db.feature_data[blend_end_index];
		// Denormalize row
		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			end_anim_row.raw_data[i] = (end_anim_row.raw_data[i] * anim_db.feature_norm_scales[i]) + anim_db.feature_norm_offsets[i];
		}

		// const AnimDBJoint& blend_end_root_motion = anim_db.root_motion[blend_end_index];
		drawAnimPose(anim_db, blend_end_index, end_anim_row, end_position, end_rotation, frame_allocator, false, end_color);
		drawAnimTrajectory(end_anim_row, end_position, end_rotation, end_color);

		const Slice<const AnimDBJoint> start_pose = anim_db.poses.const_sub_slice(anim_db.joints_per_pose * blend_start_index, anim_db.joints_per_pose);
		const Slice<const AnimDBJoint> end_pose = anim_db.poses.const_sub_slice(anim_db.joints_per_pose * blend_end_index, anim_db.joints_per_pose);

		const Vec3 pose_position{7.0f, 0.0f, 0.0f};
		const Quat pose_rotation{};
		const Vec4 blend_color{1.0f, 0.0f, 1.0f, 1.0f};

		static f32 blend_delta_time_frames = 0.0f;

		const f32 blend_delta_time = blend_delta_time_frames * (1.0f / 60.0f);

		ImGui::DragFloat("Blending Delta Time frames", &blend_delta_time_frames, 1.0f);
		ImGui::DragFloat("Blending halflife", &blending_halflife, 0.01f, 0.0f);

		const Slice<AnimDBJoint> pose = frame_allocator.alloc<AnimDBJoint>(anim_db.joints_per_pose);

		const Slice<AnimDBJoint> offsets = frame_allocator.alloc<AnimDBJoint>(anim_db.joints_per_pose);

		for (usize i = 0; i < offsets.size; ++i)
		{
			// This is temporary for debugging, this will be persistent in a normal situation
			offsets[i] = AnimDBJoint{};

			offsets[i].position = (start_pose[i].position + offsets[i].position) - end_pose[i].position;
			offsets[i].velocity = (start_pose[i].velocity + offsets[i].velocity) - end_pose[i].velocity;
			offsets[i].angular_velocity = (start_pose[i].angular_velocity + offsets[i].angular_velocity) - end_pose[i].angular_velocity;
			offsets[i].rotation = abs((offsets[i].rotation * start_pose[i].rotation) * inverse(end_pose[i].rotation));
		}

		for (usize i = 0; i < pose.size; ++i)
		{
			decay_spring_damper_exact(offsets[i].position, offsets[i].velocity, blending_halflife, blend_delta_time);
			pose[i].position = end_pose[i].position + offsets[i].position;
			pose[i].velocity = end_pose[i].velocity + offsets[i].velocity;

			decay_spring_damper_exact(offsets[i].rotation, offsets[i].angular_velocity, blending_halflife, blend_delta_time);
			pose[i].rotation = offsets[i].rotation * end_pose[i].rotation;
			pose[i].angular_velocity = offsets[i].angular_velocity + rotateVec3(offsets[i].rotation, end_pose[i].angular_velocity);
		}

		drawAnimPose(anim_db, pose.to_const_slice(), pose_position, pose_rotation, frame_allocator, blend_color);
	}

	ImGui::End();
#endif
}