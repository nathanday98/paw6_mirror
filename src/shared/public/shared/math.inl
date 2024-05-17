#pragma once

#include <cmath>

#include <shared/math.h>
#include <shared/assert.h>
#include <shared/log.h>

static constexpr f32 math_pi32 = 3.14159265359f;
static constexpr f64 math_pi64 = 3.14159265358979323846;

static constexpr Vec3 math_world_right = Vec3{1.0f, 0.0f, 0.0f};
static constexpr Vec3 math_world_forward = Vec3{0.0f, 0.0f, -1.0f};
static constexpr Vec3 math_world_up = Vec3{0.0f, 1.0f, 0.0f};

static inline f32 math_clamp(f32 x, f32 min, f32 max);
static inline usize math_clamp(usize x, usize min, usize max);
static inline u32 math_clampU32(u32 x, u32 min, u32 max);
static inline f32 math_max(f32 a, f32 b);
static inline u32 math_maxU32(u32 a, u32 b);
static inline u64 math_maxU64(u64 a, u64 b);
static inline s32 math_maxS32(s32 a, s32 b);
static inline f32 math_min(f32 a, f32 b);
static inline s32 math_minS32(s32 a, s32 b);
static inline usize math_minUsize(usize a, usize b);
static inline f32 math_sin(f32 x);
static inline f32 math_cos(f32 x);
static inline f32 math_acos(f32 x);
static inline f32 math_tan(f32 x);
static inline f32 math_atan(f32 x);
static inline f32 math_atan2(f32 x, f32 y);
static inline f32 math_toRadians(f32 degrees);
static inline f32 math_toDegrees(f32 radians);
static inline f32 math_squareRoot(f32 x);
static inline f32 math_square(f32 x);
static inline f32 math_fractionalF32(f32 x);
static inline f32 math_isClose(f32 a, f32 b, f32 margin);
static inline f32 math_abs(f32 x);
static inline s32 math_abs(s32 x);
static inline f32 math_lerp(f32 a, f32 b, f32 t);
static inline Vec2 math_lerp(Vec2 a, Vec2 b, f32 t);
static inline s32 math_midpoint(s32 a, s32 b);
static inline Vec2S32 math_midpoint(Vec2S32 a, Vec2S32 b);
static inline f32 math_remainder(f32 a, f32 b);
static inline usize math_floorToUsize(f32 x);
static inline usize math_roundToUsize(f32 x);
static inline f32 math_floor(f32 x);
static inline f32 math_round(f32 x);
static inline s32 math_floor_to_s32(f32 x);
static inline Vec2 math_roundVec2(Vec2 const& x);
static inline f32 math_ceil(f32 x);
static inline f32 math_pow(f32 x, f32 power);

f32 math_clamp(f32 x, f32 min, f32 max)
{
	return x < min ? min : (x > max ? max : x);
}

usize math_clamp(usize x, usize min, usize max)
{
	return x < min ? min : (x > max ? max : x);
}

u32 math_clampU32(u32 x, u32 min, u32 max)
{
	return x < min ? min : (x > max ? max : x);
}

u32 math_maxU32(u32 a, u32 b)
{
	return a > b ? a : b;
}

u64 math_maxU64(u64 a, u64 b)
{
	return a > b ? a : b;
}

f32 math_max(f32 a, f32 b)
{
	return std::max(a, b);
}

s32 math_maxS32(s32 a, s32 b)
{
	return a > b ? a : b;
}

f32 math_min(f32 a, f32 b)
{
	return std::min(a, b);
}

s32 math_minS32(s32 a, s32 b)
{
	return a < b ? a : b;
}

usize math_minUsize(usize a, usize b)
{
	return a < b ? a : b;
}

f32 math_sin(f32 x)
{
	return std::sinf(x);
}

f32 math_cos(f32 x)
{
	return std::cosf(x);
}

f32 math_acos(f32 x)
{
	return std::acosf(x);
}

f32 math_tan(f32 x)
{
	return std::tanf(x);
}

f32 math_atan(f32 x)
{
	return std::atanf(x);
}

f32 math_atan2(f32 x, f32 y)
{
	return std::atan2f(y, x);
}

f32 math_toRadians(f32 degrees)
{
	return degrees * (math_pi32 / 180.0f);
}

f32 math_toDegrees(f32 radians)
{
	return radians * (180.0f / math_pi32);
}

f32 math_squareRoot(f32 x)
{
	return std::sqrtf(x);
}

f32 math_square(f32 x)
{
	return x * x;
}

f32 math_fractionalF32(f32 x)
{
	f32 int_part;
	return std::modf(x, &int_part);
}

f32 math_isClose(f32 a, f32 b, f32 margin)
{
	return a <= b + margin && a >= b - margin;
}

f32 math_abs(f32 x)
{
	return std::abs(x);
}

s32 math_abs(s32 x)
{
	return std::abs(x);
}

f32 math_lerp(f32 a, f32 b, f32 t)
{
	return a * (1.0f - t) + b * t;
}

Vec2 math_lerp(Vec2 a, Vec2 b, f32 t)
{
	return Vec2{math_lerp(a.x, b.x, t), math_lerp(a.y, b.y, t)};
}

s32 math_midpoint(s32 a, s32 b)
{
	return (a + b) / 2;
}

f32 math_remainder(f32 a, f32 b)
{
	return fmod(a, b);
}

usize math_floorToUsize(f32 x)
{
	return static_cast<usize>(std::floorf(x));
}

usize math_roundToUsize(f32 x)
{
	return static_cast<usize>(std::round(x));
}

f32 math_floor(f32 x)
{
	return std::floorf(x);
}

f32 math_round(f32 x)
{
	return std::roundf(x);
}

s32 math_floor_to_s32(f32 x)
{
	return static_cast<s32>(std::floorf(x));
}

Vec2 math_roundVec2(Vec2 const& x)
{
	return Vec2{math_round(x.x), math_round(x.y)};
}

f32 math_ceil(f32 x)
{
	return std::ceilf(x);
}

f32 math_pow(f32 x, f32 power)
{
	return std::powf(x, power);
}

Vec2 Vec2::operator+(Vec2 const& rhs) const
{
	return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(Vec2 const& rhs) const
{
	return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator*(Vec2 const& rhs) const
{
	return {x * rhs.x, y * rhs.y};
}

Vec2 Vec2::operator/(Vec2 const& rhs) const
{
	return {x / rhs.x, y / rhs.y};
}

inline Vec2 Vec2::operator*(f32 rhs) const
{
	return {x * rhs, y * rhs};
}

inline Vec2 Vec2::operator/(f32 rhs) const
{
	return {x / rhs, y / rhs};
}

Vec2 Vec2::operator-() const
{
	return {-x, -y};
}

void Vec2::operator+=(Vec2 const& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator-=(Vec2 const& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator*=(Vec2 const& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
}

void Vec2::operator/=(Vec2 const& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
}

void Vec2::operator*=(const f32 rhs)
{
	x *= rhs;
	y *= rhs;
}

void Vec2::operator/=(const f32 rhs)
{
	x /= rhs;
	y /= rhs;
}

f32 Vec2::lengthSquared() const
{
	return (x * x) + (y * y);
}

f32 Vec2::length() const
{
	return math_squareRoot(lengthSquared());
}

Vec2 Vec2::normalized() const
{
	return *this / length();
}

inline Vec2 Vec2::safelyNormalized() const
{
	const f32 l = length();
	return l != 0.0f ? (*this / l) : Vec2{0.0f, 0.0f};
}

Vec2 Vec2::leftPerp() const
{
	return {y, -x};
}

Vec2 Vec2::rightPerp() const
{
	return {-y, x};
}

bool Vec2::isClose(Vec2 const& rhs, f32 margin) const
{
	const f32 dist_squared = (rhs - *this).lengthSquared();
	return dist_squared <= margin * margin;
}

f32 Vec2::dot(Vec2 const& rhs) const
{
	return (x * rhs.x) + (y * rhs.y);
}

f32* Vec2::getPtr()
{
	return &x;
}

f32 const* Vec2::getPtr() const
{
	return &x;
}

inline Vec2 Vec2::floored() const
{
	return Vec2{math_floor(x), math_floor(y)};
}

static inline Vec3 operator+(Vec3 lhs, Vec3 rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

static inline Vec3 operator-(Vec3 lhs, Vec3 rhs)
{
	return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

static inline Vec3 operator*(Vec3 lhs, Vec3 rhs)
{
	return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}

static inline Vec3 operator/(Vec3 lhs, Vec3 rhs)
{
	return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
}

static inline Vec3 operator*(Vec3 lhs, f32 rhs)
{
	return {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
}

static inline Vec3 operator/(Vec3 lhs, f32 rhs)
{
	return {lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
}

static inline Vec3 operator*(const f32 lhs, Vec3 const& rhs)
{
	return {rhs.x * lhs, rhs.y * lhs, rhs.z * lhs};
}

static inline Vec3 operator-(Vec3 rhs)
{
	return {-rhs.x, -rhs.y, -rhs.z};
}

static inline void operator+=(Vec3& lhs, Vec3 rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
}

static inline void operator-=(Vec3& lhs, Vec3 rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
}

static inline void operator*=(Vec3& lhs, Vec3 rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
}

static inline void operator/=(Vec3& lhs, Vec3 rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
}

static inline void operator*=(Vec3& lhs, f32 rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
}

static inline void operator/=(Vec3& lhs, f32 rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
}

static inline f32 lengthSquared(Vec3 v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

static inline f32 length(Vec3 v)
{
	return math_squareRoot(lengthSquared(v));
}

static inline Vec2 xy(Vec3 v)
{
	return Vec2{v.z, v.y};
}

static inline Vec2 xz(Vec3 v)
{
	return Vec2{v.x, v.z};
}

static inline Vec3 fromXZ(Vec2 v)
{
	return Vec3{v.x, 0.0f, v.y};
}

static inline Vec3 lerp(Vec3 a, Vec3 b, f32 t)
{
	return a * (1.0f - t) + b * t;
}

static inline Vec3 normalize(Vec3 v)
{
	return v / length(v);
}

static inline Vec3 normalizeSafely(Vec3 v)
{
	const f32 l = length(v);
	return l != 0.0f ? v / l : Vec3{};
}

static inline f32 dot(Vec3 lhs, Vec3 rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static inline Vec3 cross(Vec3 lhs, Vec3 rhs)
{
	return Vec3{
		.x = (lhs.y * rhs.z) - (lhs.z * rhs.y),
		.y = (lhs.z * rhs.x) - (lhs.x * rhs.z),
		.z = (lhs.x * rhs.y) - (lhs.y * rhs.x),
	};
}

static inline f32 angleBetween(Vec3 a, Vec3 b)
{
	const f32 d = dot(a, b) / (length(a) * length(b));
	return math_acos(d);
}

static inline Vec3 xzLeftPerp(Vec3 v)
{
	return {v.z, v.y, -v.x};
}

static inline Vec3 xzRightPerp(Vec3 v)
{
	return {-v.z, v.y, v.x};
}

static inline Vec2S32 operator+(Vec2S32 lhs, const Vec2S32 rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y};
}

static inline Vec2S32 operator-(Vec2S32 lhs, const Vec2S32 rhs)
{
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}

static inline Vec2S32 operator*(Vec2S32 lhs, const Vec2S32 rhs)
{
	return {lhs.x * rhs.x, lhs.y * rhs.y};
}

static inline Vec2S32 operator/(Vec2S32 lhs, const Vec2S32 rhs)
{
	return {lhs.x / rhs.x, lhs.y / rhs.y};
}

static inline Vec2S32 operator*(Vec2S32 lhs, s32 rhs)
{
	return {lhs.x * rhs, lhs.y * rhs};
}

static inline Vec2S32 operator/(Vec2S32 lhs, s32 rhs)
{
	return {lhs.x / rhs, lhs.y / rhs};
}

static inline Vec2S32 operator-(Vec2S32 rhs)
{
	return {-rhs.x, -rhs.y};
}

static inline void operator+=(Vec2S32& lhs, Vec2S32 rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
}

static inline void operator-=(Vec2S32& lhs, Vec2S32 rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
}

static inline void operator*=(Vec2S32& lhs, Vec2S32 rhs)
{
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
}

static inline void operator/=(Vec2S32& lhs, Vec2S32 rhs)
{
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
}

static inline void operator*=(Vec2S32& lhs, s32 rhs)
{
	lhs.x *= rhs;
	lhs.y *= rhs;
}

static inline void operator/=(Vec2S32& lhs, s32 rhs)
{
	lhs.x /= rhs;
	lhs.y /= rhs;
}

static inline bool operator==(Vec2S32 lhs, Vec2S32 rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

Vec2S32 math_midpoint(Vec2S32 a, Vec2S32 b)
{
	return (a + b) / 2;
}

f32 const* Vec4::getPtr() const
{
	return &x;
}

f32* Vec4::getPtr()
{
	return &x;
}

Vec3 Vec4::xyz() const
{
	return Vec3{x, y, z};
}

static inline Mat4 operator*(Mat4 const& lhs, Mat4 const& rhs)
{
	Mat4 result = {};
	for (usize row = 0; row < 4; row++)
	{
		for (usize column = 0; column < 4; column++)
		{
			f32 sum = 0.0f;
			for (usize i = 0; i < 4; i++)
			{
				sum += lhs.data[i][column] * rhs.data[row][i];
			}
			result.data[row][column] = sum;
		}
	}
	return result;
}

static inline void operator*=(Mat4& lhs, Mat4 const& rhs)
{
	lhs = lhs * rhs;
}

Mat4 Mat4::translate(Vec3 const& translation)
{
	Mat4 result = {};
	result.data[3][0] = translation.x;
	result.data[3][1] = translation.y;
	result.data[3][2] = translation.z;
	return result;
}

Mat4 Mat4::scale(Vec3 const& scale)
{
	Mat4 result = {};
	result.data[0][0] = scale.x;
	result.data[1][1] = scale.y;
	result.data[2][2] = scale.z;
	return result;
}

inline Mat4 Mat4::ortho(f32 left, f32 top, f32 right, f32 bottom, f32 near, f32 far)
{
	Mat4 result{};
	const f32 width = right - left;
	const f32 height = top - bottom;
	const f32 depth = far - near;

	// scale
	result.data[0][0] = 2.0f / width;
	result.data[1][1] = 2.0f / height;
	result.data[2][2] = 2.0f / depth;
	result.data[3][3] = 1.0f;

	// translation
	result.data[3][0] = -((right + left) / width);
	result.data[3][1] = -((top + bottom) / height);
	// result.data[3][2] = -((far + near) / depth);
	return result;
}

Vec3 Mat4::transformPoint(Vec3 const& point) const
{
	return transformVec4(Vec4{point.x, point.y, point.z, 1.0f}).xyz();
}

Vec3 Mat4::transformVector(Vec3 const& point) const
{
	return transformVec4(Vec4{point.x, point.y, point.z, 0.0f}).xyz();
}

Vec4 Mat4::transformVec4(Vec4 const& vec) const
{
	f32 const* vec_ptr = vec.getPtr();
	Vec4 result = {};
	f32* result_ptr = result.getPtr();

	for (usize row = 0; row < 4; row++)
	{
		f32 sum = 0.0f;
		for (usize column = 0; column < 4; column++)
		{
			sum += data[column][row] * vec_ptr[column];
		}
		result_ptr[row] = sum;
	}
	return result;
}

static inline Quat operator-(Quat rhs)
{
	return Quat{-rhs.x, -rhs.y, -rhs.z, -rhs.w};
}

static inline Quat operator*(Quat lhs, f32 rhs)
{
	return Quat{lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs};
}

static inline Quat operator/(Quat lhs, f32 rhs)
{
	return Quat{lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs};
}

static inline Quat operator*(f32 lhs, Quat rhs)
{
	return Quat{rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs};
}

static inline Quat operator/(f32 lhs, Quat rhs)
{
	return Quat{rhs.x / lhs, rhs.y / lhs, rhs.z / lhs, rhs.w / lhs};
}

static inline Quat operator*(Quat lhs, Quat rhs)
{
	return Quat{
		.x = (lhs.x * rhs.w) + (lhs.y * rhs.z) - (lhs.z * rhs.y) + (lhs.w * rhs.x),
		.y = (-lhs.x * rhs.z) + (lhs.y * rhs.w) + (lhs.z * rhs.x) + (lhs.w * rhs.y),
		.z = (lhs.x * rhs.y) - (lhs.y * rhs.x) + (lhs.z * rhs.w) + (lhs.w * rhs.z),
		.w = (-lhs.x * rhs.x) - (lhs.y * rhs.y) - (lhs.z * rhs.z) + (lhs.w * rhs.w),
	};
}

static inline void operator*=(Quat& lhs, Quat rhs)
{
	lhs = lhs * rhs;
}

static inline Quat operator+(Quat lhs, Quat rhs)
{
	return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
}

static inline Quat operator-(Quat lhs, Quat rhs)
{
	return {lhs.w - rhs.w, lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

static inline bool operator==(Quat lhs, Quat rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

static inline f32 lengthSquared(Quat q)
{
	return (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
}

static inline f32 length(Quat q)
{
	return math_squareRoot(lengthSquared(q));
}

static inline Quat normalize(Quat q)
{
	return q / length(q);
}

static inline Mat4 toMat4(Quat q)
{
	Mat4 result;

	const Quat normalized_quat = normalize(q);

	float xx, yy, zz, xy, xz, yz, wx, wy, wz;

	xx = normalized_quat.x * normalized_quat.x;
	yy = normalized_quat.y * normalized_quat.y;
	zz = normalized_quat.z * normalized_quat.z;
	xy = normalized_quat.x * normalized_quat.y;
	xz = normalized_quat.x * normalized_quat.z;
	yz = normalized_quat.y * normalized_quat.z;
	wx = normalized_quat.w * normalized_quat.x;
	wy = normalized_quat.w * normalized_quat.y;
	wz = normalized_quat.w * normalized_quat.z;

	result.data[0][0] = 1.0f - 2.0f * (yy + zz);
	result.data[0][1] = 2.0f * (xy + wz);
	result.data[0][2] = 2.0f * (xz - wy);
	result.data[0][3] = 0.0f;

	result.data[1][0] = 2.0f * (xy - wz);
	result.data[1][1] = 1.0f - 2.0f * (xx + zz);
	result.data[1][2] = 2.0f * (yz + wx);
	result.data[1][3] = 0.0f;

	result.data[2][0] = 2.0f * (xz + wy);
	result.data[2][1] = 2.0f * (yz - wx);
	result.data[2][2] = 1.0f - 2.0f * (xx + yy);
	result.data[2][3] = 0.0f;

	result.data[3][0] = 0.0f;
	result.data[3][1] = 0.0f;
	result.data[3][2] = 0.0f;
	result.data[3][3] = 1.0f;

	return result;
}

static inline Quat inverse(Quat q)
{
	return {q.x, q.y, q.z, -q.w};
	// return -q / lengthSquared(q);
}

static inline Vec3 rotateVec3(Quat q, Vec3 v)
{
	const Vec3 t = 2.0f * cross(Vec3{q.x, q.y, q.z}, v);
	return v + q.w * t + cross(Vec3{q.x, q.y, q.z}, t);
}

static inline f32 dot(Quat lhs, Quat rhs)
{
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
}

static inline Quat fromAngleAxis(Vec3 axis, f32 angle)
{
	const f32 sin_of_rotation = math_sin(angle * 0.5f);
	return Quat{
		.x = axis.x * sin_of_rotation,
		.y = axis.y * sin_of_rotation,
		.z = axis.z * sin_of_rotation,
		.w = math_cos(angle * 0.5f),
	};
}

static inline Quat slerp(Quat in_a, Quat in_b, f32 t)
{
	Quat a = in_a;
	Quat b = in_b;
	f32 cos_theta = dot(a, b);
	if (cos_theta < 0.0f)
	{
		cos_theta = -cos_theta;
		b = Quat{-b.x, -b.y, -b.z, -b.w};
	}

	f32 s1;
	f32 s2;
	if (1.0f - cos_theta > 0.0001f)
	{
		const f32 theta = math_acos(cos_theta);
		const f32 sin_theta = std::sin(theta);
		s1 = math_sin((1.0f - t) * theta) / sin_theta;
		s2 = math_sin(t * theta) / sin_theta;
	}
	else
	{
		// Very close, do linear interp
		s1 = 1.0f - t;
		s2 = t;
	}

	return a * s1 + b * s2;
}

static inline Quat abs(Quat q)
{
	return q.w < 0.0f ? -q : q;
}

static inline Vec3 log(Quat q, f32 eps = 1e-8f)
{
	const f32 length = math_squareRoot(q.x * q.x + q.y * q.y + q.z * q.z);

	if (length < eps)
	{
		return Vec3(q.x, q.y, q.z);
	}
	else
	{
		const f32 halfangle = math_acos(math_clamp(q.w, -1.0f, 1.0f));
		return halfangle * (Vec3(q.x, q.y, q.z) / length);
	}
}

static inline Quat exp(Vec3 v, f32 eps = 1e-8f)
{
	const f32 halfangle = math_squareRoot(v.x * v.x + v.y * v.y + v.z * v.z);

	if (halfangle < eps)
	{
		return normalize(Quat{v.x, v.y, v.z, 1.0f});
	}
	else
	{
		const f32 c = math_cos(halfangle);
		const f32 s = math_sin(halfangle) / halfangle;
		return Quat{s * v.x, s * v.y, s * v.z, c};
	}
}

static inline Vec3 toScaledAngleAxis(Quat q, f32 eps = 1e-8f)
{
	return 2.0f * log(q, eps);
}

static inline Quat fromScaledAngleAxis(Vec3 v, f32 eps = 1e-8f)
{
	return exp(v / 2.0f, eps);
}

static inline float angleBetween(Quat q, Quat p)
{
	Quat diff = abs(q * inverse(p));
	return 2.0f * math_acos(math_clamp(diff.w, -1.0f, 1.0f));
}

static inline Quat between(Vec3 p, Vec3 q)
{
	Vec3 c = cross(p, q);

	return normalize(Quat{c.x, c.y, c.z, math_squareRoot(dot(p, p) * dot(q, q)) + dot(p, q)});
}

static inline Vec3 slerp(Vec3 a, Vec3 b, f32 t)
{
	const f32 a_length = length(a);
	const f32 b_length = length(b);
	const Vec3 a_norm = a / a_length;
	const Vec3 b_norm = b / b_length;
	const f32 dot_a_b = dot(a_norm, b_norm);

	if (1.0f - abs(dot_a_b) < 0.0001f)
	{
		return a;
	}

	const f32 angle = math_acos(dot_a_b);
	const Vec3 axis_of_rotation = cross(a_norm, b_norm);
	const Quat rotation = fromAngleAxis(axis_of_rotation, angle * t);
	const Vec3 result = rotateVec3(rotation, a_norm) * math_lerp(a_length, b_length, t);
	PAW_ASSERT(!isnan(result.x) && !isnan(result.y) && !isnan(result.z));
	return result;
}