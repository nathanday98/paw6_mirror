#include <shared/math.h>

#include <shared/math.inl>
#include <shared/string.h>
#include <shared/string_id.inl>

static void vec2Formatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	Vec2 const* vec = static_cast<Vec2 const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "{ %g, %g }", vec->x, vec->y);
}

static void vec2S32Formatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	Vec2S32 const* vec = static_cast<Vec2S32 const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "{ %d, %d }", vec->x, vec->y);
}

static void vec3Formatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	Vec3 const* vec = static_cast<Vec3 const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "{ %g, %g, %g }", vec->x, vec->y, vec->z);
}

static void vec4Formatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	Vec4 const* vec = static_cast<Vec4 const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "{ %g, %g, %g, %g }", vec->x, vec->y, vec->z, vec->w);
}

static void quatFormatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	Quat const* quat = static_cast<Quat const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "{ %g, %g, %g, %g }", quat->x, quat->y, quat->z, quat->w);
}

static void linearColorFormatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	LinearColor const* color = static_cast<LinearColor const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "{ %g, %g, %g, %g }", color->r, color->g, color->b, color->a);
}

static void srgbColorFormatter(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	SRGBColor const* color = static_cast<SRGBColor const*>(data);
	string_format(
		out_buffer,
		out_buffer_size_bytes,
		"{ %u, %u, %u, %u }",
		static_cast<u32>(color->r),
		static_cast<u32>(color->g),
		static_cast<u32>(color->b),
		static_cast<u32>(color->a));
}

void math_registerFormatters()
{
	string_registerFormatter(PAW_STRING_ID("Vec2"), &vec2Formatter);
	string_registerFormatter(PAW_STRING_ID("Vec2S32"), &vec2S32Formatter);
	string_registerFormatter(PAW_STRING_ID("Vec3"), &vec3Formatter);
	string_registerFormatter(PAW_STRING_ID("Vec4"), &vec4Formatter);
	string_registerFormatter(PAW_STRING_ID("Quat"), &quatFormatter);
	string_registerFormatter(PAW_STRING_ID("LinearColor"), &linearColorFormatter);
	string_registerFormatter(PAW_STRING_ID("SRGBColor"), &srgbColorFormatter);
}

#if PAW_TESTS
#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME math

PAW_TEST(vec2Tests)
{
	const Vec2 test0 = Vec2{1.0f, 2.0f} + Vec2{2.0f, 3.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test0.x == 3.0f && test0.y == 5.0f, true);

	const Vec2 test1 = Vec2{1.0f, 2.0f} - Vec2{2.0f, 4.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test1.x == -1.0f && test1.y == -2.0f, true);

	const Vec2 test2 = Vec2{1.0f, 2.0f} * Vec2{2.0f, 4.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test2.x == 2.0f && test2.y == 8.0f, true);

	const Vec2 test3 = Vec2{1.0f, 4.0f} / Vec2{2.0f, 2.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test3.x == 0.5f && test3.y == 2.0f, true);
}

PAW_TEST(vec3Tests)
{
	const Vec3 test0 = Vec3{1.0f, 2.0f, 3.0f} + Vec3{2.0f, 3.0f, 3.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test0.x == 3.0f && test0.y == 5.0f && test0.z == 6.0f, true);

	const Vec3 test1 = Vec3{1.0f, 2.0f, 3.0f} - Vec3{2.0f, 4.0f, 3.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test1.x == -1.0f && test1.y == -2.0f && test1.z == 0.0f, true);

	const Vec3 test2 = Vec3{1.0f, 2.0f, 3.0f} * Vec3{2.0f, 4.0f, 3.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test2.x == 2.0f && test2.y == 8.0f && test2.z == 9.0f, true);

	const Vec3 test3 = Vec3{1.0f, 4.0f, 3.0f} / Vec3{2.0f, 2.0f, 3.0f};
	PAW_TEST_EXPECT_EQUAL(bool, test3.x == 0.5f && test3.y == 2.0f && test3.z == 1.0f, true);
}

PAW_TEST(mat4Tests)
{
	const Vec3 test0 = Mat4::translate(Vec3{10.0f, 4.0f, 1.0f}).transformPoint(Vec3{2.0f, 3.0f, 1.0f});
	PAW_TEST_EXPECT_EQUAL(bool, test0.x == 12.0f && test0.y == 7.0f && test0.z == 2.0f, true);

	const Vec3 test1 = Mat4::translate(Vec3{10.0f, 4.0f, 1.0f}).transformVector(Vec3{2.0f, 3.0f, 1.0f});
	PAW_TEST_EXPECT_EQUAL(bool, test1.x == 2.0f && test1.y == 3.0f && test1.z == 1.0f, true);
}

#endif