#pragma once

#include <Windows.h>

#include "NEngine/Game.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Renderer/Mesh.h"
#include "NEngine/Utils/Timer.h"

namespace NEngine {

struct WindowSettings
{
    Math::Vec2D Position;
    Math::Vec2D Size;
};

class Engine
{
public:
    static Engine *GEngine;


    Engine(int argc, const char *argv[]);
    ~Engine();

    auto OnWindowSizeChanged(float width, float height) -> void;
    auto OnWindowMoved(float x, float y) -> void;

    auto Update() -> void;

    auto PlayGame(NEngine::Game &game) -> void;
    auto GetWindowSize() const -> Math::Vec2D;

    // TODO: Move to private
    auto GetDeviceResources() -> NEngine::Helpers::DeviceResources &;

    auto LoadMesh(const std::string &path)
        -> std::vector<std::unique_ptr<NEngine::Renderer::Mesh>>;
    auto Deinit() -> void;

private:
    auto CreateDefaultWindow() -> void;
    auto EngineLoop() -> void;
    auto InitImGui() -> void;
    auto InitDeviceResources() -> void;
    auto InitTimer() -> void;
    auto ReadConfig() -> void;
    auto SaveConfig() -> void;

    constexpr static auto ENGINE_WINDOW_CLASS = L"NEngineWindowClass";

    HWND mWnd = nullptr;
    NEngine::Helpers::DeviceResources mDeviceResources;
    NEngine::Game *mGame = nullptr;
    NEngine::Utils::Timer mTimer;
    WindowSettings mWinSettings;
};

}  // namespace NEngine