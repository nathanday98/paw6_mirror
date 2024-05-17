#pragma once

#include <shared/std.h>
#include <shared/math.h>
#include <shared/slice.h>
#include <shared/optional.h>

#include "transform.h"

struct Skeleton;

struct Bone
{
	Mat4 inverse_bind_transform;
	s32 parent_index;
};

struct Pose
{
	Slice<Transform> local_transforms;
	Slice<Mat4> global_transforms;
	bool global_transforms_cached;
	void calcRenderTransforms(Slice<Mat4>& transforms, const Skeleton& skeleton) const;
	void calcGlobalTransforms(const Skeleton& skeleton);
	void debugDraw(const Skeleton& skeleton, const Vec4& color) const;
};

struct Skeleton
{
	Slice<Bone> bones;
	Slice<u64> bone_name_hashes;
	Pose bind_pose;

	Optional<usize> lookupBone(u64 name_hash) const;
};

struct BoneFrame
{
	Transform start;
	Transform end;
};

struct AnimData
{
	enum class WrapMode
	{
		None,
		Loop,
	};

	void emplaceBoneFrame(usize bone_index, usize frame_index, BoneFrame&& frame);
	const BoneFrame& getBoneFrame(usize bone_index, usize frame_index);

	struct SampleTime
	{
		usize frame_index;
		f32 fraction;
	};
	SampleTime calcSampleTime(f32 time_in_seconds);

	Slice<BoneFrame> bone_frames;
	usize bone_count;
	usize frame_count;
	f32 seconds_per_frame;
	f32 duration_seconds;
	WrapMode wrap_mode;
};

struct Animator
{

	AnimData* anim;
	f32 timer;
	f32 speed;
	Pose pose;
};