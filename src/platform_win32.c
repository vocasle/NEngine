#include "platform.h"

#if NE_PLATFORM_WIN32

#include <Windows.h>

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


#endif
