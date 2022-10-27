#pragma once

#include <memory>

#include "InputLayout.h"
#include "Model.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/Renderer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "NEngine/Helpers/DynamicConstBuffer.h"

namespace NEngine::Renderer {
class BasePass
{
public:
    explicit BasePass(Helpers::DeviceResources &deviceResources);
    virtual void Draw(Helpers::DeviceResources &deviceResources,
                      std::vector<Model *> &models);

    virtual ~BasePass() = default;

protected:
    std::unique_ptr<VertexShader> mVertexShader;
    std::unique_ptr<PixelShader> mPixelShader;
    std::unique_ptr<InputLayout> mInputLayout;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerFrameBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerSceneBuffer;
    std::unique_ptr<Helpers::DynamicConstBuffer> mPerObjectBuffer;

    void DrawMesh(const Renderer::Mesh *mesh, Helpers::DeviceResources &deviceResources);
};
}  // namespace NEngine::Renderer