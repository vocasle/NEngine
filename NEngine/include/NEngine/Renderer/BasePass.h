#pragma once

#include <memory>

#include "InputLayout.h"
#include "Mesh.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Helpers/Renderer.h"
#include "NEngine/Renderer/RasterizerState.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "RasterizerState.h"


namespace NEngine::Renderer {
class BasePass
{
public:
    explicit BasePass(Helpers::DeviceResources &deviceResources);
    virtual void Draw(
        Helpers::DeviceResources &deviceResources,
        std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &meshes);

    virtual ~BasePass() = default;
    void SetCamera(const Helpers::Camera &camera);

protected:
    std::unique_ptr<VertexShader> mVertexShader;
    std::unique_ptr<PixelShader> mPixelShader;
    std::unique_ptr<InputLayout> mInputLayout;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerFrameBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerSceneBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerObjectBuffer;
    const Helpers::Camera *mCamera;
    std::unique_ptr<RasterizerState> mRasterizerState;

    void DrawMeshPrimitive(const Renderer::MeshPrimitive *meshPrimitive,
                           Helpers::DeviceResources &deviceResources);

    void UpdatePerFrameBuffer();
};
}  // namespace NEngine::Renderer