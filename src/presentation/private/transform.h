#pragma once

#include <shared/math.h>

struct Transform
{
	Quat rotation;
	Vec3 translation;
	f32 scale = 1.0f;

	Transform inversed() const;

	Vec3 calcRight() const;
	Vec3 calcForward() const;
	Vec3 calcUp() const;

	static Transform lerp(const Transform& a, const Transform& b, f32 t);
	static void registerFormatter();
};

Mat4 toMat4(const Transform& t);
Transform lerp(const Transform& a, const Transform& b, f32 t);
