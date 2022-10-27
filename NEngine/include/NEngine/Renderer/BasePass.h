#pragma once

#include <memory>

#include "InputLayout.h"
#include "Mesh.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Helpers/Renderer.h"
#include "PixelShader.h"
#include "VertexShader.h"

namespace NEngine::Renderer {
class BasePass
{
public:
    explicit BasePass(Helpers::DeviceResources &deviceResources);
    virtual void Draw(
        Helpers::DeviceResources &deviceResources,
        std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &meshes);

    virtual ~BasePass() = default;

protected:
    std::unique_ptr<VertexShader> mVertexShader;
    std::unique_ptr<PixelShader> mPixelShader;
    std::unique_ptr<InputLayout> mInputLayout;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerFrameBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerSceneBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerObjectBuffer;

    void DrawMeshPrimitive(const Renderer::MeshPrimitive *meshPrimitive,
                           Helpers::DeviceResources &deviceResources);
};
}  // namespace NEngine::Renderer