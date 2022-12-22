#pragma once

#include <memory>

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Renderer/BasePass.h"
#include "NEngine/ECS/System.h"

namespace NEngine::ECS::Systems {

class RenderSystem : public NEngine::ECS::System
{
public:
    explicit RenderSystem(NEngine::Helpers::DeviceResources &deviceResources);

    virtual auto Update(float dt) -> void override;
    virtual auto RegisterEntity(Entity entity) -> void override;
    virtual auto UnregisterEntity(Entity entity) -> void override;

private:
    auto Clear() -> void;

    NEngine::Helpers::DeviceResources *mDeviceResources;
    std::unique_ptr<NEngine::Renderer::BasePass> mBasePass;
};

}  // namespace NEngine::ECS::Systems