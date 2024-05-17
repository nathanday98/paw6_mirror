#include "debug_draw.h"

#include <shared/math.inl>
#include <shared/log.h>
#include <shared/assert.h>
#include <shared/string.h>
#include <shared/defer.h>
#include <shared/memory.h>
#include <shared/allocator.h>

PAW_DISABLE_ALL_WARNINGS_BEGIN
#include <ft2build.h>
#include FT_FREETYPE_H
PAW_DISABLE_ALL_WARNINGS_END

#include <platform/platform.h>

#include "transform.h"
#include "new_renderer.h"
#include "allocators/page_pool.h"

struct GuiGlyph
{
	Vec2 size;
	Vec2 min_uv;
	Vec2 max_uv;
	Vec2 offset;
	f32 advance;
	bool colored;
};

struct DebugCommand2D
{
	Vec2 start;
	Vec2 end;
	Vec2 min_uv;
	Vec2 max_uv;
	LinearColor color;
	u32 texture_index;
	f32 thickness;
};

// If PageHeader is not 16 bytes we need to account for alignment offset for the commands
static_assert(sizeof(DebugCommandStore::PageHeader) == 16);

const DebugDrawCommand3D* DebugCommandStore::PageHeader::getCommandsPtr() const
{
	return reinterpret_cast<const DebugDrawCommand3D*>(reinterpret_cast<const byte*>(this) + sizeof(PageHeader));
}

DebugDrawCommand3D* DebugCommandStore::PageHeader::getCommandsPtr()
{
	return reinterpret_cast<DebugDrawCommand3D*>(reinterpret_cast<byte*>(this) + sizeof(PageHeader));
}

void DebugCommandStore::init(PagePool* debug_page_pool)
{
	this->page_pool = debug_page_pool;
	this->page_size = debug_page_pool->page_size;
	this->commands_per_page = (page_size - sizeof(PageHeader)) / sizeof(DebugDrawCommand3D);
	current_page = nullptr;
	this->page_count = 0;
}

void DebugCommandStore::deinit()
{
	PageHeader* page = current_page;
	while (page)
	{
		PageHeader* prev_page = page->prev;

		page_pool->free(reinterpret_cast<byte*>(page));

		page = prev_page;
	}

	page_pool = nullptr;
	page_size = 0;
	commands_per_page = 0;
	current_page = nullptr;
	page_count = 0;
}

void DebugCommandStore::push(const Slice<const DebugDrawCommand3D>& commands)
{
	usize remaining_commands = commands.size;
	while (remaining_commands > 0)
	{
		const usize page_remaining_slots = current_page != nullptr ? commands_per_page - current_page->count : 0;
		const usize commands_to_write = math_minUsize(remaining_commands, page_remaining_slots);
		if (commands_to_write > 0)
		{
			DebugDrawCommand3D* page_commands = current_page->getCommandsPtr() + current_page->count;
			memCopy(&commands[commands.size - remaining_commands], page_commands, commands_to_write * sizeof(DebugDrawCommand3D));
			remaining_commands -= commands_to_write;
			current_page->count += commands_to_write;
		}

		if (remaining_commands > 0) // There's no more room in the current page, allocate another
		{
			byte* mem = page_pool->alloc();
			PAW_ASSERT(calcAlignmentOffset(mem, 16) == 0);

			PageHeader* new_page = reinterpret_cast<PageHeader*>(mem);
			new_page->prev = current_page;
			current_page = new_page;
			current_page->count = 0;
			page_count++;
		}
	}
}

void DebugCommandStore::tickLifetimes(f32 delta_time_seconds)
{
	PageHeader* page = current_page;
	while (page)
	{
		DebugDrawCommand3D* page_commands = page->getCommandsPtr();
		for (int i = (int)page->count - 1; i >= 0; i--)
		{
			const usize command_index = (usize)i;
			DebugDrawCommand3D* command = &page_commands[command_index];
			if (command->remaining_time_seconds != PAW_LIFETIME_INFINITE)
			{
				command->remaining_time_seconds -= delta_time_seconds;

				if (command->remaining_time_seconds <= 0.0f)
				{
					PAW_ASSERT(command->remaining_time_seconds >= -delta_time_seconds);
					if (command_index < page->count - 1)
					{
						DebugDrawCommand3D* end_command = &page_commands[page->count - 1];
						memCopy(end_command, command, sizeof(DebugDrawCommand3D));
					}
					page->count--;
				}
			}
		}
		page = page->prev;
	}
}

void DebugCommandStore::cloneFrom(const DebugCommandStore& other)
{
	PageHeader* page = other.current_page;
	PageHeader* prev_new_page = nullptr;
	while (page)
	{
		byte* new_page_mem = page_pool->alloc();
		PageHeader* new_page = reinterpret_cast<PageHeader*>(new_page_mem);
		if (page == other.current_page)
		{
			current_page = new_page;
		}
		if (prev_new_page != nullptr)
		{
			prev_new_page->prev = new_page;
		}
		memCopy(page, new_page_mem, page_size);
		page = page->prev;
		prev_new_page = new_page;
	}
}

static struct
{
	f32 cached_dpi; // This should be updated on dpi change
	u32 texture_index;
	GpuImageHandle texture;
	Slice<GuiGlyph> glyph_data;
	FT_Face font_face;

	DebugDrawState* state;

	bool initialized;
} g;

void debugDrawInit(Platform& platform, Allocator& persistent_allocator, PagePool* debug_page_pool)
{
	FT_Library library;
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		PAW_LOG_ERROR("Failed to init FreeType");
	}

	int ft_major, ft_minor, ft_patch;
	FT_Library_Version(library, &ft_major, &ft_minor, &ft_patch);

	// error = FT_New_Face(library, "source_data/fonts/DroidSans.ttf", 0, &font_face);
	// const char* font_path = "C:\\Windows\\Fonts\\SEGUIEMJ.ttf";
	const char* font_path = "source_data/fonts/DroidSans.ttf";
	error = FT_New_Face(library, font_path, 0, &g.font_face);
	if (error)
	{
		PAW_LOG_ERROR("Failed to load font %s", font_path);
	}
	else
	{
		PAW_LOG_INFO(
			"Loaded font %s (%s)", font_path, FT_HAS_COLOR(g.font_face) ? "color supported" : "color unsupported");
	}

	g.cached_dpi = platform.get_dpi_Scale();

	// error = FT_Set_Char_Size(font_face, 0, 16 << 6, g_window_state.dpi, g_window_state.dpi);
	error = FT_Set_Pixel_Sizes(g.font_face, 0, static_cast<FT_UInt>(math_floor(20 * g.cached_dpi)));

	if (error)
	{
		PAW_LOG_ERROR("Failed to set font size");
	}

	// #define FT_CEIL(X) (((X + 63) & -64) / 64)
#define FT_CEIL(x) (x >> 6)

	const f32 font_line_height = static_cast<f32>(FT_CEIL(g.font_face->size->metrics.height));
	const f32 descender = static_cast<f32>(FT_CEIL(g.font_face->size->metrics.descender));
	const f32 font_height = (static_cast<f32>(FT_CEIL(g.font_face->size->metrics.ascender)) + descender);
	(void)font_height;
	(void)font_line_height;
	g.glyph_data = persistent_allocator.alloc<GuiGlyph>(static_cast<usize>(g.font_face->num_glyphs));

	usize tex_size = (10 + (static_cast<usize>(g.font_face->size->metrics.height >> 6))) *
		static_cast<usize>(math_ceil(math_squareRoot(static_cast<f32>(g.font_face->num_glyphs))));
	// const usize tex_size = 1024;

	Allocator& allocator = platform.get_system_allocator();

	Slice<s32> font_buffer = allocator.alloc<s32>(tex_size * tex_size);
	Defer font_buffer_defer = {[&font_buffer, &allocator]
							   { allocator.free(font_buffer); }};

	{
		const s32 padding = 1;
		s32 pen_x = padding;
		s32 pen_y = padding;

		const Vec2 texture_size_v = Vec2{static_cast<f32>(tex_size), static_cast<f32>(tex_size)};
		const Vec2 texel_size = Vec2{1.0f, 1.0f} / texture_size_v;

		// void* data_ptr = nullptr;
		// PAW_ASSERT(SUCCEEDED(result));

		// u32* font_buffer_ptr = static_cast<u32*>(data_ptr);
		bool colored = false;
		for (s32 glyph_index = 0; glyph_index < g.font_face->num_glyphs; glyph_index++)
		{
			const FT_Error ft_error =
				FT_Load_Glyph(g.font_face, glyph_index, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_COLOR);
			if (ft_error != 0)
			{
				PAW_LOG_ERROR("FreeType: %s", FT_Error_String(ft_error));
			}
			const FT_Bitmap* bitmap = &g.font_face->glyph->bitmap;

			if (pen_x + bitmap->width >= tex_size)
			{
				pen_x = 0;
				pen_y += (g.font_face->size->metrics.height >> 6) + padding;
			}

			switch (bitmap->pixel_mode)
			{
				case FT_PIXEL_MODE_BGRA:
				{
					for (u32 row = 0; row < bitmap->rows; row++)
					{
						for (u32 col = 0; col < bitmap->width; col++)
						{
							const byte* pixel = bitmap->buffer + (row * bitmap->pitch + col * 4);
							font_buffer[(pen_y + row) * static_cast<s32>(tex_size) + (pen_x + col)] =
								(pixel[3] << 24) | (pixel[0] << 16) | (pixel[1] << 8) | pixel[2];
						}
					}
					colored = true;
				}
				break;

				case FT_PIXEL_MODE_GRAY:
				{
					for (u32 row = 0; row < bitmap->rows; row++)
					{
						for (u32 col = 0; col < bitmap->width; col++)
						{
							const byte* pixel = bitmap->buffer + (row * bitmap->pitch + col);
							const u32 color = (*pixel << 24) | 0xFFFFFF;
							font_buffer[(pen_y + row) * static_cast<s32>(tex_size) + (pen_x + col)] = color;
						}
					}
				}
				break;

				default:
				{
					PAW_LOG_WARNING("Unsupported pixel type");
				}
			}

			const Vec2 pen_pos{static_cast<f32>(pen_x), static_cast<f32>(pen_y)};

			GuiGlyph& out_glyph = g.glyph_data[glyph_index];
			out_glyph.size = Vec2{static_cast<f32>(bitmap->width), static_cast<f32>(bitmap->rows)};
			out_glyph.min_uv = pen_pos * texel_size;
			out_glyph.max_uv = out_glyph.min_uv + out_glyph.size * texel_size;

			out_glyph.offset =
				Vec2{static_cast<f32>(g.font_face->glyph->bitmap_left), -static_cast<f32>(g.font_face->glyph->bitmap_top)};
			/*out_glyph.offset = Vec2{
				static_cast<f32>(FT_CEIL(font_face->glyph->metrics.horiBearingX)),
				static_cast<f32>(FT_CEIL(font_face->glyph->metrics.horiBearingY))};*/
			out_glyph.advance = static_cast<f32>(FT_CEIL(g.font_face->glyph->advance.x));
			out_glyph.colored = colored;
			pen_x += bitmap->width + padding;
		}
	}

	// g.texture = rendererCreateAndUploadTexture(font_buffer.toConstByteSlice(), GpuTextureFormat_R8G8B8A8Unorm, tex_size, tex_size, PAW_STR("Debug Font Texture", 18));
	// g.texture_index = rendererPushTextureToShader(g.texture);

	g.state->commands_3d.init(debug_page_pool);
	g.state->depth_tested_commands_3d.init(debug_page_pool);

	g.initialized = true;
}

void debugDrawDeinit()
{
	// rendererTextureRemoveRef(g.texture);
}

void debugDrawTickLifetimes(f32 delta_time_seconds)
{
	g.state->commands_3d.tickLifetimes(delta_time_seconds);
	g.state->depth_tested_commands_3d.tickLifetimes(delta_time_seconds);
}

void debugDrawSetState(DebugDrawState* state)
{
	g.state = state;
}

void debugDrawLine(const Vec3& start, const Vec3& end, bool depth, const Vec4& color, f32 thickness, f32 lifetime_seconds)
{
	PAW_ASSERT(g.initialized);
	DebugCommandStore& store = depth ? g.state->depth_tested_commands_3d : g.state->commands_3d;
	store.push({{
		color,
		start,
		end,
		thickness,
		thickness,
		lifetime_seconds,
	}});
}

void debugDrawCommands(const Slice<const DebugDrawCommand3D>& commands, bool depth)
{
	PAW_ASSERT(g.initialized);
	DebugCommandStore& store = depth ? g.state->depth_tested_commands_3d : g.state->commands_3d;
	store.push(commands);
}

void debugDrawTransform(const Transform& transform, bool depth, f32 thickness, f32 lifetime_seconds)
{
	PAW_ASSERT(g.initialized);
	const Mat4 mat = toMat4(transform);
	debugDrawTransformFromMat(mat, depth, thickness, lifetime_seconds);
}

void debugDrawTransformFromMat(const Mat4& mat, bool depth, f32 thickness, f32 lifetime_seconds)
{
	PAW_ASSERT(g.initialized);

	const Vec3 origin = mat.transformPoint(Vec3{0.0f, 0.0f, 0.0f});
	const Vec3 up = mat.transformVector(math_world_up);
	const Vec3 right = mat.transformVector(math_world_right);
	const Vec3 forward = mat.transformVector(math_world_forward);

	DebugCommandStore& store = depth ? g.state->depth_tested_commands_3d : g.state->commands_3d;

	store.push({
		{Vec4{1.0f, 0.0f, 0.0f, 1.0f}, origin, origin + right, thickness, thickness, lifetime_seconds},
		{Vec4{0.0f, 1.0f, 0.0f, 1.0f}, origin, origin + up, thickness, thickness, lifetime_seconds},
		{Vec4{0.0f, 0.0f, 1.0f, 1.0f}, origin, origin + forward, thickness, thickness, lifetime_seconds},
		//{Vec4{1.0f, 1.0f, 0.0f, 1.0f}, origin - Vec3{0.05f, 0.0f, 0.0f}, origin + Vec3{0.05f, 0.0f, 0.0f}, 0.1f, lifetime_seconds},
	});
}

void debugDrawDisc(
	const Vec3& center, f32 radius, const Vec3& up, const Vec3& right, bool depth, const Vec4& color, f32 thickness, f32 lifetime_seconds)
{
	PAW_ASSERT(g.initialized);

	const f32 segment_count = 24.0f;
	const f32 d = (math_pi32 * 2.0f) / segment_count;

	auto calc_pos = [center, up, right, radius](f32 t) -> Vec3
	{
		const f32 x = math_cos(t) * radius;
		const f32 y = math_sin(t) * radius;
		return center + up * y + right * x;
	};

	Vec3 previous_pos = calc_pos(0.0f);

	for (f32 i = 1.0f; i <= segment_count; i++)
	{
		const Vec3 pos = calc_pos(d * i);
		debugDrawLine(previous_pos, pos, depth, color, thickness, lifetime_seconds);
		previous_pos = pos;
	}
}

void debugDrawLineXZ(const Vec2& start, const Vec2& end, bool depth, const Vec4& color, f32 thickness, f32 lifetime_seconds)
{
	PAW_ASSERT(g.initialized);

	return debugDrawLine(Vec3{start.x, 0.0f, start.y}, Vec3{end.x, 0.0f, end.y}, depth, color, thickness, lifetime_seconds);
}

void debugDrawDiscXZ(const Vec2& center, f32 radius, bool depth, const Vec4& color, f32 thickness, f32 lifetime_seconds)
{
	PAW_ASSERT(g.initialized);

	const f32 segment_count = 24.0f;
	const f32 d = (math_pi32 * 2.0f) / segment_count;
	auto calc_pos = [center, radius](f32 t) -> Vec2
	{
		const f32 x = math_cos(t) * radius;
		const f32 y = math_sin(t) * radius;
		return center + xz(math_world_forward) * y + xz(math_world_right) * x;
	};

	Vec2 previous_pos = calc_pos(0.0f);

	for (f32 i = 1.0f; i <= segment_count; i++)
	{
		const Vec2 pos = calc_pos(d * i);
		debugDrawLineXZ(previous_pos, pos, depth, color, thickness, lifetime_seconds);
		previous_pos = pos;
	}
}

void debugDrawLine2D(const Vec2& start, const Vec2& end, const LinearColor& color, f32 thickness)
{
	PAW_ASSERT(g.initialized);

	rendererGameViewportAddDebugLine2D(start, end, color, thickness, Vec2{}, Vec2{}, 0);
}

void debugDrawRect2D(const Vec2& min, const Vec2& max, const LinearColor& color, f32 thickness)
{
	PAW_ASSERT(g.initialized);
	const Vec2 top_right = Vec2{max.x, min.y};
	const Vec2 bottom_left = Vec2{min.x, max.y};
	debugDrawLine2D(min, top_right, color, thickness);
	debugDrawLine2D(top_right, max, color, thickness);
	debugDrawLine2D(bottom_left, max, color, thickness);
	debugDrawLine2D(min, bottom_left, color, thickness);
}

void debugDrawText2D(const Vec2& position, const UTF8StringView& text, const LinearColor& color)
{
	PAW_ASSERT(g.initialized);

	Vec2 pen{};
	Vec2 offset = math_roundVec2(position);
	u32 unicode = 0;
	u32 byte_count = 0;
	for (usize glyph_index = 0; glyph_index < text.size_bytes; ++glyph_index)
	{
		const byte b = text.ptr[glyph_index];
		if ((b & 0b11000000) == 0b11000000)
		{
			bool bytes_2 = (b & 0b11100000) == 0b11000000;
			bool bytes_3 = (b & 0b11110000) == 0b11100000;
			bool bytes_4 = (b & 0b11111000) == 0b11110000;
			unicode = 0;
			if (bytes_2)
			{
				unicode |= b & 0b00011111;
				unicode <<= 5;
				byte_count = 1;
			}
			else if (bytes_3)
			{
				unicode |= b & 0b00001111;
				unicode <<= 4;
				byte_count = 2;
			}
			else if (bytes_4)
			{
				unicode |= b & 0b00000111;
				unicode <<= 3;
				byte_count = 3;
			}
		}
		else if ((b & 0b11000000) == 0b10000000)
		{
			PAW_ASSERT(byte_count > 0);
			byte_count--;
			unicode <<= 6;
			unicode |= b & 0b00111111;
		}
		else
		{
			unicode = b;
			byte_count = 0;
		}

		if (byte_count > 0)
		{
			continue;
		}

		const u32 char_index = FT_Get_Char_Index(g.font_face, unicode);
		const GuiGlyph& glyph = g.glyph_data[char_index];
		bool is_space = false;
		if (unicode < 128)
		{
			const byte ascii = unicode & 0xFF;
			is_space = ascii == ' ' || ascii == '\t' || ascii == '\n' || ascii == '\r';
		}
		const f32 inv_dpi_scale = 1.0f / g.cached_dpi;
		if (!is_space)
		{
			// This probably still needs some kind of position / size rounding to prevent
			Vec2 pos = offset + pen + glyph.offset * inv_dpi_scale;
			const f32 thickness = (glyph.size.x * inv_dpi_scale * 0.5f);
			rendererGameViewportAddDebugLine2D(
				pos + Vec2{thickness, 0.0f},
				pos + Vec2{thickness, glyph.size.y * inv_dpi_scale},
				glyph.colored ? LinearColor{1.0f, 1.0f, 1.0f, 1.0f} : color,
				thickness * 2.0f,
				glyph.min_uv,
				glyph.max_uv,
				g.texture_index);
		}
		pen.x += math_round(glyph.advance * inv_dpi_scale);
	}
}
