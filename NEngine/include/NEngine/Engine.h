#pragma once

#include <Windows.h>

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Game.h"
#include "NEngine/Utils/Timer.h"

namespace NEngine {

class Engine
{
public:
    Engine(int argc, const char *argv[]);

    auto OnWindowSizeChanged(long width, long height) -> void;
    auto Update() -> void;

    auto PlayGame(NEngine::Game &game) -> void;

private:
    auto CreateDefaultWindow() -> void;
    auto EngineLoop() -> void;
    auto InitImGui() -> void;
    auto InitDeviceResources() -> void;
    auto InitTimer() -> void;

    constexpr static auto ENGINE_WINDOW_CLASS = L"NEngineWindowClass";

    HWND mWnd = nullptr;
    NEngine::Helpers::DeviceResources mDeviceResources;
    NEngine::Game *mGame = nullptr;
    NEngine::Utils::Timer mTimer;
};
}  // namespace NEngine