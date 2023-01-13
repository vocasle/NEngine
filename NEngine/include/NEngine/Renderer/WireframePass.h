#pragma once

#include "BasePass.h"

namespace NEngine::Renderer {

class WireframePass : public BasePass
{
public:
    explicit WireframePass(Helpers::DeviceResources &deviceResources);
    virtual void Draw(
        Helpers::DeviceResources &deviceResources,
        std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &meshes);
    virtual ~WireframePass() = default;
};

}  // namespace NEngine::Renderer
