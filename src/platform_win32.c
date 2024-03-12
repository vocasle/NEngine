#include "platform.h"

#if NE_PLATFORM_WIN32

#include <Windows.h>
#include <stdio.h>

struct NE_Window 
{
	HWND window;
	HINSTANCE handle;
};

LRESULT win32_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
	{
		if (wparam == VK_ESCAPE) {
			PostQuitMessage(0);
			return 0;
		}
		break;
	}
	}
	return DefWindowProcA(hwnd, message, wparam, lparam);
}

struct NE_Window *ne_platform_create_window(const i8 *title, i16 x, i16 y, u16 width, u16 height)
{
	WNDCLASSA wc = { 0 };
	wc.lpfnWndProc = win32_proc;
	wc.lpszClassName = title;

	ATOM class_uid = RegisterClassA(&wc);
	(void)class_uid;
	
	HWND hwnd = CreateWindowExA(0, title, title, WS_OVERLAPPEDWINDOW, x, y, width, height, NULL, NULL, NULL, NULL);
	if (!hwnd) {
		ne_platform_println("Failed to create window", NE_LOG_LEVEL_ERROR);
		return NULL;
	}

	struct NE_Window *w = malloc(sizeof *w);
	w->window = hwnd;

	ShowWindow(hwnd, SW_MAXIMIZE);

	return w;
}

void ne_platform_destroy_window(struct NE_Window *w)
{
	DestroyWindow(w->window);
	free(w);
}

void ne_platform_pump_messages(struct NE_Window *w)
{
	(void)w;
    MSG msg = { 0 };
    while (GetMessageA(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void ne_platform_println(const i8 *msg, enum NE_LogLevel level)
{
	HANDLE h = level == NE_LOG_LEVEL_ERROR ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
	WORD old_color_attr = 0;
	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	GetConsoleScreenBufferInfo(h, &csbi);
	old_color_attr = csbi.wAttributes;

	if (level == NE_LOG_LEVEL_ERROR) {
		SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (level == NE_LOG_LEVEL_DEBUG) {
		SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else if (level == NE_LOG_LEVEL_WARNING) {
		SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (level == NE_LOG_LEVEL_INFO) {
		SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}

	OutputDebugStringA(msg);
	u64 num_written = 0;
	WriteConsoleA(h, msg, strlen(msg), (LPDWORD)&num_written, NULL);
	SetConsoleTextAttribute(h, old_color_attr);
}

void ne_platform_terminate(void)
{
	FatalAppExitA(0, "Platform termination requested");
}

struct NE_Library
{
	HINSTANCE lib;
	const i8 *name;
};
struct NE_Library *ne_platform_load_library(const i8 *name)
{
	struct NE_Library *lib = malloc(sizeof *lib);
	lib->name = name;
	lib->lib = LoadLibraryA(name);
	return lib;
}

void *ne_platform_get_proc_address(struct NE_Library *lib, const i8 *proc)
{
	return (void*)GetProcAddress(lib->lib, proc);
}

void ne_platform_destroy_library(struct NE_Library *lib)
{
	FreeLibrary(lib->lib);
	free(lib);
}

struct MemoryChunkInfo
{
	u16 head;
	u16 tail;
	u64 size;
	const i8 *file;
	i32 line;
};

#define NE_HEAD_GUARD 0xF0
#define NE_TAIL_GUARD 0x0D

void *ne_platform_allocate_dbg(u64 size, const i8 *file, i32 line)
{
	const u64 effective_size = size + sizeof(struct MemoryChunkInfo);
	void *bytes = malloc(effective_size);
	ne_platform_zero_memory(bytes, effective_size);
	struct MemoryChunkInfo *info = bytes;
	info->head = NE_HEAD_GUARD;
	info->tail = NE_TAIL_GUARD;
	info->size = size;
	info->file = file;
	info->line = line;
	info++;
	return info;
}

void ne_platform_free_dbg(void *ptr, const i8 *file, i32 line)
{
	struct MemoryChunkInfo *info = ptr;
	--info;

	i8 log_buf[1024] = { 0 };
	if (info->head != NE_HEAD_GUARD) {
		snprintf(log_buf, 1024, "FATAL ERROR: Head guard corrupted. Allocated at %s:%d, freed at: %s:%d",
			info->file, info->line, file, line);
		ne_platform_println(log_buf, NE_LOG_LEVEL_ERROR);
		ne_platform_terminate();
	}
	if (info->tail != NE_TAIL_GUARD) {
		snprintf(log_buf, 1024, "FATAL ERROR: Tail guard corrupted. Allocated at %s:%d, freed at: %s:%d",
			info->file, info->line, file, line);
		ne_platform_println(log_buf, NE_LOG_LEVEL_ERROR);
		ne_platform_terminate();
	}
	free(info);
}

#if NE_RELEASE
void *ne_platform_allocate(u64 size)
{
	return malloc(size);
}

void ne_platform_free(void *ptr)
{
	free(ptr);
}
#endif

void ne_platform_zero_memory(void *ptr, u64 size)
{
	memset(ptr, 0, size);
}

#endif
