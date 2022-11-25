#pragma once

#include <memory>

#include "InputLayout.h"
#include "Mesh.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Helpers/Renderer.h"
#include "NEngine/Renderer/RasterizerState.h"
#include "PixelShader.h"
#include "RasterizerState.h"
#include "VertexShader.h"

namespace NEngine::Renderer {

enum class BufferType {
    PerFrame,
    PerScene,
    PerObject
};

class BasePass
{
public:
    explicit BasePass(Helpers::DeviceResources &deviceResources);
    virtual void Draw(
        Helpers::DeviceResources &deviceResources,
        std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &meshes);

    virtual ~BasePass() = default;
    void SetCamera(const Helpers::Camera &camera);
    void SetRenderTarget(Texture &renderTarget);
    void SetDepthTarget(Texture &depthTarget);
    void SetViewport(D3D11_VIEWPORT &viewport);

    template <typename T>
    T *
    GetBufferValue(const std::string &name, BufferType bufferType)
    {
        if (bufferType == BufferType::PerFrame)
            return mPerFrameBuffer->GetValue<T>(name);
        if (bufferType == BufferType::PerObject)
            return mPerObjectBuffer->GetValue<T>(name);
        if (bufferType == BufferType::PerScene)
            return mPerSceneBuffer->GetValue<T>(name);
        return nullptr;
    }

    void ReloadShaders();

protected:
    std::unique_ptr<VertexShader> mVertexShader;
    std::unique_ptr<PixelShader> mPixelShader;
    std::unique_ptr<InputLayout> mInputLayout;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerFrameBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerSceneBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerObjectBuffer;
    const Helpers::Camera *mCamera = nullptr;
    std::unique_ptr<RasterizerState> mRasterizerState;
    Helpers::DeviceResources *mDeviceResources;
    Texture *mRenderTarget = nullptr;
    Texture *mDepthTarget = nullptr;
    D3D11_VIEWPORT *mViewport = nullptr;

    void DrawMeshPrimitive(const Renderer::MeshPrimitive *meshPrimitive,
                           Helpers::DeviceResources &deviceResources);

    void UpdatePerFrameBuffer();
};
}  // namespace NEngine::Renderer