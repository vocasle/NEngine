#include "platform.h"

#if NE_PLATFORM_WIN32

#include <Windows.h>
#include <strsafe.h>

struct NE_Window 
{
	HWND window;
	HINSTANCE handle;
};

LRESULT win32_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message) {
//	case WM_PAINT:
//	{
//		printf("WM_PAINT received...\n");
//		return 0;
//	}
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
		fprintf(stderr, "FATAL ERROR: Failed to create window\n");
		return NULL;
	}

	struct NE_Window *w = malloc(sizeof *w);
	w->window = hwnd;

	ShowWindow(hwnd, SW_MAXIMIZE);

	return w;
}

void ne_platform_destroy_window(struct NE_Window *w)
{
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

#endif
