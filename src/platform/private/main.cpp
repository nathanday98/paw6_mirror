#include <thread>
#include <semaphore>
#include <vector>

#include <shared/std.h>
#include <shared/assert.h>
#include <shared/string.h>
#include <shared/log.h>
#include <shared/shared.h>
#include <shared/defer.h>
#include <shared/math.inl>
#include <shared/memory.h>
#include <shared/string_id.inl>
#include <shared/allocator.h>
#if PAW_TESTS
// #include <shared/testing.h>
#endif

#include <testing/testing.h>

#include "../presentation/private/profiler.h"

#define PAW_ENABLE_IMGUI

PAW_DISABLE_ALL_WARNINGS_BEGIN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <timeapi.h>
#include <ShellScalingApi.h>
#include <hidusage.h>
#include <intrin.h>
#ifdef PAW_ENABLE_IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#endif
// #define __cplusplus
#include <d3d12/d3d12.h>
#include <dxgi1_6.h>
#include <stb_image.h>
PAW_DISABLE_ALL_WARNINGS_END

#include "../presentation/private/game.h"
#include "../presentation/private/input.h"
#include "../presentation/private/startup_args.h"
#include "../presentation/private/client_graph.h"

#include <platform/platform.h>

template <typename T>
struct SPSCBoundedQueue
{
	void init(usize buffer_size, Allocator* in_allocator)
	{
		buffer = in_allocator->alloc<T>(buffer_size);
		allocator = in_allocator;
	}

	void deinit()
	{
		allocator->free(buffer);
	}

	bool tryEnqueue(T const& item)
	{
		usize w = write_pos;
		if (w >= buffer.size)
		{
			return false;
		}
		buffer[w] = item;
		write_pos = w + 1;
		return true;
	}

	bool tryDequeue(T& item)
	{
		usize w = write_pos;
		if (read_pos >= w)
		{
			return false;
		}
		item = buffer[read_pos];
		read_pos++;
		return true;
	}

	void forceEnqueue(T const& item)
	{
		while (!tryEnqueue(item))
		{
		}
	}

private:
	Allocator* allocator;
	Slice<T> buffer;
	std::atomic<usize> write_pos;
	usize read_pos;
};
#ifdef PAW_ENABLE_IMGUI
static void initImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	//  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigWindowsResizeFromEdges = true;
	// io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.FramePadding = ImVec2(12.0f, 5.0f);
	style.ScrollbarSize = 20.0f;
	style.GrabMinSize = 16.0f;
	style.FrameBorderSize = 1.0f;
	style.TabBorderSize = 1.0f;

	ImGui::StyleColorsLight();

	// ImVec4* colors = style.Colors;
	// colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	// colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	// colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	// colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	// colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	// colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	// colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	// colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	// colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	// colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	// colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	// colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	// colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	// colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	// colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	// colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	// colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	// colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	// colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	// colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	// colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	// colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	// colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	// colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	// colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	// colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	// colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	// colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.3f);
	// colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	// colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	// colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	// colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	// colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	// colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	// colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	// colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	// colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	// colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	// colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	// colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	// colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	// colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	// colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	// colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	// colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	// colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	// colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	// colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	// colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	// colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

	// style.ScaleAllSizes(dpi_scale);
}
#endif

static struct
{
	HWND handle;
	s32 client_width;
	s32 client_height;
	s32 actual_width;
	s32 actual_height;
	s32 x, y;
	u32 dpi;
	f32 dpi_scale;
	DWORD style;
	s32 mouse_x, mouse_y;
	bool mouse_locked = false;
} g_window_state;

static Platform* g_platform;

static bool g_game_initialized = false;

static HCURSOR g_default_cursor;
static HCURSOR g_current_cursor;

static GameInputState g_game_input_state;
static GameEventData* g_current_game_event_data;

static PlatformSemaphore g_window_thread_wake_semaphore;
static PlatformSemaphore g_game_thread_wake_semaphore;

static std::atomic<bool> g_running;

static void clipCursorToCurrentWindow()
{
	if (!g_window_state.mouse_locked)
		return;

	const RECT rect{
		.left = g_window_state.x,
		.top = g_window_state.y,
		.right = g_window_state.x + g_window_state.client_width,
		.bottom = g_window_state.y + g_window_state.client_height,
	};

	ClipCursor(&rect);
}

static void logWindowState()
{
	PAW_LOG_INFO(
		"Window State:\n\tClient: %d, %d\n\tActual: %d, %d\n\tDPI: %d, %g\n\tPosition: %d, %d",
		g_window_state.client_width,
		g_window_state.client_height,
		g_window_state.actual_width,
		g_window_state.actual_height,
		g_window_state.dpi,
		g_window_state.dpi_scale,
		g_window_state.x,
		g_window_state.y);
}
#ifdef PAW_ENABLE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

static void assert_func(char const* file, u32 line, char const* expression)
{
	PAW_LOG_ERROR("Assert: (%s:%d): %s", file, line, expression);
}

SharedAssertFunc* g_shared_assert_func = &assert_func;

static PlatformKeyType g_key_mapper[VK_OEM_CLEAR]{};

static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
#ifdef PAW_ENABLE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam))
	{
		return true;
	}
#endif

	PlatformEvent event{};

	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			PAW_LOG_INFO("Posted quit");
		}
		break;

			// case WM_GETDPISCALEDSIZE:
			//{
			//	const u32 dpi = static_cast<u32>(wparam);
			//	const f32 dpi_scaling = static_cast<f32>(dpi) / USER_DEFAULT_SCREEN_DPI;
			//	SIZE* size = reinterpret_cast<SIZE*>(lparam);

			//	RECT client_rect;
			//	GetClientRect(window, &client_rect);
			//	client_rect.right = static_cast<s32>(client_rect.right * dpi_scaling);
			//	client_rect.bottom = static_cast<s32>(client_rect.bottom * dpi_scaling);

			//	AdjustWindowRectExForDpi(&client_rect, g_window_state.style, FALSE, 0, dpi);

			//	const s32 new_width = client_rect.right - client_rect.left;
			//	const s32 new_height = client_rect.bottom - client_rect.top;

			//	PAW_LOG_INFO("GetDpiScaledSize: %d, %d to %d, %d", size->cx, size->cy, new_width, new_height);
			//	size->cx = new_width;
			//	size->cy = new_height;
			//}
			// break;

		case WM_DPICHANGED:
		{
			const s32 x = LOWORD(wparam);
			const s32 y = HIWORD(wparam);
			PAW_ASSERT(x == y);

			g_window_state.dpi = x;
			g_window_state.dpi_scale = static_cast<f32>(x) / static_cast<f32>(USER_DEFAULT_SCREEN_DPI);

			const RECT* suggested = reinterpret_cast<const RECT*>(lparam);
			PAW_LOG_INFO("DPI Change: %d, %d, - %p{WIN32_RECT}", x, y, suggested);

			g_window_state.x = suggested->left;
			g_window_state.y = suggested->top;
			g_window_state.actual_width = suggested->right - suggested->left;
			g_window_state.actual_height = suggested->bottom - suggested->top;

			SetWindowPos(
				g_window_state.handle,
				NULL,
				g_window_state.x,
				g_window_state.y,
				g_window_state.actual_width,
				g_window_state.actual_height,
				SWP_NOZORDER | SWP_NOACTIVATE);

			RECT client_rect;
			GetClientRect(window, &client_rect);
			g_window_state.client_width = client_rect.right;
			g_window_state.client_height = client_rect.bottom;

			clipCursorToCurrentWindow();

			logWindowState();
		}
		break;

		case WM_MOVE:
		{
			// Casting taking directly from MSDN: https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-move
			g_window_state.x = (s32)(short)LOWORD(lparam);
			g_window_state.y = (s32)(short)HIWORD(lparam);
			event.type = PlatformEventType_WindowMove;
			event.move.x = g_window_state.x;
			event.move.y = g_window_state.y;
		}
		break;

		case WM_SIZE:
		{
			char const* type = [wparam]
			{
				switch (wparam)
				{
					case SIZE_MAXHIDE:
						return "Max Hide";
					case SIZE_MAXIMIZED:
						return "Maximized";
					case SIZE_MAXSHOW:
						return "Max Show";
					case SIZE_MINIMIZED:
						return "Minimized";
					case SIZE_RESTORED:
						return "Restored";
				}
				PAW_UNREACHABLE;
				return "Unknown";
			}();
			const UINT width = LOWORD(lparam);
			const UINT height = HIWORD(lparam);
			g_window_state.client_width = static_cast<s32>(width);
			g_window_state.client_height = static_cast<s32>(height);

			RECT window_rect{
				.left = 0,
				.top = 0,
				.right = g_window_state.client_width,
				.bottom = g_window_state.client_height,
			};
			AdjustWindowRectExForDpi(&window_rect, g_window_state.style, FALSE, 0, g_window_state.dpi);

			g_window_state.actual_width = window_rect.right - window_rect.left;
			g_window_state.actual_height = window_rect.bottom - window_rect.top;

			event.type = PlatformEventType_Resize;
			event.resize.width = (s32)width;
			event.resize.height = (s32)height;

			clipCursorToCurrentWindow();

			PAW_LOG_INFO("Size: %u, %u - %s", width, height, type);
			logWindowState();
		}
		break;

		case WM_INPUT:
		{
			u32 size = sizeof(RAWINPUT);
			static byte buffer[sizeof(RAWINPUT)];

			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));
			const RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				const RAWMOUSE& mouse = raw->data.mouse;
				/*PAW_LOG_INFO(
					"Mouse: %s%s%s%s%s - %d, %d",
					(mouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE ? "Relative" : "",
					(mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE ? " | Absolute" : "",
					(mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP ? "| Virtual Desktop" : "",
					(mouse.usFlags & MOUSE_ATTRIBUTES_CHANGED) == MOUSE_ATTRIBUTES_CHANGED ? " | Attributes Changed"
																						   : "",
					(mouse.usFlags & MOUSE_MOVE_NOCOALESCE) == MOUSE_MOVE_NOCOALESCE ? " | Move No Coalesce" : "",
					mouse.lLastX,
					mouse.lLastY);*/

				PAW_ASSERT((mouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE);

				g_game_input_state.raw_mouse_move_delta.x = mouse.lLastX;
				g_game_input_state.raw_mouse_move_delta.y = mouse.lLastY;

				g_window_state.mouse_x += mouse.lLastX;
				g_window_state.mouse_y += mouse.lLastY;
			}
		}
		break;

		case WM_MOUSEMOVE:
		{
			g_game_input_state.mouse_position.x = LOWORD(lparam);
			g_game_input_state.mouse_position.y = HIWORD(lparam);
		}
		break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		{
			// event.type = PlatformEventType_MouseButton;
			// event.mouse_button.button = MouseButton_Right;
			// event.mouse_button.is_down = message == WM_RBUTTONDOWN;
		}
		break;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		{
			// event.type = PlatformEventType_MouseButton;
			// event.mouse_button.button = MouseButton_Middle;
			// event.mouse_button.is_down = message == WM_MBUTTONDOWN;
		}
		break;

		// case WM_SYSKEYUP:
		// case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN:
		{
			WORD flags = HIWORD(lparam);
			bool was_down = (flags & KF_REPEAT) == KF_REPEAT;
			bool is_down = (flags & KF_UP) != KF_UP;

			bool extended = (lparam >> 24) & 1;
			PlatformKeyType key = g_key_mapper[wparam];
			if (key == PlatformKeyType_LCtrl && extended)
			{
				key = PlatformKeyType_RCtrl;
			}
			GameButtonState& key_state = g_game_input_state.keys[key];
			if (key_state.ended_down != is_down)
			{
				key_state.half_transition_count += (was_down != is_down);
				key_state.ended_down = is_down;
			}
		}
		break;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
		{
			if (g_current_game_event_data)
			{
				g_current_game_event_data->focus_changed = true;
				g_current_game_event_data->focused = message == WM_SETFOCUS;
			}
		}
		break;

		case WM_SETCURSOR:
		{
			const u16 hit_test = LOWORD(lparam);
			if (hit_test == HTCLIENT)
			{
				SetCursor(g_current_cursor);
				return true;
			}
		}
		break;

		default:
		{
			return DefWindowProc(window, message, wparam, lparam);
		}
	}

	return false;
}

static int appMain(Platform& platform, GameStartupArgs& game_args)
{
	g_platform = &platform;
	profilerInit(game_args);

	PAW_LOG_INFO("Test");
	PAW_LOG_SUCCESS("Test");
	PAW_LOG_WARNING("Test");
	PAW_LOG_ERROR("Test");
	constexpr StringID test_id = PAW_STRING_ID("hey there");
	PAW_LOG_INFO("Hash Test %p{StringID}", &test_id);

	PAW_PROFILER_THREAD("Window Thread");

	string_registerFormatter(
		PAW_STRING_ID("WIN32_RECT"),
		[](byte* out_buffer, usize out_buffer_size_bytes, void const* data)
		{
			const RECT* rect = static_cast<const RECT*>(data);
			string_format(
				out_buffer,
				out_buffer_size_bytes,
				"{ %d, %d, %d, %d }",
				rect->left,
				rect->top,
				rect->right - rect->left,
				rect->bottom - rect->top);
		});

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	timeBeginPeriod(1);

	g_window_thread_wake_semaphore = platform.create_semaphore(0, 1);
	g_game_thread_wake_semaphore = platform.create_semaphore(0, 1);

	g_default_cursor = LoadCursor(NULL, IDC_ARROW);
	g_current_cursor = g_default_cursor;

	g_key_mapper[(int)'0'] = PlatformKeyType_Num0;
	g_key_mapper[(int)'1'] = PlatformKeyType_Num1;
	g_key_mapper[(int)'2'] = PlatformKeyType_Num2;
	g_key_mapper[(int)'3'] = PlatformKeyType_Num3;
	g_key_mapper[(int)'4'] = PlatformKeyType_Num4;
	g_key_mapper[(int)'5'] = PlatformKeyType_Num5;
	g_key_mapper[(int)'6'] = PlatformKeyType_Num6;
	g_key_mapper[(int)'7'] = PlatformKeyType_Num7;
	g_key_mapper[(int)'8'] = PlatformKeyType_Num8;
	g_key_mapper[(int)'9'] = PlatformKeyType_Num9;
	g_key_mapper[(int)'A'] = PlatformKeyType_A;
	g_key_mapper[(int)'B'] = PlatformKeyType_B;
	g_key_mapper[(int)'C'] = PlatformKeyType_C;
	g_key_mapper[(int)'D'] = PlatformKeyType_D;
	g_key_mapper[(int)'E'] = PlatformKeyType_E;
	g_key_mapper[(int)'F'] = PlatformKeyType_F;
	g_key_mapper[(int)'G'] = PlatformKeyType_G;
	g_key_mapper[(int)'H'] = PlatformKeyType_H;
	g_key_mapper[(int)'I'] = PlatformKeyType_I;
	g_key_mapper[(int)'K'] = PlatformKeyType_K;
	g_key_mapper[(int)'L'] = PlatformKeyType_L;
	g_key_mapper[(int)'M'] = PlatformKeyType_M;
	g_key_mapper[(int)'N'] = PlatformKeyType_N;
	g_key_mapper[(int)'O'] = PlatformKeyType_O;
	g_key_mapper[(int)'P'] = PlatformKeyType_P;
	g_key_mapper[(int)'Q'] = PlatformKeyType_Q;
	g_key_mapper[(int)'R'] = PlatformKeyType_R;
	g_key_mapper[(int)'S'] = PlatformKeyType_S;
	g_key_mapper[(int)'T'] = PlatformKeyType_T;
	g_key_mapper[(int)'U'] = PlatformKeyType_U;
	g_key_mapper[(int)'V'] = PlatformKeyType_V;
	g_key_mapper[(int)'W'] = PlatformKeyType_W;
	g_key_mapper[(int)'X'] = PlatformKeyType_X;
	g_key_mapper[(int)'Y'] = PlatformKeyType_Y;
	g_key_mapper[(int)'Z'] = PlatformKeyType_Z;
	g_key_mapper[VK_NUMPAD0] = PlatformKeyType_NumPad0;
	g_key_mapper[VK_NUMPAD1] = PlatformKeyType_NumPad1;
	g_key_mapper[VK_NUMPAD2] = PlatformKeyType_NumPad2;
	g_key_mapper[VK_NUMPAD3] = PlatformKeyType_NumPad3;
	g_key_mapper[VK_NUMPAD4] = PlatformKeyType_NumPad4;
	g_key_mapper[VK_NUMPAD5] = PlatformKeyType_NumPad5;
	g_key_mapper[VK_NUMPAD6] = PlatformKeyType_NumPad6;
	g_key_mapper[VK_NUMPAD7] = PlatformKeyType_NumPad7;
	g_key_mapper[VK_NUMPAD8] = PlatformKeyType_NumPad8;
	g_key_mapper[VK_NUMPAD9] = PlatformKeyType_NumPad9;
	g_key_mapper[VK_F1] = PlatformKeyType_F1;
	g_key_mapper[VK_F2] = PlatformKeyType_F2;
	g_key_mapper[VK_F3] = PlatformKeyType_F3;
	g_key_mapper[VK_F4] = PlatformKeyType_F4;
	g_key_mapper[VK_F5] = PlatformKeyType_F5;
	g_key_mapper[VK_F6] = PlatformKeyType_F6;
	g_key_mapper[VK_F7] = PlatformKeyType_F7;
	g_key_mapper[VK_F8] = PlatformKeyType_F8;
	g_key_mapper[VK_F9] = PlatformKeyType_F9;
	g_key_mapper[VK_F10] = PlatformKeyType_F10;
	g_key_mapper[VK_F11] = PlatformKeyType_F11;
	g_key_mapper[VK_F12] = PlatformKeyType_F12;
	g_key_mapper[VK_LEFT] = PlatformKeyType_Left;
	g_key_mapper[VK_RIGHT] = PlatformKeyType_Right;
	g_key_mapper[VK_UP] = PlatformKeyType_Up;
	g_key_mapper[VK_DOWN] = PlatformKeyType_Down;
	g_key_mapper[VK_SPACE] = PlatformKeyType_Space;
	g_key_mapper[VK_CONTROL] = PlatformKeyType_LCtrl;
	g_key_mapper[VK_SHIFT] = PlatformKeyType_LShift;
	// RCtrl is handled in event

	g_key_mapper[VK_ESCAPE] = PlatformKeyType_Escape;

	HRESULT result = S_OK;
	(void)result;
	PAW_ASSERT_UNUSED(result);

	if (!game_args.disable_gpu_debug)
	{
		ID3D12Debug6* debug_controller = nullptr;
		{
			ID3D12Debug6* dc = nullptr;
			result = D3D12GetDebugInterface(IID_PPV_ARGS(&dc));
			PAW_ASSERT(SUCCEEDED(result));

			result = dc->QueryInterface(IID_PPV_ARGS(&debug_controller));
			if (!SUCCEEDED(result))
			{
				PAW_LOG_ERROR("D3D: Failed to initialize debug controller");
			}
			// PAW_ASSERT(SUCCEEDED(result));
		}

		if (debug_controller)
		{
			debug_controller->EnableDebugLayer();
			debug_controller->SetEnableGPUBasedValidation(true);
		}
	}

	IDXGIFactory7* factory = nullptr;
	const UINT dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
	result = CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&factory));

	bool found_adapter = false;

	IDXGIAdapter1* adapter;
	for (UINT adapter_index = 0;
		 factory->EnumAdapterByGpuPreference(
			 adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
		 ++adapter_index)
	{
		CHAR vendor_name[128];
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (true)
		{
			size_t num_converted = 0;
			wcstombs_s(&num_converted, vendor_name, 128, desc.Description, 128);
			PAW_LOG_INFO("%ls", vendor_name);
			found_adapter = true;
			break;
		}

		adapter->Release();
	}

	PAW_ASSERT(found_adapter);

	IDXGIOutput* output = nullptr;
	for (u32 i = 0; adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);
		u32 dpi_x, dpi_y;
		GetDpiForMonitor(desc.Monitor, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
		PAW_ASSERT(dpi_x == dpi_y);
		PAW_LOG_INFO(
			"Monitor %u: %s\n\tRect: %d, %d, %d, %d\n\tScale: %g",
			i,
			desc.DeviceName,
			desc.DesktopCoordinates.left,
			desc.DesktopCoordinates.top,
			desc.DesktopCoordinates.right - desc.DesktopCoordinates.left,
			desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top,
			static_cast<double>(dpi_x) / static_cast<double>(USER_DEFAULT_SCREEN_DPI));
	}

	{
		// #TODO: Not use vectors for this
		std::vector<DISPLAYCONFIG_PATH_INFO> paths;
		std::vector<DISPLAYCONFIG_MODE_INFO> modes;

		const u32 flags = QDC_ONLY_ACTIVE_PATHS;

		u32 path_count, mode_count;

		result = GetDisplayConfigBufferSizes(flags, &path_count, &mode_count);
		PAW_ASSERT(SUCCEEDED(result));

		paths.resize(path_count);
		modes.resize(mode_count);

		// #TODO: Handle path_count and mode_count changing between call to GetDisplayConfigBufferSizes and this
		result = QueryDisplayConfig(flags, &path_count, paths.data(), &mode_count, modes.data(), nullptr);
		PAW_ASSERT(SUCCEEDED(result));

		adapter->EnumOutputs(0, &output);
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);
			u32 dpi_x, dpi_y;
			GetDpiForMonitor(desc.Monitor, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
			PAW_ASSERT(dpi_x == dpi_y);
			g_window_state.dpi = dpi_x;
			g_window_state.dpi_scale = static_cast<f32>(dpi_x) / static_cast<f32>(USER_DEFAULT_SCREEN_DPI);

			IDXGIOutput6* output_6 = nullptr;
			result = output->QueryInterface(&output_6);
			PAW_ASSERT(SUCCEEDED(result));

			DXGI_OUTPUT_DESC1 desc_1;
			result = output_6->GetDesc1(&desc_1);
			PAW_ASSERT(SUCCEEDED(result));

			game_args.hdr.enabled = desc_1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
			game_args.hdr.min_luminance = desc_1.MinLuminance;
			game_args.hdr.max_luminance = desc_1.MaxLuminance;
			game_args.hdr.max_full_frame_luminance = desc_1.MaxFullFrameLuminance;

			for (DISPLAYCONFIG_PATH_INFO& path : paths)
			{
				const u32 source_index = (path.flags & DISPLAYCONFIG_PATH_SUPPORT_VIRTUAL_MODE) ? path.sourceInfo.sourceModeInfoIdx : path.sourceInfo.modeInfoIdx;
				PAW_ASSERT(source_index != DISPLAYCONFIG_PATH_MODE_IDX_INVALID);

				const DISPLAYCONFIG_SOURCE_MODE& source_mode = modes[source_index].sourceMode;

				if (source_mode.position.x == desc.DesktopCoordinates.left && source_mode.position.y == desc.DesktopCoordinates.top && (s32)source_mode.width == desc.DesktopCoordinates.right - desc.DesktopCoordinates.left && (s32)source_mode.height == desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top)
				{
					DISPLAYCONFIG_SDR_WHITE_LEVEL white_level{
						.header = {
							.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL,
							.size = sizeof(DISPLAYCONFIG_SDR_WHITE_LEVEL),
							.adapterId = path.targetInfo.adapterId,
							.id = path.targetInfo.id,
						},
					};

					result = DisplayConfigGetDeviceInfo(&white_level.header);
					PAW_ASSERT(SUCCEEDED(result));

					game_args.hdr.white_sdr_scale = (f32)white_level.SDRWhiteLevel / 1000.0f;
				}
			}
		}

		g_window_state.x = 100;
		g_window_state.y = 100;
		g_window_state.client_width = game_args.on_laptop ? 1600 : 1920;
		g_window_state.client_height = game_args.on_laptop ? 900 : 1080;
	}

	/*g_window_state.x = 0;
	g_window_state.y = 0;
	g_window_state.client_width = 3840;
	g_window_state.client_height = 2160;*/

	const WNDCLASSEX window_class{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = &windowProc,
		.hInstance = GetModuleHandle(nullptr),
		.hCursor = g_default_cursor,
		.lpszClassName = "PawWindowClass",
	};

	RegisterClassEx(&window_class);

	RECT window_rect{
		0,
		0,
		g_window_state.client_width,
		g_window_state.client_height,
	};

	g_window_state.style = WS_OVERLAPPEDWINDOW;
	// g_window_state.style = WS_POPUP;

	{
		RECT new_window_rect = window_rect;
		AdjustWindowRectExForDpi(&new_window_rect, g_window_state.style, FALSE, 0, g_window_state.dpi);
		PAW_LOG_INFO("Adjust from %p{WIN32_RECT} to %p{WIN32_RECT}", &window_rect, &new_window_rect);
		window_rect = new_window_rect;
	}

	g_window_state.actual_width = window_rect.right - window_rect.left;
	g_window_state.actual_height = window_rect.bottom - window_rect.top;

	g_window_state.handle = CreateWindow(
		window_class.lpszClassName,
		"Pawprint Window",
		g_window_state.style,
		g_window_state.x,
		g_window_state.y,
		g_window_state.actual_width,
		g_window_state.actual_height,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		nullptr);

	ShowWindow(g_window_state.handle, SW_MAXIMIZE);

	{
		POINT start_mouse_pos_screen;
		GetCursorPos(&start_mouse_pos_screen);
		g_window_state.mouse_x = start_mouse_pos_screen.x;
		g_window_state.mouse_y = start_mouse_pos_screen.y;

		PAW_LOG_INFO("Start mouse pos: %d, %d", g_window_state.mouse_x, g_window_state.mouse_y);

		RAWINPUTDEVICE rid{
			.usUsagePage = HID_USAGE_PAGE_GENERIC,
			.usUsage = HID_USAGE_GENERIC_MOUSE,
			.dwFlags = RIDEV_INPUTSINK,
			.hwndTarget = g_window_state.handle,
		};

		RegisterRawInputDevices(&rid, 1, sizeof(rid));
	}

	/*IDXGIOutput* output = nullptr;
	for (u32 i = 0; adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_OUTPUT_DESC desc;
		result = output->GetDesc(&desc);
		PAW_ASSERT(SUCCEEDED(result));
	}*/

#ifdef PAW_ENABLE_IMGUI
	initImGui();
	ImGui_ImplWin32_Init(g_window_state.handle);
#endif

	// ImGui_ImplSDL2_InitForVulkan(g_window);
	// ImGui_ImplSDL2_InitForD3D(window);
	// ImGui_ImplDX12_Init(
	//	device,
	//	frame_count,
	//	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	//	nullptr,
	//	D3D12_CPU_DESCRIPTOR_HANDLE{main_descriptor_heap_start_handle.ptr + general_handle_size * 1},
	//	D3D12_GPU_DESCRIPTOR_HANDLE{
	//		main_descriptor_heap->GetGPUDescriptorHandleForHeapStart().ptr + general_handle_size * 1});

#ifdef PAW_ENABLE_IMGUI
	float const font_size = math_floor(14.0f * g_window_state.dpi_scale);
	ImGuiIO& io = ImGui::GetIO();

	io.FontDefault = io.Fonts->AddFontFromFileTTF("source_data/fonts/DroidSans.ttf", font_size);
	ImGui::GetStyle().ScaleAllSizes(g_window_state.dpi_scale);
#endif

	GameSystems* game = game_init(platform, game_args, adapter, factory);
	g_game_initialized = true;

	// gameProcessEvent(platform, PlatformEvent{
	//							   .focus_changed = {.focused = true},
	//							   .type = PlatformEventType_FocusChanged,
	//						   });

#if 0
	const char* dir_to_watch = "compiled_data";
	HANDLE watch_dir_handle = CreateFileA(
		dir_to_watch,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	Defer watch_dir_defer = {[watch_dir_handle]
							 { CloseHandle(watch_dir_handle); }};

	HANDLE iocp = CreateIoCompletionPort(watch_dir_handle, NULL, 0, 1);
	PAW_ASSERT(iocp != INVALID_HANDLE_VALUE);
	Defer iocp_defer = {[iocp]
						{
							PostQueuedCompletionStatus(iocp, 0, 0, nullptr);
							CloseHandle(iocp);
						}};

	FILE_NOTIFY_INFORMATION buffer[1024];
	char path[MAX_PATH];
	const DWORD filter = FILE_NOTIFY_CHANGE_LAST_WRITE;
	OVERLAPPED overlapped{};
	BOOL readResult =
		ReadDirectoryChangesW(watch_dir_handle, buffer, sizeof(buffer), TRUE, filter, nullptr, &overlapped, nullptr);
	PAW_ASSERT(readResult);
	PAW_ASSERT_UNUSED(readResult);

	byte str_buffer[MAX_PATH];

	// u64 last_frame_start_ticks_fs = platform.getCpuTicks();

	{
		PAW_PROFILER_SCOPE("Process file modifications")

		DWORD num_bytes = 0;
		BOOL gqc_result = FALSE;
		LPOVERLAPPED ov;
		ULONG_PTR key;
		while ((gqc_result = GetQueuedCompletionStatus(iocp, &num_bytes, &key, &ov, 0)) != FALSE)
		{
			if (key != 0)
			{
				break;
			}

			DWORD offset = 0;
			FILE_NOTIFY_INFORMATION* info = buffer + offset;
			do
			{
				info = buffer + offset;
				offset += info->NextEntryOffset;
				int str_len = WideCharToMultiByte(
					CP_UTF8,
					0,
					info->FileName,
					info->FileNameLength / sizeof(WCHAR),
					path,
					MAX_PATH - 1,
					nullptr,
					nullptr);

				UTF8StringView file_path =
					string_formatNullTerminated(str_buffer, MAX_PATH, "%s/%.*s", dir_to_watch, str_len, path);
				for (usize i = 0; i < file_path.size_bytes; i++)
				{
					if (str_buffer[i] == '\\')
					{
						str_buffer[i] = '/';
					}
				}

				constexpr static UTF8StringView action_names[] = {
					PAW_STR("Added", 5),
					PAW_STR("Removed", 7),
					PAW_STR("Modified", 8),
					PAW_STR("Renamed old name", 16),
					PAW_STR("Renamed new name", 16),
				};

				PAW_LOG_INFO("%p{str}: %p{str}", &action_names[info->Action - 1], &file_path);

				switch (info->Action)
				{

					case FILE_ACTION_MODIFIED:
					{

						// Sleep(500);
						const u64 start_ticks = platform.getCpuTicks();
						// gameProcessEvent(platform, PlatformEvent{.file_update = {.path = file_path}});
						Sleep(16);
						const u64 end_ticks = platform.getCpuTicks();
						const u64 total_ticks = end_ticks - start_ticks;
						PAW_LOG_INFO(
							"File Update took %fms",
							static_cast<f64>(total_ticks) / static_cast<f64>(platform.getTicksPerSecond()) *
								1000.0);
					}
					break;
				}
			} while (info->NextEntryOffset != 0);

			readResult = ReadDirectoryChangesW(
				watch_dir_handle, buffer, sizeof(buffer), TRUE, filter, nullptr, &overlapped, nullptr);
			PAW_ASSERT(readResult);
			PAW_ASSERT_UNUSED(readResult);
		}
	}
#endif

	g_running = true;

	while (g_running)
	{
		platform.wait_for_semaphore(g_window_thread_wake_semaphore);

		if (!g_running)
		{
			break;
		}

		PAW_PROFILER_SCOPE("Event processing");

		for (usize i = 0; i < PlatformKeyType_Count; ++i)
		{
			g_game_input_state.keys[i].half_transition_count = 0;
		}

		g_game_input_state.raw_mouse_move_delta = Vec2S32{};
		g_game_input_state.mouse_wheel_delta = Vec2S32{};

		MSG msg{};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_QUIT:
				{
					g_current_game_event_data->quit_requested = true;
				}
				break;

				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				break;
			}
		}

#ifdef PAW_ENABLE_IMGUI
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#endif

		g_current_game_event_data->input = g_game_input_state;
		g_current_game_event_data = nullptr;

		platform.signal_semaphore(g_game_thread_wake_semaphore);
	}

	game_deinit(game, platform);

#ifdef PAW_ENABLE_IMGUI
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

	platform.destroy_semaphore(g_window_thread_wake_semaphore);
	platform.destroy_semaphore(g_game_thread_wake_semaphore);

	return 0;
}

Slice<byte> Platform::DEBUG_load_file_blocking(UTF8StringView const& path, Allocator& allocator)
{
	PAW_ASSERT(path.null_terminated);
	HANDLE file = CreateFile(path.ptr, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	PAW_ASSERT(file != INVALID_HANDLE_VALUE);

	LARGE_INTEGER file_size{};
	GetFileSizeEx(file, &file_size);
	PAW_ASSERT(file_size.QuadPart > 0);

	Slice<byte> file_mem = allocator.alloc<byte>(file_size.QuadPart);
	DWORD bytes_read = 0;
	ReadFile(file, file_mem.ptr, static_cast<DWORD>(file_size.QuadPart), &bytes_read, nullptr);
	PAW_ASSERT(bytes_read == static_cast<DWORD>(file_size.QuadPart));
	CloseHandle(file);
	return file_mem;
}

struct SystemAllocator final : public Allocator
{
	virtual Slice<byte> alloc_bytes(usize size, usize) override
	{
		return Slice<byte>{
			reinterpret_cast<byte*>(VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)),
			size,
		};
	}

	virtual void free_bytes(Slice<byte> mem, usize) override
	{
		VirtualFree(mem.ptr, 0, MEM_RELEASE);
	}
};

static SystemAllocator g_system_allocator{};

Allocator& Platform::get_system_allocator()
{
	return g_system_allocator;
}

PlatformThread Platform::create_thread(UTF8StringView const& name, PlatformThreadFunc* func, void* data)
{
	HANDLE thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)func, data, CREATE_SUSPENDED, nullptr);
	PAW_ASSERT_UNUSED(name);
	PAW_ASSERT(name.null_terminated);
	PAW_ASSERT(thread);
	return PlatformThread{(u64)thread};
}

void Platform::destroy_thread(PlatformThread thread)
{
	PAW_ASSERT(thread.handle);
	CloseHandle((HANDLE)thread.handle);
}

void Platform::wait_for_thread(PlatformThread thread)
{
	PAW_ASSERT(thread.handle);
	WaitForSingleObject((HANDLE)thread.handle, INFINITE);
}

void Platform::start_thread(PlatformThread thread)
{
	PAW_ASSERT(thread.handle);
	ResumeThread(HANDLE(thread.handle));
}

PlatformSemaphore Platform::create_semaphore(u32 initial_value, u32 maximum_value)
{
	HANDLE semaphore = CreateSemaphore(nullptr, initial_value, maximum_value, nullptr);
	PAW_ASSERT(semaphore);
	return PlatformSemaphore{(u64)semaphore};
}

void Platform::destroy_semaphore(PlatformSemaphore semaphore)
{
	PAW_ASSERT(semaphore.handle);
	CloseHandle((HANDLE)semaphore.handle);
}

void Platform::wait_for_semaphore(PlatformSemaphore semaphore)
{
	PAW_ASSERT(semaphore.handle);
	WaitForSingleObject((HANDLE)semaphore.handle, INFINITE);
}

void Platform::signal_semaphore(PlatformSemaphore semaphore)
{
	PAW_ASSERT(semaphore.handle);
	ReleaseSemaphore((HANDLE)semaphore.handle, 1, nullptr);
}

PlatformWaitHandle Platform::get_wait_handle(PlatformSemaphore semaphore)
{
	return PlatformWaitHandle{semaphore.handle};
}

PlatformMutex Platform::create_mutex()
{
	HANDLE handle = CreateMutex(nullptr, false, nullptr);
	PAW_ASSERT(handle);
	return PlatformMutex{(u64)handle};
}

void Platform::destroy_mutex(PlatformMutex mutex)
{
	PAW_ASSERT(mutex.handle);
	CloseHandle((HANDLE)mutex.handle);
}

void Platform::lock_mutex(PlatformMutex mutex)
{
	PAW_ASSERT(mutex.handle);
	DWORD result = WaitForSingleObject((HANDLE)mutex.handle, INFINITE);
	PAW_ASSERT_UNUSED(result);
	PAW_ASSERT(result == WAIT_OBJECT_0);
}

void Platform::unlock_mutex(PlatformMutex mutex)
{
	PAW_ASSERT(mutex.handle);
	BOOL result = ReleaseMutex((HANDLE)mutex.handle);
	PAW_ASSERT_UNUSED(result);
	PAW_ASSERT(result);
}

PlatformWaitTimer Platform::create_timer()
{
	HANDLE handle = CreateWaitableTimerEx(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	PAW_ASSERT(handle);
	return PlatformWaitTimer{u64(handle)};
}

void Platform::destroy_timer(PlatformWaitTimer timer)
{
	PAW_ASSERT(timer.handle);
	CloseHandle(HANDLE(timer.handle));
}

void Platform::set_timer_wait_time(PlatformWaitTimer timer, u32 microseconds)
{
	const u32 nanoseconds_per_microsecond = 1000;
	const u32 nanoseconds_per_interval = 100;
	const u32 intervals_per_microsecond = nanoseconds_per_microsecond / nanoseconds_per_interval;
	const s64 intervals = s64(intervals_per_microsecond) * s64(microseconds);
	LARGE_INTEGER wait_time{};
	wait_time.QuadPart = -intervals;
	BOOL result = SetWaitableTimer(HANDLE(timer.handle), &wait_time, 0, NULL, NULL, FALSE);
	PAW_ASSERT(result);
	PAW_ASSERT_UNUSED(result);
}

PlatformWaitHandle Platform::get_wait_handle(PlatformWaitTimer timer)
{
	return PlatformWaitHandle{timer.handle};
}

PlatformAutoResetEvent Platform::create_auto_reset_event()
{
	HANDLE handle = CreateEvent(NULL, FALSE, FALSE, NULL);
	PAW_ASSERT(handle);
	return PlatformAutoResetEvent{u64(handle)};
}

void Platform::destroy_auto_reset_event(PlatformAutoResetEvent event)
{
	PAW_ASSERT(event.handle);
	CloseHandle(HANDLE(event.handle));
}

PlatformWaitHandle Platform::get_wait_handle(PlatformAutoResetEvent event)
{
	return PlatformWaitHandle{event.handle};
}

usize Platform::wait_for_any(Slice<PlatformWaitHandle const> const& waitables)
{
	PAW_ASSERT(waitables.size < MAXIMUM_WAIT_OBJECTS);
	// PAW_ASSERT(out_completed.size >= waitables.size);
	const u32 wait_count = u32(waitables.size);
	const DWORD result = WaitForMultipleObjects(wait_count, (const HANDLE*)waitables.ptr, FALSE, INFINITE);
	PAW_ASSERT(result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + wait_count);
	PAW_ASSERT_UNUSED(result);

	return usize(result - WAIT_OBJECT_0);

	/*PAW_ASSERT(result == WAIT_OBJECT_0);

	usize completed_count = 0;
	for (usize i = 0; i < waitables.size; ++i)
	{
		const PlatformWaitHandle& handle = waitables[i];
		const HANDLE win32_handle = HANDLE(handle.handle);
		const DWORD wait_result = WaitForSingleObject(win32_handle, 0);
		if (usize(wait_result) == WAIT_OBJECT_0 + i)
		{
			out_completed[completed_count++] = i;
		}
	}
	out_completed.size = completed_count;*/
}

u64 Platform::get_cpu_ticks()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

u64 Platform::get_tcks_per_second()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
}

void Platform::sleep_ms(u64 ms)
{
	Sleep(static_cast<DWORD>(ms));
}

static WORD g_severity_attributes_lookup[] = {
	/* LogSeverity_Info */ FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	/* LogSeverity_Success */ FOREGROUND_GREEN,
	/* LogSeverity_Warning */ FOREGROUND_RED | FOREGROUND_GREEN,
	/* LogSeverity_Error */ BACKGROUND_RED,
};

static UTF8StringView g_severity_prefixes[] = {
	/* LogSeverity_Info */ PAW_STR("Info: ", 6),
	/* LogSeverity_Success */ PAW_STR("Success: ", 9),
	/* LogSeverity_Warning */ PAW_STR("Warning: ", 9),
	/* LogSeverity_Error */ PAW_STR("Error: ", 7),
};

static tracy::Color::ColorType g_severity_tracy_color_lookup[] = {
	/* LogSeverity_Info */ tracy::Color::White,
	/* LogSeverity_Success */ tracy::Color::Green,
	/* LogSeverity_Warning */ tracy::Color::Orange,
	/* LogSeverity_Error */ tracy::Color::Red,
};

void logger_func(LogSeverity severity, char const* text, usize len)
{
	PAW_ASSERT(severity >= LogSeverity_Info && severity <= LogSeverity_Error);

	if(TracyIsConnected) 
	{
		TracyMessageC(text, len, g_severity_tracy_color_lookup[severity]);	
	}

	const HANDLE console_handle = severity < LogSeverity_Error ? GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO screen_info;
	bool result = GetConsoleScreenBufferInfo(console_handle, &screen_info);
	PAW_ASSERT(result);
	PAW_ASSERT_UNUSED(result);

	result = SetConsoleTextAttribute(console_handle, g_severity_attributes_lookup[severity]);
	PAW_ASSERT(result);
	DWORD written = 0;

	UTF8StringView const& prefix = g_severity_prefixes[severity];
	WriteConsole(console_handle, prefix.ptr, static_cast<DWORD>(prefix.size_bytes), &written, nullptr);
	PAW_ASSERT(static_cast<usize>(written) == prefix.size_bytes);

	WriteConsole(console_handle, text, static_cast<DWORD>(len), &written, nullptr);
	PAW_ASSERT(static_cast<usize>(written) == len);

	result = SetConsoleTextAttribute(console_handle, screen_info.wAttributes);
	PAW_ASSERT(result);

	WriteConsole(console_handle, "\n", 1, &written, nullptr);
	PAW_ASSERT(written == 1);
}

Vec2S32 Platform::get_drawable_size()
{
	return Vec2S32{g_window_state.client_width, g_window_state.client_height};
}

void Platform::set_cursor_visible(bool visible)
{
	if (visible)
	{
		PAW_LOG_INFO("Set cursor visible");
		g_current_cursor = g_default_cursor;
	}
	else
	{
		PAW_LOG_INFO("Set cursor hidden");
		g_current_cursor = NULL;
	}
	SetCursor(g_current_cursor);
}

void Platform::set_cursor_position(s32 x, s32 y)
{
	POINT pos{x, y};
	ClientToScreen(g_window_state.handle, &pos);
	SetCursorPos(pos.x, pos.y);
}

void Platform::set_mouse_locked(bool locked)
{
	g_window_state.mouse_locked = locked;
	if (locked)
	{
		clipCursorToCurrentWindow();
	}
	else
	{
		ClipCursor(nullptr);
	}
}

f32 Platform::get_dpi_Scale()
{
	return g_window_state.dpi_scale;
}

void Platform::pump_events(GameEventData& out_data)
{
	PAW_PROFILER_FUNC();
	g_current_game_event_data = &out_data;
	signal_semaphore(g_window_thread_wake_semaphore);
	wait_for_semaphore(g_game_thread_wake_semaphore);
}

void Platform::request_quit()
{
	g_running = false;
	signal_semaphore(g_window_thread_wake_semaphore);
}

#if PAW_TESTS
// int tests()
//{
//	testing::init(g_page_pool_allocator);
//	Core::tests();
//	const usize fail_count = testing::getFailedCount();
//	if (fail_count > 0)
//	{
//		PAW_LOG_ERROR("%lu tests have failed!", fail_count);
//		return -1;
//	}
//	else
//	{
//		PAW_LOG_SUCCESS("All tests passed!");
//		return 0;
//	}
// }
#endif

//__declspec(dllexport) void doThing()
//{
//}

extern "C"
{
	__declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
}

extern "C"
{
	__declspec(dllexport) extern char const* D3D12SDKPath = ".\\";
}

int main(int arg_count, char* args[])
{
	bool test = false;
	bool list_all_tests = false;

	GameStartupArgs game_args{};

	for (int arg_index = 0; arg_index < arg_count; arg_index++)
	{
		char const* arg = args[arg_index];
		char const* formatted_arg = arg + 1;
		const size_t arg_len = strlen(arg);
		const StringID hash = hashCStringToStringID(formatted_arg, arg_len - 1);
		switch (hash)
		{
			case PAW_STRING_ID("profiler_wait_for_connection"):
			{
				game_args.profiler_wait_for_connection = true;
			}
			break;

			case PAW_STRING_ID("enable_gpu_profiler"):
			{
				game_args.enable_gpu_profiler = true;
			}
			break;

			case PAW_STRING_ID("test"):
			{
				test = true;
			}
			break;

			case PAW_STRING_ID("list_all_tests"):
			{
				list_all_tests = true;
			}
			break;

			case PAW_STRING_ID("disable_gpu_debug"):
			{
				game_args.disable_gpu_debug = true;
			}
			break;

			case PAW_STRING_ID("on_laptop"):
			{
				game_args.on_laptop = true;
			}
			break;

			case PAW_STRING_ID("port"):
			{
				/*previous_was_port = true;*/
			}
			break;

			case PAW_STRING_ID("editor_mode"):
			{
				game_args.editor_mode = true;
			}
			break;
		}
	}

	/*	bool previous_was_port = false;

		for (usize char_index = 0;; char_index++)
		{
			char c = cmd_line[char_index];
			if (c == ' ' || c == 0)
			{
				const char* arg = cmd_line + cmd_start;
				if (cmd_len == 0)
					continue;

				if (previous_was_port)
				{
					game_args.port = 0;

					for (usize i = 0; i < cmd_len; i++)
					{
						const byte b = arg[i] - '0';
						game_args.port += (u16)(b * math_pow(10, (f32)cmd_len - i - 1));
					}
				}

				if (arg[0] != '/')
				{
					fprintf(stderr, "Argument: \"%s\" is not in the correct format (/<argument name>)", arg);
				}
			}

			cmd_start = char_index + 1;
			cmd_len = 0;

			if (c == 0)
			{
				break;
			}
			continue;
		}

		cmd_len++;
	}*/
	Core::init();

	if (test)
	{
		return testingRunAllTests();
	}
	else if (list_all_tests)
	{
		return testingListAllTests();
	}

	Platform platform{};

	PAW_LOG_INFO("Port: %u", game_args.port);

	appMain(platform, game_args);
	return 0;
}