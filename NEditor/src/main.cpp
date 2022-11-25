#include <Windows.h>

#include "NEditor/Editor.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Utils/Utils.h"

#if WITH_IMGUI
#include <imgui/imgui_impl_win32.h>
#endif

using namespace NEngine::Input;
using namespace NEditor;

#define MAX_LOADSTRING 1000

// Global Variables:
HINSTANCE hInst;                                        // current instance
constexpr wchar_t szTitle[MAX_LOADSTRING] = L"Editor";  // The title bar text
constexpr wchar_t szWindowClass[MAX_LOADSTRING] =
    L"EditorWindowClass";  // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int, Editor *);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

HMODULE
GetCurrentModule()
{  // NB: XP+ solution!
    HMODULE hModule = NULL;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                      (LPCTSTR)GetCurrentModule,
                      &hModule);

    return hModule;
}

namespace {
std::unique_ptr<Editor> editor = nullptr;
}

int APIENTRY
wWinMain(_In_ HINSTANCE hInstance,
         _In_opt_ HINSTANCE hPrevInstance,
         _In_ LPWSTR lpCmdLine,
         _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    // LoadStringW(NULL, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    // LoadStringW(NULL, IDC_DX11, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    editor = std::make_unique<Editor>();

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow, editor.get())) {
        printf("ERROR: InitInstance failed!\n");
        return FALSE;
    }

    // HACCEL hAccelTable =
    // 	LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX11));

    MSG msg = {};

    // Main message loop:
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            editor->Tick();
        }
    }

    return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM
MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = L"Menu name";
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = nullptr;

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int, Editor*)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable
//        and create and display the main program window.
//
BOOL
InitInstance(HINSTANCE hInstance, int nCmdShow, Editor *editor)
{
    hInst = hInstance;  // Store instance handle in our global variable

    uint32_t width = 0;
    uint32_t height = 0;

    editor->GetDefaultSize(&width, &height);

    RECT rc = {0, 0, (LONG)width, (LONG)height};

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

    HWND hWnd = CreateWindow(szWindowClass,
                             szTitle,
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             rc.right - rc.left,
                             rc.bottom - rc.top,
                             NULL,
                             NULL,
                             hInstance,
                             NULL);

    if (!hWnd) {
        printf("ERROR: hWnd is NULL!\n");
        return FALSE;
    }

    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)editor);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    GetClientRect(hWnd, &rc);

    editor->Initialize(hWnd, rc.right - rc.left, rc.bottom - rc.top);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL s_in_sizemove = FALSE;
    static BOOL s_in_suspend = FALSE;
    static BOOL s_minimized = FALSE;
    static BOOL s_fullscreen = FALSE;

#if WITH_IMGUI
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

#endif

    switch (message) {
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId) {
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        } break;
        case WM_PAINT:
            if (s_in_sizemove && editor) {
                editor->Tick();
            }
            else {
                PAINTSTRUCT ps;
                (void)BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
            }
            break;

        case WM_MOVE:
            if (editor) {
                // OnWindowMoved();
            }
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED) {
                if (!s_minimized) {
                    s_minimized = TRUE;
                    if (!s_in_suspend && editor)
                        // OnSuspending();
                        s_in_suspend = TRUE;
                }
            }
            else if (s_minimized) {
                s_minimized = FALSE;
                if (s_in_suspend && editor)
                    // OnResuming();
                    s_in_suspend = FALSE;
            }
            else if (!s_in_sizemove && editor) {
                editor->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
            }
            break;

        case WM_ENTERSIZEMOVE:
            s_in_sizemove = TRUE;
            break;

        case WM_EXITSIZEMOVE:
            s_in_sizemove = FALSE;
            if (editor) {
                RECT rc;
                GetClientRect(hWnd, &rc);

                editor->OnWindowSizeChanged(rc.right - rc.left,
                                            rc.bottom - rc.top);
            }
            break;

        case WM_GETMINMAXINFO:
            if (lParam) {
                MINMAXINFO *info = (MINMAXINFO *)(lParam);
                info->ptMinTrackSize.x = 320;
                info->ptMinTrackSize.y = 200;
            }
            break;

        case WM_ACTIVATEAPP:
            if (editor) {
                if (wParam) {
                    // OnActivated();
                }
                else {
                    // OnDeactivated();
                }
            }
            // Keyboard::ProcessMessage(message, wParam, lParam);
            // Mouse::ProcessMessage(message, wParam, lParam);
            break;

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMQUERYSUSPEND:
                    if (!s_in_suspend && editor)
                        // OnSuspending();
                        s_in_suspend = TRUE;
                    return TRUE;

                case PBT_APMRESUMESUSPEND:
                    if (!s_minimized) {
                        if (s_in_suspend && editor)
                            // OnResuming();
                            s_in_suspend = FALSE;
                    }
                    return TRUE;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SYSKEYDOWN:
            // Keyboard::ProcessMessage(message, wParam, lParam);
            if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
                // Implements the classic ALT+ENTER fullscreen toggle
                if (s_fullscreen) {
                    SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                    SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                    uint32_t width = 800;
                    uint32_t height = 600;
                    if (editor) {
                        editor->GetDefaultSize(&width, &height);
                    }
                    ShowWindow(hWnd, SW_SHOWNORMAL);

                    SetWindowPos(hWnd,
                                 HWND_TOP,
                                 0,
                                 0,
                                 width,
                                 height,
                                 SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
                else {
                    SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);
                    SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                    SetWindowPos(hWnd,
                                 HWND_TOP,
                                 0,
                                 0,
                                 0,
                                 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                     SWP_FRAMECHANGED);

                    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
                }

                s_fullscreen = !s_fullscreen;
            }
            break;

        case WM_MENUCHAR:
            // A menu is active and the user presses a key that does not
            // correspond to any mnemonic or accelerator key. Ignore so we don't
            // produce an error beep.
            return MAKELRESULT(0, MNC_CLOSE);

        case WM_MOUSEMOVE:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseMove(message, wParam, lParam);
            }
            break;
        case WM_INPUT:
        case WM_LBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseDown(
                    message, wParam, lParam, Mouse::ButtonType::Left);
            }
            break;
        case WM_LBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseUp(
                    message, wParam, lParam, Mouse::ButtonType::Left);
            }
            break;
        case WM_RBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseDown(
                    message, wParam, lParam, Mouse::ButtonType::Right);
            }
            break;
        case WM_RBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseUp(
                    message, wParam, lParam, Mouse::ButtonType::Right);
            }
            break;
        case WM_MBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseDown(
                    message, wParam, lParam, Mouse::ButtonType::Scroll);
            }
            break;
        case WM_MBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseUp(
                    message, wParam, lParam, Mouse::ButtonType::Scroll);
            }
            break;
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_MOUSEHOVER:
            break;

        case WM_KEYDOWN:
        case WM_CHAR:
            if (!ImGui::GetIO().WantCaptureKeyboard) {
                Keyboard::Get().OnKeyDown(wParam);
                if (wParam == VK_ESCAPE) {
                    PostQuitMessage(0);
                }
            }
            break;
        case WM_KEYUP:
            if (!ImGui::GetIO().WantCaptureKeyboard) {
                Keyboard::Get().OnKeyUp(wParam);
            }
            break;
        case WM_SYSKEYUP:
        {
            // Keyboard::ProcessMessage(message, wParam, lParam);
        } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK
About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;
}