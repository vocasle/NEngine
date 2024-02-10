#include "platform.h"

#if NE_PLATFORM_WIN32

#include <Windows.h>
#include <strsafe.h>

struct NE_Window 
{
	HWND window;
	HINSTANCE handle;
};

LRESULT window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  (void)hwnd;
  (void)message;
  (void)wparam;
  (void)lparam;
  switch(message) {
    case WM_CLOSE:
    DestroyWindow(hwnd);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProc(hwnd, message, wparam, lparam);
}

NE_API struct NE_Window *ne_platform_create_window(const i8 *title, i16 x, i16 y, u16 width, u16 height)
{
	struct NE_Window *w = malloc(sizeof *w);
	w->handle = GetModuleHandleA(NULL);

	WNDCLASSA wc = { 0 };
	wc.lpfnWndProc = window_proc;
	wc.hInstance = w->handle;
  wc.lpszClassName = "NEngine_Window_Class";

  if (!RegisterClassA(&wc)) {
    MessageBoxA(NULL, "Failed to register window class", "FATAL ERROR", MB_OK);
    exit(-1);
  }

  w->window = CreateWindowExA(0, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, x, y, width, height, NULL, NULL, w->handle, NULL);
  if (!w->window) {
    const DWORD err = GetLastError();
    LPSTR message_buffer = NULL;
    TCHAR format_buffer[512] = { 0 };
    
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message_buffer, 0, NULL);

    StringCbPrintfA(format_buffer, sizeof(format_buffer), "%lu - %s", err, message_buffer);

    MessageBoxA(NULL, format_buffer, "FATAL ERROR", MB_OK);
    LocalFree(message_buffer);
    exit(-1);
  }

  ShowWindow(w->window, SW_SHOW);

	return w;
}

NE_API void ne_platform_destroy_window(struct NE_Window *w)
{
	free(w);
}


NE_API b8 ne_platform_pump_messages(struct NE_Window *w)
{
  (void)w;
  MSG m = { 0 };
  while (GetMessage(&m, NULL, 0, 0) > 0) {
    TranslateMessage(&m);
    DispatchMessage(&m);
  }
  return NE_TRUE;
}

#endif
