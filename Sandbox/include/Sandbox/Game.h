#pragma once

#define _CRTDBG_MAP_ALLOC
#include <wrl/client.h>

#include <memory>
#include <utility>
#include <vector>

#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Utils/Timer.h"
#include "NEngine/Renderer/BasePass.h"

class Game {
public:
    Game();
    ~Game();

    void Tick();
    void Initialize(HWND hWnd, uint32_t width, uint32_t height);
    void GetDefaultSize(uint32_t *width, uint32_t *height);
    void OnWindowSizeChanged(int width, int height);

private:
    void Clear();
    void Update();
    void Render();
    void CreateWindowSizeDependentResources();

#if WITH_IMGUI
    void UpdateImgui();
#endif

    std::unique_ptr<NEngine::Helpers::DeviceResources> m_deviceResources;
    NEngine::Utils::Timer m_timer;
    NEngine::Helpers::Camera m_camera;

    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> m_meshes;
    std::unique_ptr<NEngine::Renderer::BasePass> m_basePass;
};
