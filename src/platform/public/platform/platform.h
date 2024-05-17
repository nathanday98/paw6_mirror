#pragma once

#include <shared/std.h>
#include <shared/math.h>
#include <shared/slice.h>
#include <shared/string.h>

struct Allocator;
struct GameEventData;
struct StringID;
template <typename T>
struct Slice;

enum MouseButton
{
	MouseButton_Left,
	MouseButton_Middle,
	MouseButton_Right,
	MouseButton_Count,
};

enum PlatformKeyType
{
	PlatformKeyType_Unknown,
	PlatformKeyType_Num0,
	PlatformKeyType_Num1,
	PlatformKeyType_Num2,
	PlatformKeyType_Num3,
	PlatformKeyType_Num4,
	PlatformKeyType_Num5,
	PlatformKeyType_Num6,
	PlatformKeyType_Num7,
	PlatformKeyType_Num8,
	PlatformKeyType_Num9,
	PlatformKeyType_A,
	PlatformKeyType_B,
	PlatformKeyType_C,
	PlatformKeyType_D,
	PlatformKeyType_E,
	PlatformKeyType_F,
	PlatformKeyType_G,
	PlatformKeyType_H,
	PlatformKeyType_I,
	PlatformKeyType_J,
	PlatformKeyType_K,
	PlatformKeyType_L,
	PlatformKeyType_M,
	PlatformKeyType_N,
	PlatformKeyType_O,
	PlatformKeyType_P,
	PlatformKeyType_Q,
	PlatformKeyType_R,
	PlatformKeyType_S,
	PlatformKeyType_T,
	PlatformKeyType_U,
	PlatformKeyType_V,
	PlatformKeyType_W,
	PlatformKeyType_X,
	PlatformKeyType_Y,
	PlatformKeyType_Z,
	PlatformKeyType_NumPad0,
	PlatformKeyType_NumPad1,
	PlatformKeyType_NumPad2,
	PlatformKeyType_NumPad3,
	PlatformKeyType_NumPad4,
	PlatformKeyType_NumPad5,
	PlatformKeyType_NumPad6,
	PlatformKeyType_NumPad7,
	PlatformKeyType_NumPad8,
	PlatformKeyType_NumPad9,
	PlatformKeyType_F1,
	PlatformKeyType_F2,
	PlatformKeyType_F3,
	PlatformKeyType_F4,
	PlatformKeyType_F5,
	PlatformKeyType_F6,
	PlatformKeyType_F7,
	PlatformKeyType_F8,
	PlatformKeyType_F9,
	PlatformKeyType_F10,
	PlatformKeyType_F11,
	PlatformKeyType_F12,
	PlatformKeyType_Left,
	PlatformKeyType_Right,
	PlatformKeyType_Up,
	PlatformKeyType_Down,
	PlatformKeyType_RCtrl,
	PlatformKeyType_LCtrl,
	PlatformKeyType_LShift,
	PlatformKeyType_RShift,
	PlatformKeyType_Escape,
	PlatformKeyType_Enter,
	PlatformKeyType_Tab,
	PlatformKeyType_LAlt,
	PlatformKeyType_RAlt,
	PlatformKeyType_PageUp,
	PlatformKeyType_PageDown,
	PlatformKeyType_Home,
	PlatformKeyType_End,
	PlatformKeyType_Delete,
	PlatformKeyType_Backspace,
	PlatformKeyType_LGui,
	PlatformKeyType_RGui,
	PlatformKeyType_Space,
	PlatformKeyType_Count,
};

enum PlatformEventType
{
	PlatformEventType_Unknown,
	PlatformEventType_Resize,
	PlatformEventType_WindowMove,
	PlatformEventType_FileUpdate,
};

struct PlatformEvent
{
	union
	{

		struct
		{
			s32 width, height;
		} resize;

		struct
		{
			s32 x, y;
		} move;

		struct
		{
			UTF8StringView path;
		} file_update;
	};

	PlatformEventType type;
};

struct PlatformThread
{
	u64 handle;
};

typedef void PlatformThreadFunc(void*);

struct PlatformSemaphore
{
	u64 handle;
};

struct PlatformWaitHandle
{
	u64 handle;
};

struct PlatformMutex
{
	u64 handle;
};

struct PlatformWaitTimer
{
	u64 handle;
};

struct PlatformAutoResetEvent
{
	u64 handle;
};

struct Platform
{
	Slice<byte> DEBUG_load_file_blocking(UTF8StringView const& path, Allocator& allocator);

	Allocator& get_system_allocator();

	PlatformThread create_thread(UTF8StringView const& name, PlatformThreadFunc* func, void* data);
	void destroy_thread(PlatformThread thread);
	void start_thread(PlatformThread thread);
	void wait_for_thread(PlatformThread thread);

	PlatformSemaphore create_semaphore(u32 initial_value, u32 maximum_value);
	void destroy_semaphore(PlatformSemaphore semaphore);
	void wait_for_semaphore(PlatformSemaphore semaphore);
	void signal_semaphore(PlatformSemaphore semaphore);
	PlatformWaitHandle get_wait_handle(PlatformSemaphore semaphore);

	PlatformMutex create_mutex();
	void destroy_mutex(PlatformMutex mutex);
	void lock_mutex(PlatformMutex mutex);
	void unlock_mutex(PlatformMutex mutex);

	PlatformWaitTimer create_timer();
	void destroy_timer(PlatformWaitTimer timer);
	void set_timer_wait_time(PlatformWaitTimer timer, u32 microseconds);
	PlatformWaitHandle get_wait_handle(PlatformWaitTimer timer);

	PlatformAutoResetEvent create_auto_reset_event();
	void destroy_auto_reset_event(PlatformAutoResetEvent event);
	PlatformWaitHandle get_wait_handle(PlatformAutoResetEvent event);

	usize wait_for_any(Slice<PlatformWaitHandle const> const& waitables);

	u64 get_cpu_ticks();
	u64 get_tcks_per_second();

	void sleep_ms(u64 ms);

	Vec2S32 get_drawable_size();

	void set_cursor_visible(bool visible);
	void set_cursor_position(s32 x, s32 y);
	void set_mouse_locked(bool locked);

	f32 get_dpi_Scale();

	void pump_events(GameEventData& out_data);
	void request_quit();
};
