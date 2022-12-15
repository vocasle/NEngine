#pragma once

#include <Windows.h>

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Game.h"
#include "NEngine/Utils/Timer.h"
#include "NEngine/Math/Math.h"

namespace NEngine {

class Engine
{
public:
    Engine(int argc, const char *argv[]);

    auto OnWindowSizeChanged(long width, long height) -> void;
    auto Update() -> void;

    auto PlayGame(NEngine::Game &game) -> void;
    auto GetWindowSize() const -> Math::Vec2D;

    // TODO: Move to private
    auto GetDeviceResources() -> NEngine::Helpers::DeviceResources &;

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