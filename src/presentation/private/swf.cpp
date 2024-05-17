#include "swf.h"

#include <shared/slice.h>
#include <shared/string.h>
#include <shared/log.h>
#include <shared/allocator.h>
#include <shared/memory.h>
#include <shared/defer.h>
#include <shared/math.inl>
#include <shared/memory.h>

#include <stdio.h>

#if 1
#include <tesselator.h>
#endif
#include <stb_truetype.h>
#include <imgui.h>

#include <vector>
#include <algorithm>

#include "new_renderer.h"
#include "debug_draw.h"

#undef PAW_LOG_INFO
#define PAW_LOG_INFO(fmt, ...)

static f32 fromTwips(s32 value)
{
	return (f32)value / 20.0f;
}

struct Rect
{
	f32 x_min = 0.0f;
	f32 y_min = 0.0f;
	f32 x_max = 0.0f;
	f32 y_max = 0.0f;
};

struct Color
{
	byte r;
	byte g;
	byte b;
	byte a;

	LinearColor toLinear() const
	{
		const f32 d = 1.0f / 255.0f;
		return LinearColor{(f32)r * d, (f32)g * d, (f32)b * d, (f32)a * d};
	}

	u32 toU32() const
	{
		u32 result = r;
		result <<= 8;
		result |= g;
		result <<= 8;
		result |= b;
		result <<= 8;
		result |= a;
		return result;
	}
};

enum SpreadMode
{
	SpreadMode_Pad = 0,
	SpreadMode_Reflect = 1,
	SpreadMode_Repeat = 2,
};

enum InterpolationMode
{
	InterpolationMode_NormalRGB = 0,
	InterpolationMode_LinearRGB = 1,
};

struct Matrix
{
	f32 translation_x;
	f32 translation_y;
	f32 rotate_skew0 = 0.0f;
	f32 rotate_skew1 = 0.0f;
	f32 scale_x = 1.0f;
	f32 scale_y = 1.0f;
};

static void invertMatrix(Matrix* mat)
{
	Matrix old = *mat;
	float det = old.scale_x * old.scale_y - old.rotate_skew0 * old.rotate_skew1;
	mat->scale_x = old.scale_y / det;
	mat->rotate_skew0 = old.rotate_skew0 / -det;
	mat->rotate_skew1 = old.rotate_skew1 / -det;
	mat->scale_y = old.scale_x / det;
	mat->translation_x = (old.scale_y * old.translation_x - old.rotate_skew1 * old.translation_y) / -det;
	mat->translation_y = (old.rotate_skew0 * old.translation_x - old.scale_x * old.translation_y) / det;
}

enum class TagType
{
	End = 0,
	ShowFrame = 1,
	DefineShape = 2,
	SetBackgroundColor = 9,
	DoAction = 12,
	DefineShape2 = 22,
	PlaceObject2 = 26,
	RemoveObject2 = 28,
	DefineShape3 = 32,
	DefineSprite = 39,
	FrameLabel = 43,
	DefineMorphShape = 46,
	ExportAssets = 56,
	FileAttributes = 69,
	PlaceObject3 = 70,
	DefineShape4 = 83,
	DefineMorphShape2 = 84,
	Max,
};

static char const* getTagTypeName(TagType type)
{
	switch (type)
	{
		case TagType::End:
			return "End";
			break;
		case TagType::ShowFrame:
			return "ShowFrame";
			break;
		case TagType::DefineShape:
			return "DefineShape";
			break;
		case TagType::SetBackgroundColor:
			return "SetBackgroundColor";
			break;
		case TagType::DoAction:
			return "DoAction";
			break;
		case TagType::DefineShape2:
			return "DefineShape2";
			break;
		case TagType::PlaceObject2:
			return "PlaceObject2";
			break;
		case TagType::RemoveObject2:
			return "RemoveObject2";
			break;
		case TagType::DefineShape3:
			return "DefineShape3";
			break;
		case TagType::DefineSprite:
			return "DefineSprite";
			break;
		case TagType::FrameLabel:
			return "FrameLabel";
			break;
		case TagType::DefineMorphShape:
			return "DefineMorphShape";
			break;
		case TagType::ExportAssets:
			return "ExportAssets";
			break;
		case TagType::FileAttributes:
			return "FileAttributes";
			break;
		case TagType::PlaceObject3:
			return "PlaceObject3";
			break;
		case TagType::DefineShape4:
			return "DefineShape4";
			break;
		case TagType::DefineMorphShape2:
			return "DefineMorphShape2";
			break;
		case TagType::Max:
			PAW_UNREACHABLE;
			break;
	}

	return "Unknown";
};

struct TagHeader
{
	TagType type;
	usize size;
};

enum class FillStyleType
{
	Solid = 0x00,
	LinearGradient = 0x10,
	RadialGradient = 0x12,
	FocalGradient = 0x13,
	RepeatingBitmap = 0x40,
	ClippedBitmap = 0x41,
	NonSmoothedRepeatingBitmap = 0x42,
	NonSmoothedClippedBitmap = 0x43,
};

static char const* getFillStyleTypeName(FillStyleType type)
{
	switch (type)
	{
		case FillStyleType::Solid:
			return "Solid";
		case FillStyleType::LinearGradient:
			return "LinearGradient";
		case FillStyleType::RadialGradient:
			return "RadialGradient";
		case FillStyleType::FocalGradient:
			return "FocalGradient";
		case FillStyleType::RepeatingBitmap:
			return "RepeatingBitmap";
		case FillStyleType::ClippedBitmap:
			return "ClippedBitmap";
		case FillStyleType::NonSmoothedRepeatingBitmap:
			return "NonSmoothedRepeatingBitmap";
		case FillStyleType::NonSmoothedClippedBitmap:
			return "NonSmoothedClippedBitmap";
	}
	return "Unknown";
}

struct CachedGradient
{
	u32 colors[15];
	u32 ratios[15];
	u32 control_point_count;
};

struct FillStyle
{
	FillStyleType type;
	union
	{
		u32 solid_srgb;
		struct
		{
			Matrix matrix;
			u32 gradient_cache_index;
			SpreadMode spread;
			InterpolationMode interpolation;
			f32 focal;
		} gradient;
	};
};

struct Parser
{
	Slice<byte> data;
	byte* walker;
	usize bit_offset;

	void init(Slice<byte> const& in_data)
	{
		data = in_data;
		walker = data.ptr;
		bit_offset = 0;
	}

	void alignToNextByte()
	{
		if (bit_offset != 0)
		{
			bit_offset = 0;
			walker++;
		}
	}

	u8 readU8()
	{
		alignToNextByte();
		u8* ptr = (u8*)walker;
		walker += 1;
		return *ptr;
	}

	u16 readU16()
	{
		alignToNextByte();
		u16* ptr = (u16*)walker;
		walker += 2;
		return *ptr;
	}

	u32 readU32()
	{
		alignToNextByte();
		u32* ptr = (u32*)walker;
		walker += 4;
		return *ptr;
	}

	s8 readS8()
	{
		alignToNextByte();
		s8* ptr = (s8*)walker;
		walker += 1;
		return *ptr;
	}

	s16 readS16()
	{
		alignToNextByte();
		s16* ptr = (s16*)walker;
		walker += 2;
		return *ptr;
	}

	s32 readS32()
	{
		alignToNextByte();
		s32* ptr = (s32*)walker;
		walker += 4;
		return *ptr;
	}

	f32 readFixed88()
	{
		alignToNextByte();
		s16 raw = readS16();
		return (f32)raw / 256.0f;
	}

	f32 readFixed1616()
	{
		alignToNextByte();
		s32 raw = readS32();
		return (f32)raw / 65536.0f;
	}

	u32 readUN(usize n)
	{
		u32 result = 0;
		for (usize i = 0; i < n; i++)
		{
			u32 bit = ((*walker) >> (7 - bit_offset)) & 1;
			result |= bit;
			result <<= 1;

			bit_offset++;
			if (bit_offset == 8)
			{
				bit_offset = 0;
				walker++;
			}
		}
		result >>= 1;

		return result;
	}

	s32 readSN(usize n)
	{
		u32 value = readUN(n);
		s32 result = 0;

		u8 bit = (u8)(value >> (n - 1));
		result = bit * 0xFFFFFFFF;
		result <<= n;

		result |= value;
		return result;
	}

	f32 readFixed8N(usize n)
	{
		s32 value = readSN(n);
		return (f32)value / 256.0f;
	}

	f32 readFixed16N(usize n)
	{
		s32 value = readSN(n);
		return (f32)value / 65536.0f;
	}

	Rect readRect()
	{
		alignToNextByte();
		usize bit_count = (usize)readUN(5);
		Rect result{};
		result.x_min = fromTwips(readSN(bit_count));
		result.x_max = fromTwips(readSN(bit_count));
		result.y_min = fromTwips(readSN(bit_count));
		result.y_max = fromTwips(readSN(bit_count));
		return result;
	}

	u32 readColorRGB()
	{
		u32 result = readU8();
		result <<= 8;
		result |= readU8();
		result <<= 8;
		result |= readU8();
		result <<= 8;
		result |= 255;
		return result;
	}

	u32 readColorRGBA()
	{
		u32 result = readU8();
		result <<= 8;
		result |= readU8();
		result <<= 8;
		result |= readU8();
		result <<= 8;
		result |= readU8();
		return result;
	}

	UTF8StringView readString()
	{
		alignToNextByte();
		byte const* start = walker;
		u8 b = readU8();
		usize length = 0;
		while (b != 0)
		{
			b = readU8();
			length++;
		}
		return UTF8StringView{.ptr = reinterpret_cast<char const*>(start), .size_bytes = length};
	}

	TagHeader readTagHeader()
	{
		TagHeader result{};
		u16 tag_code_and_length = readU16();
		result.type = (TagType)(tag_code_and_length >> 6);
		result.size = tag_code_and_length & 0b111111;
		if (result.size == 0x3F)
		{
			result.size = readU32();
		}
		return result;
	}

	Matrix readMatrix()
	{
		Matrix result{};
		bool const has_scale = readUN(1) == 1;
		if (has_scale)
		{
			const u32 scale_bits = readUN(5);
			result.scale_x = readFixed16N(scale_bits);
			result.scale_y = readFixed16N(scale_bits);
		}

		bool const has_rotate = readUN(1) == 1;
		if (has_rotate)
		{
			const u32 rotate_bits = readUN(5);
			result.rotate_skew0 = readFixed16N(rotate_bits);
			result.rotate_skew1 = readFixed16N(rotate_bits);
		}

		const u32 translate_bits = readUN(5);
		result.translation_x = fromTwips(readSN(translate_bits));
		result.translation_y = fromTwips(readSN(translate_bits));

		return result;
	}
};

enum class EdgeType
{
	UnKnown,
	Straight,
	Curve,
	InvertedCurve,
	// End,
};

struct Edge
{
	EdgeType type;
	Vec2S32 start;
	Vec2S32 control;
	Vec2S32 end;
};

struct Line
{
	Vec2S32 start;
	Vec2S32 end;
};

struct LineF32
{
	Vec2 start;
	Vec2 end;
};

static f32 signed2DTriArea(Vec2 a, Vec2 b, Vec2 c)
{
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

static Vec2 fromTwips(Vec2S32 x)
{
	return Vec2{fromTwips(x.x), fromTwips(x.y)};
}

struct Curve
{
	Vec2S32 start;
	Vec2S32 control;
	Vec2S32 end;

	f32 calcArea() const
	{
		f32 result0 = math_abs(signed2DTriArea(fromTwips(start), fromTwips(control), fromTwips(end)));
		f32 result = result0 / 2.0f;
		// PAW_ASSERT(result > 0);
		return result;
		// return ((start.x * (control.y - end.y)) + (control.x * (end.y - start.y)) + (end.x * (start.y - control.y)))
		// / 	2;
	}
};

struct CurveF32
{
	Vec2 start;
	Vec2 control;
	Vec2 end;

	CurveF32(Curve c)
	{
		start = fromTwips(c.start);
		control = fromTwips(c.control);
		end = fromTwips(c.end);
	}

	f32 calcArea() const
	{
		f32 result0 = math_abs(signed2DTriArea(start, control, end));
		f32 result = result0 / 2.0f;
		// PAW_ASSERT(result > 0);
		return result;
		// return ((start.x * (control.y - end.y)) + (control.x * (end.y - start.y)) + (end.x * (start.y - control.y)))
		// / 	2;
	}
};

static bool curveTrisIntersectF32(Curve const& a, Curve const& b)
{
	bool lines_intersect = false;

	CurveF32 a_f32{a};
	CurveF32 b_f32{b};

	{
		Slice<LineF32 const> const lines_a{
			{a_f32.start, a_f32.control}, {a_f32.control, a_f32.end}, {a_f32.end, a_f32.start}};
		Slice<LineF32 const> const lines_b{
			{b_f32.start, b_f32.control}, {b_f32.control, b_f32.end}, {b_f32.end, b_f32.start}};

		for (usize line_index = 0; line_index < lines_a.size; ++line_index)
		{
			LineF32 const& la = lines_a[line_index];
			for (usize i = 0; i < lines_b.size; ++i)
			{
				LineF32 const& lb = lines_b[i];
				// Sign of areas correspond to which side of ab points c and d are
				f32 a1 = signed2DTriArea(la.start, la.end, lb.end);	  // Compute winding of abd (+ or -)
				f32 a2 = signed2DTriArea(la.start, la.end, lb.start); // To intersect, must have sign opposite of a1
				// PAW_LOG_INFO("F32: A1: %g", a1);
				// PAW_LOG_INFO("F32: A2: %g", a2);
				// If c and d are on different sides of ab, areas have different signs
				if (a1 * a2 < 0.0f)
				{
					// Compute signs for a and b with respect to segment cd
					f32 a3 = signed2DTriArea(lb.start, lb.end, la.start); // Compute winding of cda (+ or -)
					// Since area is constant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
					// f32 a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
					f32 a4 = a3 + a2 - a1;
					// Points a and b on different sides of cd if areas have different signs
					if (a3 * a4 < 0.0f)
					{
						// Segments intersect. Find intersection point along L(t) = a + t * (b - a).
						// Given height h1 of an over cd and height h2 of b over cd,
						// t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - b*h2/2) = a3 / (a3 - a4),
						// where b (the base of the triangles cda and cdb, i.e., the length
						// of cd) cancels out.
						lines_intersect = true;
						break;
					}
				}
			}
			if (lines_intersect)
			{
				break;
			}
		}
	}

	usize intersecting_point_count = 0;

	{
		Slice<Vec2 const> points{{a_f32.start, a_f32.control, a_f32.end}};
		CurveF32 const& curve = b;

		for (const Vec2 point : points)
		{

			const f32 s = signed2DTriArea(curve.start, point, curve.end);
			const f32 t = signed2DTriArea(curve.control, point, curve.start);

			if ((s < 0) != (t < 0) && s != 0 && t != 0)
				break;

			const f32 d = signed2DTriArea(curve.end, point, curve.control);
			if (d == 0 || (d < 0) == (s + t <= 0))
			{
				intersecting_point_count++;
			}
		}
	}

	if (intersecting_point_count == 1 && (a.end == b.start || b.end == a.start))
	{
		return false;
	}

	intersecting_point_count = 0;

	{
		Slice<Vec2 const> points{{b_f32.start, b_f32.control, b_f32.end}};
		CurveF32 const& curve = a;

		for (const Vec2 point : points)
		{
			const f32 s = signed2DTriArea(curve.start, point, curve.end);
			const f32 t = signed2DTriArea(curve.control, point, curve.start);

			if ((s < 0) != (t < 0) && s != 0 && t != 0)
				break;

			const f32 d = signed2DTriArea(curve.end, point, curve.control);

			if (d == 0 || (d < 0) == (s + t <= 0))
			{
				intersecting_point_count++;
			}
		}
	}

	if (intersecting_point_count == 1 && (a.end == b.start || b.end == a.start))
	{
		return false;
	}

	return lines_intersect || intersecting_point_count > 0;
}

struct EdgeRun
{
	usize start_index;
	usize end_index;
};

struct EdgeList
{
	Slice<Edge> edges;
	usize edge_count;
	Slice<EdgeRun> contours;
	usize run_count;

	void push(Edge edge, Allocator& allocator)
	{
		if (edges.ptr == nullptr)
		{
			edges = allocator.alloc<Edge>(2048);
			memset(edges.ptr, 0, edges.calc_total_size_in_bytes());
			edge_count = 0;
		}

		PAW_ASSERT(edge_count < edges.size);

		edges[edge_count++] = edge;
	}

	void insert(Edge edge, usize index, Allocator& allocator)
	{
		PAW_ASSERT(edge_count < edges.size);
		PAW_UNUSED_ARG(allocator);
		for (usize i = edge_count; i > index; i--)
		{
			edges[i] = edges[i - 1];
		}

		edges[index] = edge;
		edge_count++;
	}
};

static void readStyleArray(std::vector<FillStyle>& out_fill_styles, std::vector<std::vector<std::vector<Edge>>>& out_edge_lists, TagHeader const& tag, Parser& parser, std::vector<CachedGradient>& gradient_cache, bool debug)
{
	out_edge_lists.push_back({}); // 0 style is empty

	out_fill_styles.push_back(FillStyle{.type = FillStyleType::Solid, .solid_srgb = 0});

	usize local_fill_style_count = static_cast<usize>(parser.readU8());
	if (local_fill_style_count == 0xFF)
	{
		local_fill_style_count = static_cast<usize>(parser.readU16());
	}

	for (usize fill_style_index = 0; fill_style_index < local_fill_style_count; fill_style_index++)
	{
		const FillStyleType type = static_cast<FillStyleType>(parser.readU8());
		FillStyle style{};
		style.type = type;
		switch (type)
		{
			case FillStyleType::Solid:
			{
				if (tag.type >= TagType::DefineShape3)
				{
					style.solid_srgb = parser.readColorRGBA();
				}
				else
				{
					style.solid_srgb = parser.readColorRGB();
					if (!debug)
					{
						PAW_LOG_INFO(
							"%s Fill %llu: 0x%08X",
							getFillStyleTypeName(type),
							fill_style_index + 1,
							style.solid_srgb);
					}
				}
			}
			break;

			case FillStyleType::RadialGradient:
			case FillStyleType::LinearGradient:
			case FillStyleType::FocalGradient:
			{
				style.gradient.matrix = parser.readMatrix();
				parser.alignToNextByte();
				style.gradient.spread = (SpreadMode)parser.readUN(2);
				style.gradient.interpolation = (InterpolationMode)parser.readUN(2);

				CachedGradient cached_gradient{};

				cached_gradient.control_point_count = parser.readUN(4);

				for (u32 i = 0; i < cached_gradient.control_point_count; i++)
				{
					const u8 ratio = parser.readU8();
					u32 color_srgb;
					if (tag.type >= TagType::DefineShape3)
					{
						color_srgb = parser.readColorRGBA();
					}
					else
					{
						color_srgb = parser.readColorRGB();
					}

					cached_gradient.colors[i] = color_srgb;
					cached_gradient.ratios[i] = ratio;
				}

				if (type == FillStyleType::FocalGradient)
				{
					style.gradient.focal = parser.readFixed88();
				}
				else
				{
					style.gradient.focal = 0.0f;
				}

				if (cached_gradient.control_point_count == 2 && cached_gradient.ratios[1] < 255)
				{
					cached_gradient.ratios[2] = 255;
					cached_gradient.colors[2] = cached_gradient.colors[1];
					cached_gradient.control_point_count = 3;
				}

				bool exists = false;
				u32 gradient_cache_index = 0;
				for (u32 i = 0; i < (u32)gradient_cache.size(); ++i)
				{
					CachedGradient const& cache_entry = gradient_cache[i];
					if (cache_entry.control_point_count == cached_gradient.control_point_count)
					{
						int result = memcmp(cached_gradient.colors, cache_entry.colors, sizeof(cached_gradient.colors[0]) * cached_gradient.control_point_count);
						result |= memcmp(cached_gradient.ratios, cache_entry.ratios, sizeof(cached_gradient.ratios[0]) * cached_gradient.control_point_count);
						if (result == 0)
						{
							gradient_cache_index = i;
							exists = true;
							break;
						}
					}
				}

				if (!exists)
				{
					gradient_cache_index = (u32)gradient_cache.size();
					gradient_cache.push_back(cached_gradient);
				}

				style.gradient.gradient_cache_index = gradient_cache_index;

				if (!debug)
				{
					PAW_LOG_INFO("%s Fill %llu: %d control points", getFillStyleTypeName(type), fill_style_index + 1, gradient_cache[style.gradient.gradient_cache_index].control_point_count);
				}
			}
			break;

			case FillStyleType::RepeatingBitmap:
			case FillStyleType::ClippedBitmap:
			case FillStyleType::NonSmoothedRepeatingBitmap:
			case FillStyleType::NonSmoothedClippedBitmap:
				PAW_UNREACHABLE;
			default:
				PAW_UNREACHABLE;
		}

		out_fill_styles.push_back(style);
		out_edge_lists.push_back({});
	}

	usize line_style_count = static_cast<usize>(parser.readU8());
	if (line_style_count == 0xFF)
	{
		line_style_count = static_cast<usize>(parser.readU16());
	}

	PAW_ASSERT(line_style_count == 0);
}

SwfStore loadSwfFromMemory(Slice<byte> const& data, Allocator& allocator, bool debug, Allocator& persistent_allocator)
{
	Parser parser{};
	parser.init(data);

	u8 compression = parser.readU8();
	PAW_ASSERT(compression == 'F');
	PAW_ASSERT_UNUSED(compression);
	parser.readU8(); // W
	parser.readU8(); // S

	const u8 version_number = parser.readU8();
	PAW_ASSERT(version_number == 8);
	PAW_ASSERT_UNUSED(version_number);

	const usize uncompressed_size = static_cast<usize>(parser.readU32());
	(void)uncompressed_size;

	const Rect frame_size = parser.readRect();

	const f32 frame_rate = parser.readFixed88();
	(void)frame_rate;

	const usize frame_count = static_cast<usize>(parser.readU16());
	(void)frame_count;

	/*Slice<FillStyle> fill_styles = allocator.alloc<FillStyle>(100);
	fill_styles[0] = FillStyle{.type = FillStyleType::Solid, .solid = Color{}};
	usize fill_style_count = 1;*/

	/*Slice<EdgeList> edge_lists = allocator.alloc<EdgeList>(fill_styles.size);
	memset(edge_lists.ptr, 0, edge_lists.calc_total_size_in_bytes());*/

	std::vector<SwfVertex> final_vertices;
	std::vector<u32> final_indices;
	Slice<SwfShape> final_shapes = persistent_allocator.alloc<SwfShape>(256);
	usize final_shape_count = 0;

	std::vector<Vec2> scratch_vertices{};
	scratch_vertices.reserve(100);

	std::vector<CachedGradient> gradient_cache{};
	gradient_cache.push_back({});

	TagHeader tag{};
	do
	{
		tag = parser.readTagHeader();
		byte* tag_start = parser.walker;
		if (!debug)
		{
			PAW_LOG_INFO("Tag: %s", getTagTypeName(tag.type));
		}

		switch (tag.type)
		{
			case TagType::DefineShape:
			case TagType::DefineShape2:
			case TagType::DefineShape3:
			case TagType::DefineShape4:
			{
				std::vector<std::vector<std::vector<Edge>>> edge_lists;

				std::vector<FillStyle> fill_styles;

				u32 fill_style_offset = 0;

				const u16 id = parser.readU16();
				(void)id;

				Rect bounds = parser.readRect();

				if (tag.type == TagType::DefineShape4)
				{
					Rect edge_bounds = parser.readRect();
					(void)edge_bounds;
					// bounds = edge_bounds;
					// parser.readUN(5); // Reserved

					// parser.readUN(1); // UseFillWindingRule
					// parser.readUN(1); // USeNonScalingStrokes
					// parser.readUN(1); // UsesScalingStrokes
					parser.readU8();
				}
				if (!debug)
				{
					PAW_LOG_INFO("=========================== %s %hu ===========================", getTagTypeName(tag.type), id);
				}

				fill_style_offset = (u32)fill_styles.size();
				readStyleArray(fill_styles, edge_lists, tag, parser, gradient_cache, debug);

				u32 fill_bit_count = parser.readUN(4);
				u32 line_bit_count = parser.readUN(4);

				u32 current_fill_style0 = 0;
				u32 current_fill_style1 = 0;

				Vec2S32 vertex_pos = {0, 0};

				usize current_edge_list_index0 = 0;
				usize current_edge_list_index1 = 0;

				while (true)
				{
					bool const is_edge = static_cast<bool>(parser.readUN(1));
					const u32 type_flags = parser.readUN(5);
					if (!is_edge)
					{
						if (type_flags == 0)
						{
							if (!debug)
							{
								PAW_LOG_INFO("End shape");
							}
							break;
						}

						if (!debug)
						{
							PAW_LOG_INFO("Style change");
						}
						const u8 state_new_styles = (type_flags >> 4) & 1;
						const u8 state_line_style = (type_flags >> 3) & 1;
						const u8 state_fill_style1 = (type_flags >> 2) & 1;
						const u8 state_fill_style0 = (type_flags >> 1) & 1;
						const u8 state_move_to = (type_flags >> 0) & 1;

						// vertex_pos = Vec2S32{0, 0};

						if (state_move_to)
						{
							const usize move_bit_count = static_cast<usize>(parser.readUN(5));
							const s32 move_delta_x = parser.readSN(move_bit_count);
							const s32 move_delta_y = parser.readSN(move_bit_count);
							if (!debug)
							{
								PAW_LOG_INFO("Move To delta: %d, %d", move_delta_x, move_delta_y);
							}
							vertex_pos.x = move_delta_x;
							vertex_pos.y = move_delta_y;
						}

						if (state_fill_style0)
						{
							u32 fill_style = parser.readUN(fill_bit_count);
							current_fill_style0 = fill_style;
							if (!debug)
							{
								PAW_LOG_INFO("Change fill0 to %d", fill_style);
							}
						}

						if (state_fill_style1)
						{
							u32 fill_style = parser.readUN(fill_bit_count);
							current_fill_style1 = fill_style;
							if (!debug)
							{
								PAW_LOG_INFO("Change fill1 to %d", fill_style);
							}
						}

						if (state_line_style)
						{
							/*u32 line_style =*/parser.readUN(line_bit_count);
							// PAW_LOG_INFO("Change line to %d", line_style);
						}

						if (state_new_styles)
						{
							fill_style_offset = (u32)fill_styles.size();
							readStyleArray(fill_styles, edge_lists, tag, parser, gradient_cache, debug);
							fill_bit_count = parser.readUN(4);
							line_bit_count = parser.readUN(4);
						}

						current_edge_list_index0 = edge_lists[current_fill_style0 + fill_style_offset].size();
						edge_lists[current_fill_style0 + fill_style_offset].push_back({});

						current_edge_list_index1 = edge_lists[current_fill_style1 + fill_style_offset].size();
						edge_lists[current_fill_style1 + fill_style_offset].push_back({});
					}
					else
					{
						bool const is_straight = static_cast<bool>((type_flags >> 4) & 1);
						const usize num_bits = (usize)(type_flags & 0b1111);

						if (is_straight)
						{
							bool const general_line = static_cast<bool>(parser.readUN(1));
							const Vec2S32 start = vertex_pos;
							if (general_line)
							{
								s32 delta_x = parser.readSN(num_bits + 2);
								s32 delta_y = parser.readSN(num_bits + 2);
								vertex_pos.x += delta_x;
								vertex_pos.y += delta_y;
							}
							else
							{
								bool const is_vertical = static_cast<bool>(parser.readUN(1));
								const s32 length = parser.readSN(num_bits + 2);
								if (is_vertical)
								{
									vertex_pos.y += length;
								}
								else
								{
									vertex_pos.x += length;
								}
							}

							// scratch_vertices[scratch_vertex_count++] = vertex_pos;
							const Vec2S32 end = vertex_pos;
							if (!debug)
							{
								PAW_LOG_INFO("Straight: %p{Vec2S32}, %p{Vec2S32}", &start, &end);
							}
							if (current_fill_style0 > 0)
							{
								edge_lists[current_fill_style0 + fill_style_offset][current_edge_list_index0].push_back(
									Edge{
										.type = EdgeType::Straight,
										.start = start,
										.end = end,
									});
								if (debug)
								{
									// debugDrawLine2D(fromTwips(start), fromTwips(end), fill_styles[current_fill_style0].solid.toLinear(), 1.0f);
								}
							}

							if (current_fill_style1 > 0)
							{
								edge_lists[current_fill_style1 + fill_style_offset][current_edge_list_index1].push_back(
									Edge{
										.type = EdgeType::Straight,
										.start = start,
										.end = end,
									});
								if (debug)
								{
									// debugDrawLine2D(fromTwips(start), fromTwips(end), fill_styles[current_fill_style1].solid.toLinear(), 1.0f);
								}
							}
						}
						else
						{
							// PAW_LOG_INFO("Curved edge");
							const s32 control_delta_x = parser.readSN(num_bits + 2);
							const s32 control_delta_y = parser.readSN(num_bits + 2);
							const s32 anchor_delta_x = parser.readSN(num_bits + 2);
							const s32 anchor_delta_y = parser.readSN(num_bits + 2);

							Vec2S32 start = vertex_pos;

							vertex_pos.x += control_delta_x;
							vertex_pos.y += control_delta_y;

							Vec2S32 control = vertex_pos;

							vertex_pos.x += anchor_delta_x;
							vertex_pos.y += anchor_delta_y;

							Vec2S32 end = vertex_pos;

							Vec2S32 a = control - start;
							Vec2S32 b = end - start;

							s32 winding = a.x * b.y - b.x * a.y;
							const EdgeType type = winding > 0 ? EdgeType::Curve : EdgeType::InvertedCurve;
							const EdgeType inv_type = winding > 0 ? EdgeType::InvertedCurve : EdgeType::Curve;
							if (current_fill_style0 > 0)
							{
								if (debug)
								{
									/*debugDrawLine2D(fromTwips(start), fromTwips(end), fill_styles[current_fill_style0].solid.toLinear(), 1.0f);
									debugDrawLine2D(fromTwips(start), fromTwips(control), fill_styles[current_fill_style0].solid.toLinear(), 1.0f);
									debugDrawLine2D(fromTwips(control), fromTwips(end), fill_styles[current_fill_style0].solid.toLinear(), 1.0f);*/
								}

								edge_lists[current_fill_style0 + fill_style_offset][current_edge_list_index0].push_back(
									Edge{
										.type = inv_type,
										.start = start,
										.control = control,
										.end = end,
									});
							}

							if (current_fill_style1 > 0)
							{
								if (debug)
								{
									/*debugDrawLine2D(fromTwips(start), fromTwips(end), fill_styles[current_fill_style1].solid.toLinear(), 1.0f);
									debugDrawLine2D(fromTwips(start), fromTwips(control), fill_styles[current_fill_style1].solid.toLinear(), 1.0f);
									debugDrawLine2D(fromTwips(control), fromTwips(end), fill_styles[current_fill_style1].solid.toLinear(), 1.0f);*/
								}
								edge_lists[current_fill_style1 + fill_style_offset][current_edge_list_index1].push_back(
									Edge{
										.type = type,
										.start = start,
										.control = control,
										.end = end,
									});
							}

							if (!debug)
							{
								PAW_LOG_INFO(
									"%s Curve: %p{Vec2S32}, %p{Vec2S32}, %p{Vec2S32}",
									winding > 0.0f ? "" : "Inverted",
									&start,
									&control,
									&end);
							}
							// PAW_LOG_INFO("Control delta: %.3f, %.3f", control_delta_x, control_delta_y);
							// PAW_LOG_INFO("Anchor delta: %.3f, %.3f", anchor_delta_x, anchor_delta_y);
						}
					}
				}

#if 1
				// Reorder all paths to create properly ordered closed paths
				usize open_path_count = 0;
				do
				{
					open_path_count = 0;

					for (std::vector<std::vector<Edge>>& style : edge_lists)
					{
						// std::sort(style.begin(), style.end(), [](const std::vector<Edge>& a, const std::vector<Edge>& b) -> bool
						//		  {
						//	if (a.size() == 0 || b.size() == 0)
						//	{
						//		return false;
						//	}
						//	return a[a.size() - 1].end == b[0].start; });

						for (int edge_list_index = (int)style.size() - 1; edge_list_index >= 0; edge_list_index--)
						{
							std::vector<Edge>& edges = style[edge_list_index];
							if (edges.size() == 0)
							{
								continue;
							}

							if (edges.size() > 1 && edges[0].start != edges[edges.size() - 1].end)
							{
								open_path_count++;
							}

							for (usize check_edge_list_index = style.size() - 1; check_edge_list_index > 0; check_edge_list_index--)
							{
								std::vector<Edge>& check_edges = style[check_edge_list_index];
								if (edge_list_index == (int)check_edge_list_index || check_edges.size() == 0)
								{
									continue;
								}

								if (edges[0].start == check_edges[check_edges.size() - 1].end)
								{
									check_edges.insert(check_edges.end(), edges.begin(), edges.end());
									style.erase(style.begin() + edge_list_index);
									if (!debug)
									{
										PAW_LOG_INFO("Moved list %llu into end of %llu", edge_list_index, check_edge_list_index);
									}
									break;
								}

								if (edges[edges.size() - 1].end == check_edges[0].start)
								{
									check_edges.insert(check_edges.begin(), edges.begin(), edges.end());
									style.erase(style.begin() + edge_list_index);
									if (!debug)
									{
										PAW_LOG_INFO("Moved list %llu into start of %llu", edge_list_index, check_edge_list_index);
									}
									break;
								}

								if (edges[0].start == check_edges[0].start)
								{
									for (Edge& edge : edges)
									{
										Vec2S32 temp = edge.start;
										edge.start = edge.end;
										edge.end = temp;
									}

									check_edges.insert(check_edges.begin(), edges.rbegin(), edges.rend());
									style.erase(style.begin() + edge_list_index);
									if (!debug)
									{
										PAW_LOG_INFO("Moved reversed list %llu into start of %llu", edge_list_index, check_edge_list_index);
									}
									break;
								}

								if (edges[edges.size() - 1].end == check_edges[check_edges.size() - 1].end)
								{
									for (Edge& edge : edges)
									{
										Vec2S32 temp = edge.start;
										edge.start = edge.end;
										edge.end = temp;
									}

									check_edges.insert(check_edges.end(), edges.rbegin(), edges.rend());
									style.erase(style.begin() + edge_list_index);
									if (!debug)
									{
										PAW_LOG_INFO("Moved reversed list %llu into start of %llu", edge_list_index, check_edge_list_index);
									}
									break;
								}
							}
						}
					}
				} while (open_path_count > 0);
#endif

				{
					// Split curves with biggest areas on intersections
					for (std::vector<std::vector<Edge>>& style : edge_lists)
					{
						for (std::vector<Edge>& edges : style)
						{
							if (edges.size() == 0)
							{
								continue;
							}

							/*Slice<Edge>& edges = list.edges;
							usize& edge_count = list.edge_count;*/
							usize intersection_count = 0;
							do
							{
								intersection_count = 0;
								for (usize edge_index = 0; edge_index < edges.size(); edge_index++)
								{
									Edge& edge = edges[edge_index];
									if (edge.type != EdgeType::Curve)
									{
										continue;
									}

									PAW_ASSERT(edge.type != EdgeType::Straight);

									// bool broke = false;
									for (std::vector<Edge>& inv_edges : style)
									{
										for (usize inv_curve_index = 0; inv_curve_index < inv_edges.size(); inv_curve_index++)
										{
											Edge& inv_edge = inv_edges[inv_curve_index];
											if (inv_edge.type != EdgeType::InvertedCurve)
											{
												continue;
											}

											const Curve edge_curve = {edge.start, edge.control, edge.end};
											const Curve inv_edge_curve = {inv_edge.start, inv_edge.control, inv_edge.end};
											bool const f32_intersect = curveTrisIntersectF32(edge_curve, inv_edge_curve);
											if (f32_intersect)
											{
												bool const same_index = inv_curve_index == edge_index && inv_edges.data() == edges.data();
												bool const not_inverted = inv_edge.type != EdgeType::InvertedCurve;
												// const bool next_index = inv_curve_index == edge_index + 1;
												// const bool prev_index = inv_curve_index == edge_index - 1;
												if (same_index || not_inverted)
												{
													continue;
												}

												intersection_count++;

												const f32 edge_area = edge_curve.calcArea();
												const f32 inv_edge_area = inv_edge_curve.calcArea();

												PAW_ASSERT(edge_area > 0.0f || inv_edge_area > 0.0f);

												const Edge curve = edge_area > inv_edge_area ? edge : inv_edge;

												const Vec2S32 mid_left = math_midpoint(curve.start, curve.control);
												const Vec2S32 mid_right = math_midpoint(curve.control, curve.end);
												const Vec2S32 mid_point = math_midpoint(mid_left, mid_right);

												if (edge_area > inv_edge_area)
												{
													edge.start = curve.start;
													edge.control = mid_left;
													edge.end = mid_point;
													edges.insert(edges.begin() + edge_index + 1, {curve.type, mid_point, mid_right, curve.end});
												}
												else
												{
													inv_edge.start = curve.start;
													inv_edge.control = mid_left;
													inv_edge.end = mid_point;
													inv_edges.insert(inv_edges.begin() + inv_curve_index + 1, {curve.type, mid_point, mid_right, curve.end});
												}

												PAW_ASSERT(curve.type != EdgeType::Straight);
												PAW_ASSERT(curve.type != EdgeType::UnKnown);

												break;
											}
										}
									}
								}
							} while (intersection_count > 0);
						}
					}
				}

#if 0

	if (debug)
	{
		if (ImGui::Begin("Swf"))
		{
			for (usize style_index = 0; style_index < edge_lists.size(); style_index++)
			{
				const std::vector<std::vector<Edge>>& style = edge_lists[style_index];
				ImGui::PushID((int)style_index);
				if (ImGui::TreeNode((void*)(u64)style_index, "Style %llu", style_index))
				{
					for (usize edge_list_index = 0; edge_list_index < style.size(); edge_list_index++)
					{
						const std::vector<Edge>& edges = style[edge_list_index];
						const char* style_type = getFillStyleTypeName(fill_styles[style_index].type);
						if (ImGui::TreeNode((void*)(u64)(edge_list_index + 10000000), "Edge %llu - Size: %llu - Style type: %s", edge_list_index, edges.size(), style_type))
						{

							for (const Edge& edge : edges)
							{

								switch (edge.type)
								{

									case EdgeType::Straight:
									{
										// debugDrawText2D(fromTwips(edge.start), str);
										debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{1.0f, 0, 1.0f, 1.0f}, 0.1f);
									}
									break;

									case EdgeType::Curve:
									{
										// debugDrawText2D(fromTwips(edge.start), str);
										debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{1.0f, 0.0f, 1.0f, 1.0f}, 0.1f);
										/*debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.control), LinearColor{c, 0.1f, 1.0f, 1.0f}, 0.5f);
										debugDrawLine2D(fromTwips(edge.control), fromTwips(edge.end), LinearColor{c, 0.1f, 0.0f, 1.0f}, 0.5f);*/
									}
									break;

									case EdgeType::InvertedCurve:
									{
										// debugDrawText2D(fromTwips(edge.start), str);
										// debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{c, 1.0f, 0.0f, 1.0f}, 5.0f);
										debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.control), LinearColor{1.0f, 0.0f, 1.0f, 1.0f}, 0.1f);
										debugDrawLine2D(fromTwips(edge.control), fromTwips(edge.end), LinearColor{1.0f, 0.0f, 1.0f, 1.0f}, 0.1f);
									}
									break;
								}

								ImGui::Text("Edge: start: %d, %d - end: %d, %d", edge.start.x, edge.start.y, edge.end.x, edge.end.y);
								if (ImGui::IsItemHovered())
								{
									debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{0.5f, 0.5f, 1.0f, 1.0f}, 0.5f);
								}
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
		ImGui::End();
	}

	// static byte str_buffer[32];

#endif

#if 0
	if (debug)
	{
		for (usize style_index = 1; style_index < edge_lists.size(); style_index++)
		{
			int index = 0;
			const std::vector<std::vector<Edge>>& style = edge_lists[style_index];
			for (const std::vector<Edge>& edges : style)
			{
				if (edges.size() == 0)
				{
					continue;
				}

				for (const Edge& edge : edges)
				{
					const f32 c = (f32)index / (f32)500;
					//  const UTF8StringView str = string_format({str_buffer, 32}, "%d", index);
					switch (edge.type)
					{

						case EdgeType::Straight:
						{
							// debugDrawText2D(fromTwips(edge.start), str);
							debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{c, 0, 0.0f, 1.0f}, 1.0f);
						}
						break;

						case EdgeType::Curve:
						{
							// debugDrawText2D(fromTwips(edge.start), str);
							debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{c, 0.0f, 0.0f, 1.0f}, 1.0f);
							/*debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.control), LinearColor{c, 0.1f, 0.0f, 1.0f}, 0.5f);
							debugDrawLine2D(fromTwips(edge.control), fromTwips(edge.end), LinearColor{c, 0.1f, 0.0f, 1.0f}, 0.5f);*/
						}
						break;

						case EdgeType::InvertedCurve:
						{
							// debugDrawText2D(fromTwips(edge.start), str);
							// debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.end), LinearColor{c, 1.0f, 0.0f, 1.0f}, 1.0f);
							debugDrawLine2D(fromTwips(edge.start), fromTwips(edge.control), LinearColor{c, 0.0f, 0.0f, 1.0f}, 1.0f);
							debugDrawLine2D(fromTwips(edge.control), fromTwips(edge.end), LinearColor{c, 0.0f, 0.0f, 1.0f}, 1.0f);
						}
						break;
					}
					index++;
				}
			}
		}
	}
#endif
				// for (EdgeList& list : edge_lists)
				// {
				// 	if (list.edge_count == 0)
				// 	{
				// 		continue;
				// 	}

				// 	Slice<Edge>& edges = list.edges;
				// 	usize& edge_count = list.edge_count;

				// 	Vec2S32 prev_edge_end = edges[0].end;

				// 	for (usize edge_index = 1; edge_index < edge_count; edge_index++)
				// 	{
				// 		const Edge& edge = edges[edge_index];
				// 		PAW_ASSERT(edge.type != EdgeType::UnKnown);
				// 		PAW_ASSERT(edge.start == prev_edge_end);
				// 		prev_edge_end = edge.end;
				// 	}

				// 	if (edge_count > 1)
				// 	{
				// 		PAW_ASSERT(edges[0].start == edges[edge_count - 1].end);
				// 	}
				// }

				if (!debug)
				{
					Slice<SwfVertex> curve_vertices = allocator.alloc<SwfVertex>(10000);
					u32 curve_vertex_count = 0;

					static constexpr Vec2 curve_uv0{1.0f, 0.0f};
					static constexpr Vec2 curve_uv1{1.5f, 0.0f};
					static constexpr Vec2 curve_uv2{2.0f, 1.0f};

					static constexpr Vec2 inv_curve_uv0{-1.0f, 0.0f};
					static constexpr Vec2 inv_curve_uv1{-1.5f, 0.0f};
					static constexpr Vec2 inv_curve_uv2{-2.0f, 1.0f};

					static constexpr Vec2 fill_uv{1.0f, 1.0f};

					PAW_ASSERT(edge_lists.size() == fill_styles.size());
					for (usize style_index = 1; style_index < edge_lists.size(); style_index++)
					{
						std::vector<std::vector<Edge>> const& edge_list = edge_lists[style_index];
						FillStyle const& style = fill_styles[style_index];

						f32 gradient_color_data_z = 0.0f;
						f32 gradient_color_data_w = 0.0f;
						Matrix gradient_matrix;
						bool is_gradient = style.type == FillStyleType::LinearGradient || style.type == FillStyleType::RadialGradient || style.type == FillStyleType::FocalGradient;
						const Vec2 gradient_box_size{1638.4f, 1638.4f};
						if (is_gradient)
						{
							gradient_matrix = style.gradient.matrix;
							invertMatrix(&gradient_matrix);

							const f32 grad_type = style.type == FillStyleType::LinearGradient ? -1.0f : 1.0f;
							const f32 shifted_focal_point = -(style.gradient.focal + 1.0f);
							const f32 grad_index = (f32)style.gradient.gradient_cache_index;
							const f32 grad_index_type = grad_index * grad_type;

							gradient_color_data_z = grad_index_type;
							gradient_color_data_w = shifted_focal_point;
						}

						auto create_vert = [&gradient_matrix, &gradient_box_size, &gradient_color_data_w, &gradient_color_data_z, is_gradient, &style](Vec2 const& pos, Vec2 const& uv) -> SwfVertex
						{
							if (is_gradient)
							{
								Vec2 grad_pos{
									pos.x * gradient_matrix.scale_x + pos.y * gradient_matrix.rotate_skew1 + gradient_matrix.translation_x,
									pos.x * gradient_matrix.rotate_skew0 + pos.y * gradient_matrix.scale_y + gradient_matrix.translation_y,
								};
								grad_pos /= gradient_box_size;
								if (style.type == FillStyleType::LinearGradient)
								{
									// grad_pos.x += 0.5f;
									// grad_pos.x *= 0.5f;
								}
								return {pos, uv, {grad_pos.x, style.type == FillStyleType::LinearGradient ? 0.0f : grad_pos.y, gradient_color_data_z, gradient_color_data_w}};
							}
							else
							{
								const f32 d = 1.0f / 255.0f;
								const Vec4 color{
									(f32)((style.solid_srgb >> 24) & 0xFF) * d,
									(f32)((style.solid_srgb >> 16) & 0xFF) * d,
									(f32)((style.solid_srgb >> 8) & 0xFF) * d,
									(f32)((style.solid_srgb >> 0) & 0xFF) * d,
								};
								return {pos, uv, color};
							}
						};

						TESStesselator* tesselator = tessNewTess(0);
						Defer tesselator_defer = {[&tesselator]
												  { tessDeleteTess(tesselator); }};

						scratch_vertices.clear();

						curve_vertex_count = 0;

						for (std::vector<Edge> const& edges : edge_list)
						{
							if (edges.size() == 0)
							{
								continue;
							}

							scratch_vertices.clear();

							scratch_vertices.push_back(fromTwips(edges[0].start));

							for (usize edge_index = 0; edge_index < edges.size(); edge_index++)
							{
								Edge const& edge = edges[edge_index];
								switch (edge.type)
								{
									case EdgeType::Curve:
									{
										scratch_vertices.push_back(fromTwips(edge.end));
										curve_vertices[curve_vertex_count++] = create_vert(fromTwips(edge.start), curve_uv0);
										curve_vertices[curve_vertex_count++] = create_vert(fromTwips(edge.control), curve_uv1);
										curve_vertices[curve_vertex_count++] = create_vert(fromTwips(edge.end), curve_uv2);
									}
									break;

									case EdgeType::Straight:
									{
										scratch_vertices.push_back(fromTwips(edge.end));
									}
									break;

									case EdgeType::InvertedCurve:
									{
										scratch_vertices.push_back(fromTwips(edge.control));
										scratch_vertices.push_back(fromTwips(edge.end));

										curve_vertices[curve_vertex_count++] = create_vert(fromTwips(edge.start), inv_curve_uv0);
										curve_vertices[curve_vertex_count++] = create_vert(fromTwips(edge.control), inv_curve_uv1);
										curve_vertices[curve_vertex_count++] = create_vert(fromTwips(edge.end), inv_curve_uv2);
									}
									break;

									case EdgeType::UnKnown:
										PAW_UNREACHABLE;
										break;
								}
							}

							tessAddContour(
								tesselator, 2, scratch_vertices.data(), sizeof(scratch_vertices[0]), static_cast<int>(scratch_vertices.size()));
						}

						if (scratch_vertices.size() > 0)
						{
							SwfShape& final_shape = final_shapes[final_shape_count++];
							final_shape.vertex_offset = (u32)final_vertices.size();
							final_shape.index_offset = (u32)final_indices.size();
							final_shape.min_x = bounds.x_min;
							final_shape.min_y = bounds.y_min;
							final_shape.max_x = bounds.x_max;
							final_shape.max_y = bounds.y_max;

							int tess_result = tessTesselate(tesselator, TESS_WINDING_NONZERO, TESS_POLYGONS, 3, 2, nullptr);
							PAW_ASSERT(tess_result == 1);
							PAW_ASSERT_UNUSED(tess_result);

							TESSreal const* tess_verts = tessGetVertices(tesselator);
							const u32 vert_count = tessGetVertexCount(tesselator);
							TESSindex const* tess_indices = tessGetElements(tesselator);
							int const tess_index_count = (tessGetElementCount(tesselator) * 3);

							for (u32 i = 0; i < vert_count; i++)
							{
								final_vertices.push_back(create_vert(Vec2{tess_verts[i * 2 + 0], tess_verts[i * 2 + 1]}, fill_uv));
							}

							for (u32 i = 0; i < (u32)tess_index_count; i++)
							{
								final_indices.push_back(tess_indices[i] + final_shape.vertex_offset);
							}
							for (u32 i = 0; i < (u32)curve_vertex_count; i++)
							{
								final_vertices.push_back(curve_vertices[i]);
								final_indices.push_back(i + vert_count + final_shape.vertex_offset);
							}

							PAW_ASSERT(final_shape_count < final_shapes.size);
							final_shape.index_count = tess_index_count + curve_vertex_count;
						}
					}
				}
			}
			break;

			case TagType::DefineSprite:
			{
				const u16 id = parser.readU16();
				(void)id;
				if (!debug)
				{
					PAW_LOG_INFO("=========================== %s %hu ===========================", getTagTypeName(tag.type), id);
				}
				const u16 sprite_frame_count = parser.readU16();
				(void)sprite_frame_count;
				if (!debug)
				{
					PAW_LOG_INFO("Frame Count: %hu", sprite_frame_count);
				}
			}
			break;

			default:
			{
				PAW_LOG_WARNING("Tag not handled");
			}
			break;
		}

		parser.bit_offset = 0;
		parser.walker = tag_start + tag.size;
	} while (tag.type != TagType::End);

	{
		FILE* file = fopen("vertices.bin", "wb");
		fwrite(final_vertices.data(), final_vertices.size() * sizeof(final_vertices[0]), 1, file);
		fclose(file);
	}

	{
		FILE* file = fopen("indices.bin", "wb");
		fwrite(final_indices.data(), final_indices.size() * sizeof(final_indices[0]), 1, file);
		fclose(file);
	}

	// GpuBufferView vertex_buffer = rendererCreateAndUploadBuffer({(byte*)final_vertices.data(), final_vertices.size() * sizeof(SwfVertex)}, GpuBufferType_Storage);

	const usize gradient_atlas_width = 256;
	const usize gradient_atlas_height = gradient_cache.size() * 3;
	Slice<u32> gradient_buffer = allocator.alloc<u32>(gradient_atlas_width * gradient_atlas_height);
	memset(gradient_buffer.ptr, 255, gradient_buffer.calc_total_size_in_bytes());
	for (usize gradient_index = 0; gradient_index < gradient_cache.size(); ++gradient_index)
	{
		CachedGradient const& gradient = gradient_cache[gradient_index];
		usize start_control_point = 0;
		usize end_control_point = 1;

		for (u32 pixel = 0; pixel < 256; ++pixel)
		{
			if (pixel >= gradient.ratios[end_control_point] && pixel != 255)
			{
				start_control_point++;
				end_control_point++;
			}

			const f32 delta = (f32)(gradient.ratios[end_control_point] - gradient.ratios[start_control_point]);
			const f32 local_index = (f32)(pixel - gradient.ratios[start_control_point]);

			const f32 t = local_index / delta;

			const Vec4 start_color{
				(f32)((gradient.colors[start_control_point] >> 24) & 0xFF),
				(f32)((gradient.colors[start_control_point] >> 16) & 0xFF),
				(f32)((gradient.colors[start_control_point] >> 8) & 0xFF),
				(f32)((gradient.colors[start_control_point] >> 0) & 0xFF),
			};

			const Vec4 end_color{
				(f32)((gradient.colors[end_control_point] >> 24) & 0xFF),
				(f32)((gradient.colors[end_control_point] >> 16) & 0xFF),
				(f32)((gradient.colors[end_control_point] >> 8) & 0xFF),
				(f32)((gradient.colors[end_control_point] >> 0) & 0xFF),
			};

			const u8 r = (u8)math_lerp(start_color.x, end_color.x, t);
			const u8 g = (u8)math_lerp(start_color.y, end_color.y, t);
			const u8 b = (u8)math_lerp(start_color.z, end_color.z, t);
			const u8 a = (u8)math_lerp(start_color.w, end_color.w, t);

#define RGBA(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)
			const u32 color = RGBA(r, g, b, a);
			for (usize i = 0; i < 3; i++)
			{
				const usize write_pos = (gradient_index * 3 + i) * gradient_atlas_width + pixel;
				gradient_buffer[write_pos] = color;
			}
		}
	}

	{
		FILE* file = fopen("gradients.bin", "wb");
		u32 const gradient_count = u32(gradient_cache.size());
		u32 const width = u32(gradient_atlas_width);
		u32 const height = u32(gradient_atlas_height);
		fwrite(&width, sizeof(width), 1, file);
		fwrite(&height, sizeof(height), 1, file);
		fwrite(&gradient_count, sizeof(gradient_count), 1, file);
		fwrite(gradient_buffer.ptr, gradient_buffer.size * sizeof(gradient_buffer[0]), 1, file);
		fclose(file);
	}

	{
		FILE* file = fopen("shapes.bin", "wb");
		fwrite(final_shapes.ptr, final_shape_count * sizeof(final_shapes[0]), 1, file);
		fclose(file);
	}

	// GpuImageHandle gradient_image = rendererCreateAndUploadTexture(gradient_buffer.to_const_byte_slice(), GpuTextureFormat_R8G8B8A8Unorm, gradient_atlas_width, gradient_atlas_height, PAW_STR("Vector Gradient Text", 20));

	return SwfStore{
		//.vertex_buffer_slot = rendererPushBufferToShader(vertex_buffer),
		//.index_buffer = rendererCreateAndUploadBuffer({(byte*)final_indices.data(), final_indices.size() * sizeof(u32)}, GpuBufferType_Index),
		//.gradient_texture_slot = rendererPushTextureToShader(gradient_image),
		//.gradient_image = gradient_image,
		.gradient_count = (u32)gradient_cache.size(),
		.shapes = final_shapes,
		.shape_count = final_shape_count,
		.min_x = frame_size.x_min,
		.min_y = frame_size.y_min,
		.max_x = frame_size.x_max,
		.max_y = frame_size.y_max,
	};
}

void unloadSwf(SwfStore* swf)
{
	rendererTextureRemoveRef(swf->gradient_image);
}