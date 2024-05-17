#include "transform.h"

#include <shared/math.inl>
#include <shared/string.h>
#include <shared/string_id.inl>

Transform Transform::inversed() const
{
	Transform result = {};
	result.scale = 1.0f / scale;

	const Quat inverse_rotation = inverse(rotation);
	result.translation = -(rotateVec3(inverse_rotation, translation * result.scale));
	result.rotation = inverse_rotation;
	return result;
}

Vec3 Transform::calcRight() const
{
	return rotateVec3(rotation, math_world_right);
}

Vec3 Transform::calcForward() const
{
	return rotateVec3(rotation, math_world_forward);
}

Vec3 Transform::calcUp() const
{
	return rotateVec3(rotation, math_world_up);
}

static void transformFormatter(byte* out_buffer, usize out_buffer_size_bytes, const void* data)
{
	const Transform* transform = reinterpret_cast<const Transform*>(data);
	string_format(
		out_buffer,
		out_buffer_size_bytes,
		"{\n\ttranslation: %p{Vec3},\n\trotation: %p{Quat},\n\tscale: %p{Vec3},\n}",
		&transform->translation,
		&transform->rotation,
		&transform->scale);
}

void Transform::registerFormatter()
{
	string_registerFormatter(PAW_STRING_ID("Transform"), &transformFormatter);
}

Mat4 toMat4(const Transform& t)
{
	Mat4 result = toMat4(t.rotation);
	result.data[0][0] *= t.scale;
	result.data[1][1] *= t.scale;
	result.data[2][2] *= t.scale;

	result.data[3][0] = t.translation.x;
	result.data[3][1] = t.translation.y;
	result.data[3][2] = t.translation.z;
	result.data[3][3] = 1.0f;
	return result;
}

Transform lerp(const Transform& a, const Transform& b, f32 t)
{
	return Transform{
		.rotation = slerp(a.rotation, b.rotation, t),
		.translation = lerp(a.translation, b.translation, t),
		.scale = math_lerp(a.scale, b.scale, t),
	};
}