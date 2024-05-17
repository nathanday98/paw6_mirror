#pragma once

#include "anim.h"
#include "mesh.h"

struct Assets
{
	inline static Skeleton pedestrian_skeleton;
	inline static AnimData walk_anim;
	inline static SkeletalMesh pedestrian_mesh;

	static void loadAll();
};