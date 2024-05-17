#include "motion_matching.h"

#include <shared/math.inl>
#include <shared/log.h>
#include <shared/string.h>

#include <cstddef>

#include "imgui_wrapper.h"
#include "game_state.h"
#include "debug_draw.h"
#include "transform.h"
#include "allocators/frame_allocator.h"
#include "profiler.h"

usize g_motion_match_feature_sizes[] = {
	/* MotionMatchFeature_TrajectoryPosition */ sizeof(AnimDBFeatures::Data::trajectory_positions) / 4,
	/* MotionMatchFeature_TrajectoryDirection */ sizeof(AnimDBFeatures::Data::trajectory_directions) / 4,
	/* MotionMatchFeature_LeftFootPosition */ sizeof(AnimDBFeatures::Data::left_foot_position) / 4,
	/* MotionMatchFeature_LeftFootVelocity */ sizeof(AnimDBFeatures::Data::left_foot_velocity) / 4,
	/* MotionMatchFeature_RightFootPosition */ sizeof(AnimDBFeatures::Data::right_foot_position) / 4,
	/* MotionMatchFeature_RightFootVelocity */ sizeof(AnimDBFeatures::Data::right_foot_velocity) / 4,
	/* MotionMatchFeature_HipVelocity */ sizeof(AnimDBFeatures::Data::hip_velocity) / 4,
};

usize g_motion_match_feature_offsets[] = {
	/* MotionMatchFeature_TrajectoryPosition */ offsetof(AnimDBFeatures::Data, trajectory_positions) / 4,
	/* MotionMatchFeature_TrajectoryDirection */ offsetof(AnimDBFeatures::Data, trajectory_directions) / 4,
	/* MotionMatchFeature_LeftFootPosition */ offsetof(AnimDBFeatures::Data, left_foot_position) / 4,
	/* MotionMatchFeature_LeftFootVelocity */ offsetof(AnimDBFeatures::Data, left_foot_velocity) / 4,
	/* MotionMatchFeature_RightFootPosition */ offsetof(AnimDBFeatures::Data, right_foot_position) / 4,
	/* MotionMatchFeature_RightFootVelocity */ offsetof(AnimDBFeatures::Data, right_foot_velocity) / 4,
	/* MotionMatchFeature_HipVelocity */ offsetof(AnimDBFeatures::Data, hip_velocity) / 4,
};

f32 g_motion_match_feature_weights[] = {
	/* MotionMatchFeature_TrajectoryPosition */ 1.0f,
	/* MotionMatchFeature_TrajectoryDirection */ 1.5f,
	/* MotionMatchFeature_LeftFootPosition */ 0.75f,
	/* MotionMatchFeature_LeftFootVelocity */ 1.0f,
	/* MotionMatchFeature_RightFootPosition */ 0.75f,
	/* MotionMatchFeature_RightFootVelocity */ 1.0f,
	/* MotionMatchFeature_HipVelocity */ 1.0f,
};

static const char* g_feature_names[] = {
	"Trajectory Positions",
	"Trajectory Directions",
	"Left Foot Position",
	"Left Foot Velocity",
	"Right Foot Position",
	"Right Foot Velocity",
	"Hip Velocity",
};

usize AnimDB::query(const AnimDBFeatures& denormalized_features) const
{
	PAW_PROFILER_FUNC();
	AnimDBFeatures query = denormalized_features;
	// Normalize query
	for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
	{
		query.raw_data[i] = (query.raw_data[i] - feature_norm_offsets[i]) / feature_norm_scales[i];
	}

	f32 smallest_square_distance = FLT_MAX;
	usize winner_index = 0;

	for (usize row_index = 0; row_index < feature_data.size; ++row_index)
	{

		f32 square_distance = 0.0f;

		const AnimDBFeatures& row = feature_data[row_index];

		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			square_distance += math_square(query.raw_data[i] - row.raw_data[i]);
		}

		if (square_distance < smallest_square_distance)
		{
			smallest_square_distance = square_distance;
			winner_index = row_index;
		}
	}
	return winner_index;
}

static void drawAnimPose(const AnimDB& anim_db, usize pose_index, const AnimDBFeatures& denormalized_row, Vec3 draw_position, Quat draw_rotation, FrameAllocator& frame_allocator, const Vec4& color, bool draw_velocities = false)
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

static void drawQueryTrajectory(const AnimDBFeatures& denormalized_features, Vec3 draw_position, Quat draw_rotation, const Vec4& color)
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

#ifdef PAW_ENABLE_IMGUI

static void drawQueryGUI(const AnimDBFeatures& normalized_features, const char* text)
{
	ImGui::Text("%s", text);
	ImGui::SameLine();
	ImGui::Separator();

	ImGui::Text("Trajectory Position 0: %g, %g", normalized_features.data.trajectory_positions[0].x, normalized_features.data.trajectory_positions[0].y);
	ImGui::Text("Trajectory Position 1: %g, %g", normalized_features.data.trajectory_positions[1].x, normalized_features.data.trajectory_positions[1].y);
	ImGui::Text("Trajectory Position 2: %g, %g", normalized_features.data.trajectory_positions[2].x, normalized_features.data.trajectory_positions[2].y);

	ImGui::Text("Trajectory Direction 0: %g, %g", normalized_features.data.trajectory_directions[0].x, normalized_features.data.trajectory_directions[0].y);
	ImGui::Text("Trajectory Direction 1: %g, %g", normalized_features.data.trajectory_directions[1].x, normalized_features.data.trajectory_directions[1].y);
	ImGui::Text("Trajectory Direction 2: %g, %g", normalized_features.data.trajectory_directions[2].x, normalized_features.data.trajectory_directions[2].y);

	ImGui::Text("Left Foot Position: %g, %g, %g", normalized_features.data.left_foot_position.x, normalized_features.data.left_foot_position.y, normalized_features.data.left_foot_position.z);
	ImGui::Text("Left Foot Velocity: %g, %g, %g", normalized_features.data.left_foot_velocity.x, normalized_features.data.left_foot_velocity.y, normalized_features.data.left_foot_velocity.z);

	ImGui::Text("Right Foot Position: %g, %g, %g", normalized_features.data.right_foot_position.x, normalized_features.data.right_foot_position.y, normalized_features.data.right_foot_position.z);
	ImGui::Text("Right Foot Velocity: %g, %g, %g", normalized_features.data.right_foot_velocity.x, normalized_features.data.right_foot_velocity.y, normalized_features.data.right_foot_velocity.z);

	ImGui::Text("Hip Velocity: %g, %g, %g", normalized_features.data.hip_velocity.x, normalized_features.data.hip_velocity.y, normalized_features.data.hip_velocity.z);
	ImGui::Separator();
}
#endif

void AnimDB::debugQuery(const GameState& game_state, FrameAllocator& frame_allocator)
{
#ifdef PAW_ENABLE_IMGUI

	if (ImGui::Begin("Motion Matching Debug"))
	{

		AnimDBFeatures query = game_state.anim_db_query;
		// drawQueryTrajectory(query, game_state.entity_position, game_state.entity_rotation, Vec4{0.0f, 1.0f, 1.0f, 1.0f});
		//  drawAnimPose(*this, game_state.anim_query_start_frame, query, game_state.entity_position, game_state.entity_rotation, frame_allocator, Vec4{0.0f, 0.0f, 1.0f, 1.0f});
		{
			AnimDBFeatures next_frame_features = feature_data[game_state.anim_query_start_frame];
			for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
			{
				next_frame_features.raw_data[i] = (next_frame_features.raw_data[i] * feature_norm_scales[i]) + feature_norm_offsets[i];
			}
			// drawQueryTrajectory(next_frame_features, game_state.entity_position, game_state.entity_rotation, Vec4{0.0f, 1.0f, 0.0f, 1.0f});
		}
		ImGui::Text("Current frame %llu", game_state.anim_query_start_frame);

		// Normalize query
		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			query.raw_data[i] = (query.raw_data[i] - feature_norm_offsets[i]) / feature_norm_scales[i];
		}

		f32 smallest_square_distance = FLT_MAX;
		f32 winner_goal_cost = 0.0f;
		f32 winner_pose_cost = 0.0f;
		usize winner_index = 0;

		f32 per_feature_smallest[MotionMatchFeature_Count];
		for (usize i = 0; i < MotionMatchFeature_Count; i++)
		{
			per_feature_smallest[i] = FLT_MAX;
		}
		usize per_feature_winning_index[MotionMatchFeature_Count];

		for (usize row_index = 0; row_index < feature_data.size; ++row_index)
		{
			const AnimDBFeatures& row = feature_data[row_index];

			const usize goal_count = g_motion_match_feature_offsets[MotionMatchFeature_TrajectoryDirection + 1];
			f32 goal_distance = 0.0f;

			for (usize feature_index = 0; feature_index < MotionMatchFeature_Count; ++feature_index)
			{
				f32 smallest = 0.0f;

				const usize f32_count = g_motion_match_feature_sizes[feature_index];
				const usize offset = g_motion_match_feature_offsets[feature_index];
				for (usize i = 0; i < f32_count; ++i)
				{
					smallest += math_square(query.raw_data[i] - row.raw_data[offset + i]);
				}

				if (smallest < per_feature_smallest[feature_index])
				{
					per_feature_smallest[feature_index] = smallest;
					per_feature_winning_index[feature_index] = row_index;
				}
			}

			for (usize i = 0; i < goal_count; i++)
			{
				goal_distance += math_square(query.raw_data[i] - row.raw_data[i]);
			}

			f32 pose_distance = 0.0f;
			for (usize i = goal_count + 1; i < AnimDBFeatures::f32_count; i++)
			{
				pose_distance += math_square(query.raw_data[i] - row.raw_data[i]);
			}

			const f32 square_distance = pose_distance + goal_distance;

			if (row_index == game_state.anim_query_start_frame)
			{
				ImGui::Text("Next frame: Pose: %g, Goal %g, Total: %g", pose_distance, goal_distance, square_distance);
			}

			if (square_distance < smallest_square_distance)
			{
				smallest_square_distance = square_distance;
				winner_index = row_index;
				winner_goal_cost = goal_distance;
				winner_pose_cost = pose_distance;
			}
		}

		ImGui::Text("Winning frame %llu", winner_index);
		ImGui::Text("Winner: Pose: %g, Goal %g, Total: %g", winner_pose_cost, winner_goal_cost, smallest_square_distance);

		drawQueryGUI(query, "Input Query");
		drawQueryGUI(feature_data[game_state.anim_query_start_frame], "Next frame");

		AnimDBFeatures winning_features = feature_data[winner_index];
		drawQueryGUI(winning_features, "Winner");
		// Denormalize
		for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
		{
			winning_features.raw_data[i] = (winning_features.raw_data[i] * feature_norm_scales[i]) + feature_norm_offsets[i];
		}

		ImGui::BeginTable("Feature Winners", 4, ImGuiTableFlags_Borders);
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Value");
		ImGui::TableSetupColumn("Index");
		ImGui::TableHeadersRow();
		static usize selected_feature_index = MotionMatchFeature_Count + 1;
		for (usize feature_index = 0; feature_index < MotionMatchFeature_Count; ++feature_index)
		{
			const char* name = g_feature_names[feature_index];

			ImGui::SameLine();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("%s", name);
			ImGui::TableNextColumn();
			ImGui::Text("%g", per_feature_smallest[feature_index]);
			ImGui::TableNextColumn();
			ImGui::Text("%llu", per_feature_winning_index[feature_index]);
			ImGui::TableNextColumn();
			ImGui::BeginDisabled(feature_index == selected_feature_index);
			ImGui::PushID(name);
			if (ImGui::Button("Show"))
			{
				selected_feature_index = feature_index;
			}
			ImGui::PopID();
			ImGui::EndDisabled();
		}

		ImGui::EndTable();

		if (selected_feature_index < MotionMatchFeature_Count)
		{
			AnimDBFeatures selected_features = feature_data[per_feature_winning_index[selected_feature_index]];
			// Denormalize
			for (usize i = 0; i < AnimDBFeatures::f32_count; i++)
			{
				selected_features.raw_data[i] = (selected_features.raw_data[i] * feature_norm_scales[i]) + feature_norm_offsets[i];
			}
			drawAnimPose(*this, per_feature_winning_index[selected_feature_index], selected_features, game_state.entity_position, game_state.entity_rotation, frame_allocator, Vec4{0.0f, 1.0f, 1.0f, 1.0f});
		}
		// drawAnimPose(*this, winner_index, winning_features, game_state.simulation_position, game_state.simulation_rotation, frame_allocator, Vec4{1.0f, 0.0f, 0.0f, 1.0f});

		ImGui::End();
	}
#else
	PAW_UNUSED_ARG(game_state);
	PAW_UNUSED_ARG(frame_allocator);
#endif
}
