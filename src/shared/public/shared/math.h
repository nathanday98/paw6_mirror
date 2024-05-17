#pragma once

#include <shared/std.h>

struct Vec2
{
	f32 x = 0.0f;
	f32 y = 0.0f;

	inline Vec2 operator+(const Vec2& rhs) const;
	inline Vec2 operator-(const Vec2& rhs) const;
	inline Vec2 operator*(const Vec2& rhs) const;
	inline Vec2 operator/(const Vec2& rhs) const;
	inline Vec2 operator*(f32 rhs) const;
	inline Vec2 operator/(f32 rhs) const;
	inline Vec2 operator-() const;
	inline void operator+=(const Vec2& rhs);
	inline void operator-=(const Vec2& rhs);
	inline void operator*=(const Vec2& rhs);
	inline void operator/=(const Vec2& rhs);
	inline void operator*=(const f32 rhs);
	inline void operator/=(const f32 rhs);
	inline f32 lengthSquared() const;
	inline f32 length() const;
	inline Vec2 normalized() const;
	inline Vec2 safelyNormalized() const;
	inline Vec2 leftPerp() const;
	inline Vec2 rightPerp() const;
	inline bool isClose(const Vec2& rhs, f32 margin) const;
	inline f32 dot(const Vec2& rhs) const;
	inline f32* getPtr();
	inline const f32* getPtr() const;
	inline Vec2 floored() const;
};

struct Vec3
{
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 z = 0.0f;
};

struct Vec2S32
{
	s32 x = 0;
	s32 y = 0;
};

struct Vec4
{
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 z = 0.0f;
	f32 w = 0.0f;

	inline const f32* getPtr() const;
	inline f32* getPtr();
	inline Vec3 xyz() const;
};

// Row major
struct Mat4
{
	f32 data[4][4] = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
	};

	static inline Mat4 translate(const Vec3& translation);
	static inline Mat4 scale(const Vec3& scale);
	static inline Mat4 ortho(f32 left, f32 top, f32 right, f32 bottom, f32 near, f32 far);
	inline Vec3 transformPoint(const Vec3& point) const;
	inline Vec3 transformVector(const Vec3& vector) const;
	inline Vec4 transformVec4(const Vec4& vec) const;
};

struct Quat
{
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 z = 0.0f;
	f32 w = 1.0f;
};

struct Point3
{
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 z = 0.0f;
};

struct LinearColor
{
	f32 r = 0.0f;
	f32 g = 0.0f;
	f32 b = 0.0f;
	f32 a = 0.0f;
};

struct SRGBColor
{
	u8 r = 0;
	u8 g = 0;
	u8 b = 0;
	u8 a = 0;
};

void math_registerFormatters();

#if PAW_TESTS
void math_tests();
#endif