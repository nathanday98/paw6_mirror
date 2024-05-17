#include "usd_anim.h"

#include <shared/std.h>
#include <shared/assert.h>
#include <shared/log.h>
#include <shared/math.inl>
#include <shared/slice.h>

PAW_DISABLE_ALL_WARNINGS_BEGIN
#include <pxr/usdImaging/usdImaging/meshAdapter.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/plug/notice.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primCompositionQuery.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdSkel/animation.h>
#include <pxr/usd/usdSkel/animQuery.h>
#include <pxr/usd/usdSkel/skeletonQuery.h>
#include <pxr/usd/usdSkel/topology.h>
#include <pxr/usd/usdSkel/cache.h>
#include <pxr/usd/usdSkel/root.h>

#include <sgsmooth.h>
PAW_DISABLE_ALL_WARNINGS_END

#include "debug_draw.h"
#include "motion_matching.h"
#include "allocators/arena_allocator.h"
#include "profiler.h"

static Mat4 toMat4(const pxr::GfMatrix4d& usd_mat)
{
	Mat4 mat;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat.data[i][j] = (f32)usd_mat.data()[i * 4 + j];
		}
	}
	return mat;
}

void sgs_error(const char* message)
{
	PAW_LOG_ERROR("SGSmooth: %s", message);
}

void tempLoadAnim(const char* path, ArenaAllocator& persistent_allocator, AnimDB& out_anim_db)
{
	PAW_PROFILER_FUNC();
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(path);
	PAW_ASSERT(stage);

	pxr::UsdGeomXformCache xform_cache;
	pxr::UsdSkelCache skel_cache;

	const double frames_per_second = stage->GetFramesPerSecond();
	const double time_codes_per_second = stage->GetTimeCodesPerSecond();
	PAW_LOG_INFO("Frames Per Second: %g", frames_per_second);
	PAW_LOG_INFO("Time Codes Per Second: %g", time_codes_per_second);

	const double length_seconds = stage->GetEndTimeCode() / time_codes_per_second;
	const double length_minutes = length_seconds / 60.0;
	PAW_LOG_INFO("Length (seconds): %g", length_seconds);
	PAW_LOG_INFO("Length (minutes): %g", length_minutes);

	const s32 frame_count = static_cast<s32>(length_seconds * frames_per_second);
	// const s32 frame_count = 1;

	// const f32 seconds_per_frame = 1.0f / (f32)frames_per_second;

	//  const double frame_delta_time = 1.0 / frames_per_second;

	for (pxr::UsdPrim prim : stage->TraverseAll())
	{
		// if (prim.IsA<pxr::UsdSkelRoot>())
		//{
		//	pxr::UsdSkelRoot skel_root(prim);
		//	skel_cache.Populate(skel_root, pxr::Usd_PrimFlagsPredicate::Tautology());
		// }

		if (prim.IsA<pxr::UsdSkelSkeleton>())
		{
			// pxr::UsdSkelSkeleton skeleton(prim);
		}

		if (prim.IsA<pxr::UsdSkelAnimation>())
		{
			pxr::GfMatrix4d transform = xform_cache.GetLocalToWorldTransform(prim);
			Mat4 prim_transform = toMat4(transform);

			out_anim_db.temp_instance_transform = prim_transform;

			pxr::UsdSkelAnimation animation(prim);
			PAW_LOG_INFO("Animation");
			pxr::UsdSkelAnimQuery anim_query = skel_cache.GetAnimQuery(animation);
			pxr::VtTokenArray joint_tokens = anim_query.GetJointOrder();
			pxr::UsdSkelTopology topology(joint_tokens);
			pxr::VtIntArray parent_indices = topology.GetParentIndices();

			s32 simulation_position_joint_index = -1;
			s32 simulation_rotation_joint_index = -1;
			s32 left_foot_joint_index = -1;
			s32 right_foot_joint_index = -1;
			s32 hip_joint_index = -1;

			for (usize i = 0; i < joint_tokens.size(); i++)
			{
				const pxr::TfToken& token = joint_tokens[i];
				const std::string& str = token.GetString();
				PAW_LOG_INFO("%s", str.c_str());
				if (str.ends_with("BVH_Spine2"))
				{
					simulation_position_joint_index = static_cast<s32>(i);
					hip_joint_index = static_cast<s32>(i);
				}

				if (str.ends_with("BVH_Hips"))
				{
					simulation_rotation_joint_index = static_cast<s32>(i);
				}

				if (str.ends_with("BVH_LeftFoot"))
				{
					left_foot_joint_index = static_cast<s32>(i);
				}

				if (str.ends_with("BVH_RightFoot"))
				{
					right_foot_joint_index = static_cast<s32>(i);
				}
			}

			PAW_ASSERT(simulation_position_joint_index > -1);
			PAW_ASSERT(simulation_rotation_joint_index > -1);
			PAW_ASSERT(left_foot_joint_index > -1);
			PAW_ASSERT(right_foot_joint_index > -1);
			PAW_ASSERT(hip_joint_index > -1);

			out_anim_db.left_foot_index = left_foot_joint_index;
			out_anim_db.right_foot_index = right_foot_joint_index;
			out_anim_db.hip_index = hip_joint_index;

			pxr::VtVec3fArray joint_translations;
			pxr::VtQuatfArray joint_rotations;
			pxr::VtVec3hArray joint_scales;

			std::vector<Vec3> global_positions;
			global_positions.resize(joint_tokens.size() + 1);
			std::vector<Quat> global_rotations;
			global_rotations.resize(joint_tokens.size() + 1);
			std::vector<Vec3> global_velocities;
			global_velocities.resize(joint_tokens.size() + 1);
			std::vector<Vec3> global_angular_velocities;
			global_angular_velocities.resize(joint_tokens.size() + 1);

			std::vector<double> sim_positions_x;
			std::vector<double> sim_positions_z;
			std::vector<double> sim_directions_x;
			std::vector<double> sim_directions_z;
			sim_positions_x.resize(frame_count);
			sim_positions_z.resize(frame_count);
			sim_directions_x.resize(frame_count);
			sim_directions_z.resize(frame_count);

			std::vector<Vec3> hip_positions;
			hip_positions.resize(frame_count);

			out_anim_db.feature_data = persistent_allocator.alloc<AnimDBFeatures>(frame_count);

			out_anim_db.joints_per_pose = joint_tokens.size();
			// out_anim_db.poses = persistent_allocator.alloc<AnimDBJoint>(out_anim_db.joints_per_pose * frame_count);
			// Ew
			out_anim_db.poses = Slice<AnimDBJoint>{new AnimDBJoint[out_anim_db.joints_per_pose * frame_count], out_anim_db.joints_per_pose* frame_count};
			out_anim_db.root_motion = persistent_allocator.alloc<AnimDBJoint>(frame_count);
			out_anim_db.parent_indices = persistent_allocator.alloc<s32>(out_anim_db.joints_per_pose);
			out_anim_db.parent_indices[0] = -1;
			for (usize i = 0; i < parent_indices.size(); i++)
			{
				out_anim_db.parent_indices[i] = parent_indices[i];
			}

			for (s32 frame_index = 0; frame_index < frame_count; frame_index += 1)
			{
				anim_query.ComputeJointLocalTransformComponents(&joint_translations, &joint_rotations, &joint_scales, pxr::UsdTimeCode(static_cast<double>(frame_index)));

				Slice<AnimDBJoint> out_pose = out_anim_db.poses.sub_slice(frame_index * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				AnimDBFeatures& out_features = out_anim_db.feature_data[frame_index];

				Vec3 simulation_position_joint_position{};
				Vec3 simulation_rotation_joint_forward{};
				for (usize joint_index = 0; joint_index < joint_translations.size(); joint_index++)
				{
					const pxr::GfVec3f& usd_joint_translation = joint_translations[joint_index];
					const pxr::GfQuatf& usd_joint_rotation = joint_rotations[joint_index];
					// const pxr::GfVec3h& joint_scale = joint_scales[joint_index];

					const Vec3 joint_local_translation = Vec3{usd_joint_translation[0], usd_joint_translation[1], usd_joint_translation[2]} * 0.01f;
					const Quat joint_local_rotation{usd_joint_rotation.GetImaginary()[0], usd_joint_rotation.GetImaginary()[1], usd_joint_rotation.GetImaginary()[2], usd_joint_rotation.GetReal()};

					out_pose[joint_index].position = joint_local_translation;
					out_pose[joint_index].rotation = joint_local_rotation;

					const int parent_index = parent_indices[joint_index];
					Vec3 global_position = joint_local_translation;
					Quat global_rotation = joint_local_rotation;
					if (parent_index > -1)
					{
						const Vec3& parent_position = global_positions[parent_index];
						const Quat& parent_rotation = global_rotations[parent_index];

						global_position = rotateVec3(parent_rotation, joint_local_translation) + parent_position;
						global_rotation = parent_rotation * joint_local_rotation;
					}

					if (joint_index == simulation_position_joint_index)
					{
						simulation_position_joint_position = global_position;
					}

					if (joint_index == hip_joint_index)
					{
						hip_positions[frame_index] = global_position;
					}

					if (joint_index == simulation_rotation_joint_index)
					{
						const Vec3 dir = rotateVec3(global_rotation, math_world_up) * Vec3{1.0f, 0.0f, 1.0f};
						sim_directions_x[frame_index] = dir.x;
						sim_directions_z[frame_index] = dir.z;
					}

					if (joint_index == left_foot_joint_index)
					{
						if (frame_index < frame_count - 1)
						{
							// out_anim_db.rows[frame_index].left_foot_velocity = out_anim_db.rows[frame_index + 1].left_foot_position - world_pos;
						}

						out_features.data.left_foot_position = global_position;
					}

					if (joint_index == right_foot_joint_index)
					{
						if (frame_index < frame_count - 1)
						{
							// out_anim_db.rows[frame_index].right_foot_velocity = out_anim_db.rows[frame_index + 1].right_foot_position - world_pos;
						}

						out_features.data.right_foot_position = global_position;
					}

					global_positions[joint_index] = global_position;
					global_rotations[joint_index] = global_rotation;
				}

				const Vec3 simulation_ground_position = Vec3{simulation_position_joint_position.x, 0.0f, simulation_position_joint_position.z};

				sim_positions_x[frame_index] = simulation_ground_position.x;
				sim_positions_z[frame_index] = simulation_ground_position.z;
			}

			//{
			//	Vec3 previous_sim_pos{(f32)sim_positions_x[0], 0.0f, (f32)sim_positions_z[0]};

			//	for (usize i = 1; i < sim_positions_x.size(); i++)
			//	{
			//		const Vec3 position{
			//			(f32)sim_positions_x[i],
			//			0.0f,
			//			(f32)sim_positions_z[i],
			//		};
			//		debugDrawLine(previous_sim_pos, position, true, Vec4{0.3f, 0.7f, 0.3f, 0.5f}, 0.025f, PAW_LIFETIME_INFINITE);
			//		previous_sim_pos = position;
			//	}
			//}

			{
				calc_sgsmooth((int)sim_positions_x.size(), sim_positions_x.data(), 31, 3);
				calc_sgsmooth((int)sim_positions_z.size(), sim_positions_z.data(), 31, 3);
				calc_sgsmooth((int)sim_directions_x.size(), sim_directions_z.data(), 61, 3);
				calc_sgsmooth((int)sim_directions_x.size(), sim_directions_z.data(), 61, 3);

				Vec3 previous_sim_pos{(f32)sim_positions_x[0], 0.0f, (f32)sim_positions_z[0]};
				PAW_ASSERT(sim_positions_x.size() == frame_count);
				PAW_ASSERT(sim_positions_z.size() == frame_count);
				PAW_ASSERT(sim_directions_x.size() == frame_count);
				PAW_ASSERT(sim_directions_z.size() == frame_count);

				for (usize frame_index = 0; frame_index < frame_count; frame_index++)
				{
					const Vec3 sim_position{
						(f32)sim_positions_x[frame_index],
						0.0f,
						(f32)sim_positions_z[frame_index],
					};

					const Vec3 sim_rotation_dir = normalize(Vec3{
						(f32)sim_directions_x[frame_index],
						0.0f,
						(f32)sim_directions_z[frame_index],
					});

					const Quat sim_rotation = normalize(between(math_world_forward, sim_rotation_dir));

					// Vec3 prev = sim_position;
					const Quat inverse_sim_rotation = inverse(sim_rotation);

					// Rebase root joint to the sim joint
					Slice<AnimDBJoint> out_pose = out_anim_db.poses.sub_slice(frame_index * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
					out_pose[0].position = rotateVec3(inverse_sim_rotation, out_pose[0].position - sim_position);
					out_pose[0].rotation = inverse_sim_rotation * out_pose[0].rotation;

					// Make sim joint the new root joint
					out_anim_db.root_motion[frame_index].position = sim_position;
					out_anim_db.root_motion[frame_index].rotation = sim_rotation;

					AnimDBFeatures& out_features = out_anim_db.feature_data[frame_index];

					for (usize i = 1; i < 4; i++)
					{
						const usize future_index = math_minUsize(frame_index + i * 20, frame_count - 1);
						const Vec3 future_sim_position{
							(f32)sim_positions_x[future_index],
							0.0f,
							(f32)sim_positions_z[future_index],
						};

						const Vec3 future_sim_rotation_dir = normalize(Vec3{
							(f32)sim_directions_x[future_index],
							0.0f,
							(f32)sim_directions_z[future_index],
						});

						// const Quat future_sim_rotation = normalize(between(math_world_forward, future_sim_rotation_dir));

						const Vec2 local_trajectory_position = xz(rotateVec3(inverse_sim_rotation, future_sim_position - sim_position));
						// const Vec3 local_trajectory_dir = rotateVec3(inverse_sim_rotation, Vec3{math_cos((f32)sim_rotations[future_index]), 0.0f, math_sin((f32)sim_rotations[future_index])});
						const Vec2 local_trajectory_dir = xz(rotateVec3(inverse_sim_rotation, future_sim_rotation_dir)).normalized();

						out_features.data.trajectory_positions[i - 1] = local_trajectory_position;
						out_features.data.trajectory_directions[i - 1] = local_trajectory_dir;
					}

					out_features.data.left_foot_position = rotateVec3(inverse_sim_rotation, out_features.data.left_foot_position - sim_position);
					out_features.data.right_foot_position = rotateVec3(inverse_sim_rotation, out_features.data.right_foot_position - sim_position);

					debugDrawLine(previous_sim_pos, sim_position, true, Vec4{1.0f, 0.0f, 0.0f, 0.5f}, 0.001f, PAW_LIFETIME_INFINITE);
					debugDrawLine(previous_sim_pos, sim_position + sim_rotation_dir * 0.1f, true, Vec4{0.0f, 1.0f, 0.0f, 0.5f}, 0.001f, PAW_LIFETIME_INFINITE);

					previous_sim_pos = sim_position;
				}
			}

			for (usize frame_index = 1; frame_index < frame_count - 1; frame_index++)
			{
				Slice<AnimDBJoint> out_pose = out_anim_db.poses.sub_slice(frame_index * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> next_pose = out_anim_db.poses.const_sub_slice((frame_index + 1) * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> previous_pose = out_anim_db.poses.const_sub_slice((frame_index - 1) * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);

				for (usize joint_index = 0; joint_index < out_anim_db.joints_per_pose; joint_index++)
				{
					const Vec3& previous_position = previous_pose[joint_index].position;
					const Vec3& next_position = next_pose[joint_index].position;
					out_pose[joint_index].velocity = (next_position - previous_position) * 0.5f * (f32)frames_per_second;

					const Quat& previous_rotation = previous_pose[joint_index].rotation;
					const Quat& next_rotation = next_pose[joint_index].rotation;
					out_pose[joint_index].angular_velocity = toScaledAngleAxis(abs(inverse(next_rotation) * previous_rotation)) * 0.05f * (f32)frames_per_second;
				}
			}

			for (usize frame_index = 1; frame_index < frame_count - 1; frame_index++)
			{
				const AnimDBJoint& previous_root_motion = out_anim_db.root_motion[frame_index - 1];
				const AnimDBJoint& next_root_motion = out_anim_db.root_motion[frame_index + 1];
				AnimDBJoint& root_motion = out_anim_db.root_motion[frame_index];

				root_motion.velocity = (next_root_motion.position - previous_root_motion.position) * 0.5f * (f32)frames_per_second;

				root_motion.angular_velocity = toScaledAngleAxis(abs(inverse(next_root_motion.rotation) * previous_root_motion.rotation)) * 0.05f * (f32)frames_per_second;
			}

			{
				Slice<AnimDBJoint> out_pose = out_anim_db.poses.sub_slice(0 * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> pose1 = out_anim_db.poses.const_sub_slice(1 * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> pose2 = out_anim_db.poses.const_sub_slice(2 * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> pose3 = out_anim_db.poses.const_sub_slice(3 * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);

				for (usize joint_index = 0; joint_index < out_anim_db.joints_per_pose; joint_index++)
				{

					out_pose[joint_index].velocity = pose1[joint_index].velocity - (pose3[joint_index].velocity - pose2[joint_index].velocity);
					out_pose[joint_index].angular_velocity = pose1[joint_index].angular_velocity - (pose3[joint_index].angular_velocity - pose2[joint_index].angular_velocity);
				}
			}

			{
				AnimDBJoint& out_root_motion = out_anim_db.root_motion[0];
				const AnimDBJoint& root_motion_1 = out_anim_db.root_motion[1];
				const AnimDBJoint& root_motion_2 = out_anim_db.root_motion[2];
				const AnimDBJoint& root_motion_3 = out_anim_db.root_motion[3];

				out_root_motion.velocity = root_motion_1.velocity - (root_motion_3.velocity - root_motion_2.velocity);
				out_root_motion.angular_velocity = root_motion_1.angular_velocity - (root_motion_3.angular_velocity - root_motion_2.angular_velocity);
			}

			{
				Slice<AnimDBJoint> out_pose = out_anim_db.poses.sub_slice(frame_count - 1 * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> pose1 = out_anim_db.poses.const_sub_slice((frame_count - 2) * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				const Slice<const AnimDBJoint> pose2 = out_anim_db.poses.const_sub_slice((frame_count - 3) * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);

				for (usize joint_index = 0; joint_index < out_anim_db.joints_per_pose; joint_index++)
				{

					out_pose[joint_index].velocity = pose1[joint_index].velocity + (pose1[joint_index].velocity - pose2[joint_index].velocity);
					out_pose[joint_index].angular_velocity = pose1[joint_index].angular_velocity + (pose1[joint_index].angular_velocity - pose2[joint_index].angular_velocity);
				}
			}

			{
				AnimDBJoint& out_root_motion = out_anim_db.root_motion[frame_count - 1];
				const AnimDBJoint& root_motion_1 = out_anim_db.root_motion[frame_count - 2];
				const AnimDBJoint& root_motion_2 = out_anim_db.root_motion[frame_count - 3];

				out_root_motion.velocity = root_motion_1.velocity + (root_motion_1.velocity - root_motion_2.velocity);
				out_root_motion.angular_velocity = root_motion_1.angular_velocity + (root_motion_1.angular_velocity - root_motion_2.angular_velocity);
			}

			for (usize frame_index = 0; frame_index < frame_count; frame_index++)
			{
				const Slice<const AnimDBJoint> pose = out_anim_db.poses.const_sub_slice(frame_index * out_anim_db.joints_per_pose, out_anim_db.joints_per_pose);
				AnimDBFeatures& out_features = out_anim_db.feature_data[frame_index];

				for (usize joint_index = 0; joint_index < out_anim_db.joints_per_pose; joint_index++)
				{
					const Vec3& joint_local_translation = pose[joint_index].position;
					const Quat& joint_local_rotation = pose[joint_index].rotation;
					const Vec3& joint_local_velocity = pose[joint_index].velocity;
					const Vec3& joint_local_angular_velocity = pose[joint_index].angular_velocity;

					const int parent_index = parent_indices[joint_index];
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

						global_position = rotateVec3(parent_rotation, joint_local_translation) + parent_position;
						global_rotation = parent_rotation * joint_local_rotation;
						global_velocity = parent_velocity + rotateVec3(parent_rotation, joint_local_velocity) + cross(parent_angular_velocity, rotateVec3(parent_rotation, joint_local_translation));
						global_angular_velocity = rotateVec3(parent_rotation, joint_local_angular_velocity) + parent_angular_velocity;
					}
					else
					{

						global_position = joint_local_translation;
						global_rotation = joint_local_rotation;
						global_velocity = joint_local_velocity;
						global_angular_velocity = joint_local_angular_velocity;
					}

					global_positions[joint_index] = global_position;
					global_rotations[joint_index] = global_rotation;
					global_velocities[joint_index] = global_velocity;
					global_angular_velocities[joint_index] = global_angular_velocity;

					if (joint_index == left_foot_joint_index)
					{
						out_features.data.left_foot_velocity = global_velocity;
					}

					if (joint_index == right_foot_joint_index)
					{
						out_features.data.right_foot_velocity = global_velocity;
					}

					if (joint_index == hip_joint_index)
					{
						out_features.data.hip_velocity = global_velocity;
					}
				}
			}

			// Normalize features

			f32 variances[AnimDBFeatures::f32_count];

			for (usize feature_index = 0; feature_index < MotionMatchFeature_Count; feature_index++)
			{
				const usize size = g_motion_match_feature_sizes[feature_index];
				const usize offset = g_motion_match_feature_offsets[feature_index];
				const f32 weight = g_motion_match_feature_weights[feature_index];

				// First compute what is essentially the mean value for each feature dimension
				for (usize i = 0; i < size; i++)
				{
					out_anim_db.feature_norm_offsets[offset + i] = 0.0f;
				}

				for (usize row = 0; row < out_anim_db.feature_data.size; row++)
				{
					for (usize i = 0; i < size; i++)
					{
						out_anim_db.feature_norm_offsets[offset + i] += out_anim_db.feature_data[row].raw_data[offset + i] / (f32)out_anim_db.feature_data.size;
					}
				}

				// Compute the variance of each feature dimension
				for (usize i = 0; i < size; i++)
				{
					variances[i] = 0.0f;
				}

				for (usize row = 0; row < out_anim_db.feature_data.size; row++)
				{
					for (usize i = 0; i < size; i++)
					{
						const f32 feature_data = out_anim_db.feature_data[row].raw_data[offset + i];
						variances[i] += math_square(feature_data - out_anim_db.feature_norm_offsets[offset + i]) / (f32)out_anim_db.feature_data.size;
					}
				}

				// Compute the overall std of the feature as the average std across all dimensions
				f32 std = 0.0f;
				for (usize i = 0; i < size; i++)
				{
					std += math_squareRoot(variances[i]) / (f32)size;
				}

				// Features with no variation can have zero std which is almost always a bug
				PAW_ASSERT(std > 0.0f);

				// The scale of a feature is just the std divided by the weight
				for (usize i = 0; i < size; i++)
				{
					out_anim_db.feature_norm_scales[offset + i] = std / weight;
				}

				for (usize row = 0; row < out_anim_db.feature_data.size; row++)
				{
					for (usize i = 0; i < size; i++)
					{
						f32& feature_data = out_anim_db.feature_data[row].raw_data[offset + i];
						feature_data = (feature_data - out_anim_db.feature_norm_offsets[offset + i]) / out_anim_db.feature_norm_scales[offset + i];
					}
				}
			}

			// query.ComputeJointLocalTransforms(&joint_transforms, pxr::UsdTimeCode(0.0));
			//  pxr::UsdSkelAnimQuery query(animation);
		}
	}
}
