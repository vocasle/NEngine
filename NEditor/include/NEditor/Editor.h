#pragma once

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/BasePass.h"
#include "NEngine/Renderer/Mesh.h"
#include "NEngine/Renderer/Texture.h"
#include "NEngine/Utils/Timer.h"

namespace NEditor {
class Editor
{
public:
    void Tick();
    void Initialize(HWND wnd, uint32_t width, uint32_t height);
    void GetDefaultSize(uint32_t *width, uint32_t *height);
    void OnWindowSizeChanged(int width, int height);
    ~Editor();

private:
    void UpdateImgui();
    void Clear();
    bool OnViewportSizeChanged();
    void UpdateViewportSize(const NEngine::Math::Vec2D &viewportSize);
    void RegisterKeys();
    void ProcessViewportInput();
    void LoadMesh(const std::string &path);

    NEngine::Helpers::DeviceResources mDeviceResources;
    NEngine::Math::Vec2D mWinSize = {1280, 720};

    D3D11_VIEWPORT mViewport;
    std::unique_ptr<NEngine::Renderer::Texture> mRenderTarget;
    std::unique_ptr<NEngine::Renderer::Texture> mDepthTarget;
    std::unique_ptr<NEngine::Renderer::BasePass> mBasePass;
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> mMeshes;
    NEngine::Helpers::Camera mCamera = {{0, 0, 5}};
    NEngine::Utils::Timer mTimer;
    NEngine::Helpers::DirectionalLight mDirLight;
};
}  // namespace NEditor