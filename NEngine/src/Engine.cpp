#include "NEngine/Engine.h"

#include <sstream>

#include "NEngine/Helpers/ModelImporter.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Utils/Utils.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#if NENGINE_TEST_MATH
#include "NEngine/Math/MathTest.h"
#endif

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

extern "C"
{
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace NEngine;
using namespace NEngine::Utils;
using namespace NEngine::Input;
using namespace NEngine::Helpers;

static auto CALLBACK
WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
    static BOOL s_in_sizemove = FALSE;
    static BOOL s_in_suspend = FALSE;
    static BOOL s_minimized = FALSE;
    static BOOL s_fullscreen = FALSE;

    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
        HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }

    auto engine = reinterpret_cast<NEngine::Engine *>(
        GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_COMMAND:
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        } break;
        case WM_PAINT:
            if (s_in_sizemove && engine) {
                engine->Update();
            }
            else {
                PAINTSTRUCT ps;
                (void)BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
            }
            break;

        case WM_MOVE:
            if (engine) {
                engine->OnWindowMoved(float(LOWORD(lParam)),
                                      float(HIWORD(lParam)));
            }
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED) {
                if (!s_minimized) {
                    s_minimized = TRUE;
                    if (!s_in_suspend && engine)
                        // OnSuspending();
                        s_in_suspend = TRUE;
                }
            }
            else if (s_minimized) {
                s_minimized = FALSE;
                if (s_in_suspend && engine)
                    // OnResuming();
                    s_in_suspend = FALSE;
            }
            else if (!s_in_sizemove && engine) {
                engine->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
            }
            break;

        case WM_ENTERSIZEMOVE:
            s_in_sizemove = TRUE;
            break;

        case WM_EXITSIZEMOVE:
            s_in_sizemove = FALSE;
            if (engine) {
                RECT rc;
                GetClientRect(hWnd, &rc);

                engine->OnWindowSizeChanged(
                    static_cast<float>(rc.right - rc.left),
                    static_cast<float>(rc.bottom - rc.top));
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
            if (engine) {
                if (wParam) {
                    // OnActivated();
                }
                else {
                    // OnDeactivated();
                }
            }
            // Keyboard::ProcessuMsg(uMsg, wParam, lParam);
            // Mouse::ProcessuMsg(uMsg, wParam, lParam);
            break;

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMQUERYSUSPEND:
                    if (!s_in_suspend && engine)
                        // OnSuspending();
                        s_in_suspend = TRUE;
                    return TRUE;

                case PBT_APMRESUMESUSPEND:
                    if (!s_minimized) {
                        if (s_in_suspend && engine)
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
            // Keyboard::ProcessuMsg(uMsg, wParam, lParam);
            if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
                // Implements the classic ALT+ENTER fullscreen toggle
                if (s_fullscreen) {
                    SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                    SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                    ShowWindow(hWnd, SW_SHOWNORMAL);

                    SetWindowPos(hWnd,
                                 HWND_TOP,
                                 0,
                                 0,
                                 DEFAULT_WIN_WIDTH,
                                 DEFAULT_WIN_HEIGHT,
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
            return MAKELRESULT(0, MNC_CLOSE);

        case WM_MOUSEMOVE:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseMove(uMsg, wParam, lParam);
            }
            break;
        case WM_INPUT:
        case WM_LBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseDown(
                    uMsg, wParam, lParam, Mouse::ButtonType::Left);
            }
            break;
        case WM_LBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseUp(
                    uMsg, wParam, lParam, Mouse::ButtonType::Left);
            }
            break;
        case WM_RBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseDown(
                    uMsg, wParam, lParam, Mouse::ButtonType::Right);
            }
            break;
        case WM_RBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseUp(
                    uMsg, wParam, lParam, Mouse::ButtonType::Right);
            }
            break;
        case WM_MBUTTONDOWN:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseDown(
                    uMsg, wParam, lParam, Mouse::ButtonType::Scroll);
            }
            break;
        case WM_MBUTTONUP:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseUp(
                    uMsg, wParam, lParam, Mouse::ButtonType::Scroll);
            }
            break;
        case WM_MOUSEWHEEL:
            if (!ImGui::GetIO().WantCaptureMouse) {
                Mouse::Get().OnMouseWheel(uMsg, wParam, lParam);
            }
            break;
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
            // Keyboard::ProcessuMsg(uMsg, wParam, lParam);
        } break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace NEngine {

Engine *GEngine = nullptr;

Engine::Engine(int argc, const char *argv[])
{
#if NENGINE_TEST_MATH
    Math::MathTest();
#endif
    UtilsDebugPrint("Engine init with args:\n");
    for (int i = 0; i < argc; ++i) {
        UtilsDebugPrint("[%d]: %s\n", i, argv[i]);
    }

    ReadConfig();

    CreateDefaultWindow();
    if (!mWnd) {
        UTILS_FATAL_ERROR("Failed to create default window!");
        return;
    }

    SetWindowLongPtr(mWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    InitDeviceResources();
    InitImGui();
    InitTimer();

    ShowWindow(mWnd, SW_MAXIMIZE);

    GEngine = this;
}

auto
Engine::CreateDefaultWindow() -> void
{
    auto wc = WNDCLASS();
    wc.lpfnWndProc = WinProc;
    wc.lpszClassName = ENGINE_WINDOW_CLASS;
    RegisterClass(&wc);
    auto workAreaRect = RECT();
    if (SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0)) {
        UTILS_PRINTLN("We got some parameters!");
        mWinSettings.Size.X =
            static_cast<float>(workAreaRect.right - workAreaRect.left);
        mWinSettings.Size.Y =
            static_cast<float>(workAreaRect.bottom - workAreaRect.top);
        mWinSettings.Position = Math::Vec2D(0, 0);
    }
    else {
        UTILS_PRINTLN("Failed to get system parameters: %s",
                      UtilsGetLastWin32Error().c_str());
    }

    mWnd = CreateWindowEx(0,
                          NEngine::Engine::ENGINE_WINDOW_CLASS,
                          L"NEngine v0.0.1",
                          WS_OVERLAPPEDWINDOW,
                          static_cast<int>(mWinSettings.Position.X),
                          static_cast<int>(mWinSettings.Position.Y),
                          static_cast<int>(mWinSettings.Size.X),
                          static_cast<int>(mWinSettings.Size.Y),
                          nullptr,
                          nullptr,
                          nullptr,
                          nullptr);
}

auto
Engine::EngineLoop() -> void
{
    auto msg = MSG();
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Update();
        }
    }
}

auto
Engine::OnWindowSizeChanged(float width, float height) -> void
{
    UtilsDebugPrint("Window size changed: %f, %f\n", width, height);
    mDeviceResources.WindowSizeChanged(static_cast<int>(width),
                                       static_cast<int>(height));
    mWinSettings.Size = {width, height};
}

auto
Engine::Update() -> void
{
    mTimer.Update();
    if (mGame) {
        mGame->Update(mTimer.GetDelta());
    }
}

auto
Engine::InitImGui() -> void
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplWin32_Init(mWnd);
    ImGui_ImplDX11_Init(mDeviceResources.GetDevice(),
                        mDeviceResources.GetDeviceContext());
}

auto
Engine::InitDeviceResources() -> void
{
    auto rect = RECT();
    GetWindowRect(mWnd, &rect);
    mDeviceResources.SetWindow(
        mWnd, rect.right - rect.left, rect.bottom - rect.top);
    mDeviceResources.CreateDeviceResources();
    mDeviceResources.CreateWindowSizeDependentResources();
}

auto
Engine::PlayGame(NEngine::Game &game) -> void
{
    mGame = &game;
    game.InitWithEngine(*this);

    EngineLoop();
}

auto
Engine::InitTimer() -> void
{
    mTimer.Initialize();
}

auto
Engine::GetWindowSize() const -> Math::Vec2D
{
    return {static_cast<float>(mDeviceResources.GetBackBufferWidth()),
            static_cast<float>(mDeviceResources.GetBackBufferHeight())};
}

auto
Engine::GetDeviceResources() -> NEngine::Helpers::DeviceResources &
{
    return mDeviceResources;
}

Engine::~Engine()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    GEngine = nullptr;
}

auto
Engine::LoadMesh(const std::string &path)
    -> std::vector<NEngine::Renderer::Mesh>
{
    ModelImporter importer(mDeviceResources);
    auto model = importer.Load(path);
    return model;
}

auto
Engine::OnWindowMoved(float x, float y) -> void
{
    UtilsDebugPrint("Window moved: %f %f\n", x, y);
    mWinSettings.Position = {x, y};
}

auto
Engine::SaveConfig() -> void
{
    std::ostringstream out;

    out << "[window]" << '\n'
        << "x=" << mWinSettings.Position.X << '\n'
        << "y=" << mWinSettings.Position.Y << '\n'
        << "w=" << mWinSettings.Size.X << '\n'
        << "h=" << mWinSettings.Size.Y << '\n';

    const auto windowConfig = out.str();

    const auto filename =
        UtilsFormatStr("%s/%s", NENGINE_BUILD_DIR, "config.ini");
    UtilsWriteData(filename.c_str(), windowConfig.c_str(), windowConfig.size());
}

auto
Engine::ReadConfig() -> void
{
    const auto filename =
        UtilsFormatStr("%s/%s", NENGINE_BUILD_DIR, "config.ini");
    const auto bytes = UtilsReadData(filename.c_str());

    const auto stringData = std::string(std::begin(bytes), std::end(bytes));

    std::istringstream in(stringData);
    std::string line;
    while (std::getline(in, line)) {
        if (line.find("[window]") != std::string::npos) {
            float x = 0;
            float y = 0;
            char ch = 0;
            if (!(in >> ch >> ch >> x)) {
                UtilsDebugPrint("Failed to read window x coordinate\n");
            }

            if (!(in >> ch >> ch >> y)) {
                UtilsDebugPrint("Failed to read window y coordinate\n");
            }

            mWinSettings.Position = {x, y};

            x = DEFAULT_WIN_WIDTH;
            y = DEFAULT_WIN_HEIGHT;

            if (!(in >> ch >> ch >> x)) {
                UtilsDebugPrint("Failed to read window width\n");
            }

            if (!(in >> ch >> ch >> y)) {
                UtilsDebugPrint("Failed to read window height\n");
            }

            mWinSettings.Size = {x, y};
        }
    }
}

auto
Engine::Deinit() -> void
{
    SaveConfig();
}

}  // namespace NEngine