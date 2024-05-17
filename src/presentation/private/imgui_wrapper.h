#pragma once

#include <shared/std.h>
#include <shared/math.h>

#define IM_VEC2_CLASS_EXTRA \
	ImVec2(const Vec2& f)   \
	{                       \
		x = f.x;            \
		y = f.y;            \
	}                       \
	operator Vec2() const   \
	{                       \
		return Vec2(x, y);  \
	}

#define IM_VEC4_CLASS_EXTRA      \
	ImVec4(const Vec4& f)        \
	{                            \
		x = f.x;                 \
		y = f.y;                 \
		z = f.z;                 \
		w = f.w;                 \
	}                            \
	operator Vec4() const        \
	{                            \
		return Vec4(x, y, z, w); \
	}

#ifdef PAW_ENABLE_IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#endif