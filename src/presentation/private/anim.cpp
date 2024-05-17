#include "anim.h"

#include <shared/math.inl>

#include "debug_draw.h"

Optional<usize> Skeleton::lookupBone(u64 name_hash) const
{
	for (usize i = 0; i < bone_name_hashes.size; i++)
	{
		if (bone_name_hashes[i] == name_hash)
		{
			return i;
		}
	}

	return optional::none;
}

void Pose::calcRenderTransforms(Slice<Mat4>& transforms, const Skeleton& skeleton) const
{
	if (!global_transforms_cached)
	{

		for (usize joint_index = 0; joint_index < local_transforms.size; joint_index++)
		{
			const Transform& transform = local_transforms[joint_index];
			const Bone& bone = skeleton.bones[joint_index];
			Mat4& final_transform = transforms[joint_index];

			Mat4 parent_mat = Mat4{};
			if (bone.parent_index >= 0)
			{
				parent_mat = transforms[bone.parent_index];
			}

			final_transform = parent_mat * toMat4(transform);
		}

		for (usize transform_index = 0; transform_index < transforms.size; transform_index++)
		{
			transforms[transform_index] =
				transforms[transform_index] * skeleton.bones[transform_index].inverse_bind_transform;
		}
	}
	else
	{
		for (usize transform_index = 0; transform_index < transforms.size; transform_index++)
		{
			transforms[transform_index] =
				global_transforms[transform_index] * skeleton.bones[transform_index].inverse_bind_transform;
		}
	}
}

void Pose::calcGlobalTransforms(const Skeleton& skeleton)
{
	for (usize joint_index = 0; joint_index < local_transforms.size; joint_index++)
	{
		const Transform& transform = local_transforms[joint_index];
		const Bone& bone = skeleton.bones[joint_index];
		Mat4& final_transform = global_transforms[joint_index];

		Mat4 parent_mat = Mat4{};
		if (bone.parent_index >= 0)
		{
			parent_mat = global_transforms[bone.parent_index];
		}

		final_transform = parent_mat * toMat4(transform);
	}
	global_transforms_cached = true;
}

void Pose::debugDraw(const Skeleton& skeleton, const Vec4& color) const
{
	PAW_ASSERT(global_transforms_cached);
	for (usize joint_index = 0; joint_index < local_transforms.size; joint_index++)
	{
		const Transform& joint = local_transforms[joint_index];
		const Bone& bone = skeleton.bones[joint_index];
		Mat4 parent_mat = Mat4{};
		if (bone.parent_index >= 0)
		{
			parent_mat = global_transforms[bone.parent_index];
		}

		const Mat4 local_mat = parent_mat * toMat4(joint);
		const Vec3 parent_origin = parent_mat.transformPoint(Vec3{});
		const Vec3 origin = local_mat.transformPoint(Vec3{});
		debugDrawLine(parent_origin, origin, false, color);
	}
}

void AnimData::emplaceBoneFrame(usize bone_index, usize frame_index, BoneFrame&& frame)
{
	bone_frames[frame_index * bone_count + bone_index] = std::move(frame);
}

const BoneFrame& AnimData::getBoneFrame(usize bone_index, usize frame_index)
{
	return bone_frames[frame_index * bone_count + bone_index];
}

AnimData::SampleTime AnimData::calcSampleTime(f32 time_in_seconds)
{
	const f32 local_anim_time = math_remainder(time_in_seconds, duration_seconds - seconds_per_frame);
	const f32 frame_index_f = local_anim_time / seconds_per_frame;
	const usize frame_index = math_floorToUsize(frame_index_f);
	const f32 frac = math_remainder(local_anim_time, seconds_per_frame);
	return {.frame_index = frame_index, .fraction = frac};
}