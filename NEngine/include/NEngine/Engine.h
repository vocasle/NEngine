#pragma once

#include <Windows.h>

#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine {

class Engine
{
public:
    Engine(int argc, const char *argv[]);

    auto OnWindowSizeChanged(long width, long height) -> void;
    auto Update() -> void;

private:
    auto CreateDefaultWindow() -> void;
    auto EngineLoop() -> void;
    auto InitImGui() -> void;
    auto InitDeviceResources() -> void;

    constexpr static auto ENGINE_WINDOW_CLASS = L"NEngineWindowClass";

    HWND mWnd = nullptr;
    NEngine::Helpers::DeviceResources mDeviceResources;
};
}  // namespace NEngine