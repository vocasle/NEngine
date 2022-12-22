#pragma once

#include <memory>

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Renderer/BasePass.h"

namespace NEngine::ECS::Systems {

class RenderSystem
{
public:
    explicit RenderSystem(NEngine::Helpers::DeviceResources &deviceResources);

    auto Update(float dt) -> void;

private:
    NEngine::Helpers::DeviceResources *mDeviceResources;
    std::unique_ptr<NEngine::Renderer::BasePass> mBasePass;
};

}  // namespace NEngine::ECS::Systems