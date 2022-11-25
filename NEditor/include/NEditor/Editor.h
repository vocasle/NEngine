#pragma once

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "DirectXMath.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Renderer/BasePass.h"
#include "NEngine/Renderer/Mesh.h"
#include "NEngine/Renderer/Texture.h"

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

    NEngine::Helpers::DeviceResources mDeviceResources;
    HWND mWindow = nullptr;
    DirectX::XMFLOAT2 mWinSize = {1280, 720};

    D3D11_VIEWPORT mViewport;
    std::unique_ptr<NEngine::Renderer::Texture> mRenderTarget;
    std::unique_ptr<NEngine::Renderer::Texture> mDepthTarget;
    std::unique_ptr<NEngine::Renderer::BasePass> mBasePass;
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> mMeshes;
    NEngine::Helpers::Camera mCamera = {{0, 0, 5}};
};
}  // namespace NEditor