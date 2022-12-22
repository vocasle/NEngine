#pragma once

#include <memory>

#include "NEngine/ECS/EntityManager.h"
#include "NEngine/ECS/System.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Renderer/BasePass.h"

namespace NEngine::ECS::Systems {

class RenderSystem : public NEngine::ECS::System
{
public:
    RenderSystem(NEngine::Helpers::DeviceResources &deviceResources,
                 ECS::DefaultEntityManager &entityManager);

    virtual auto Update(float dt) -> void override;
    virtual auto RegisterEntity(Entity entity) -> void override;
    virtual auto UnregisterEntity(Entity entity) -> void override;

private:
    auto Clear() -> void;

    NEngine::Helpers::DeviceResources *mDeviceResources;
    std::unique_ptr<NEngine::Renderer::BasePass> mBasePass;
    ECS::DefaultEntityManager *mEntityManager;
    std::vector<ECS::Entity> mEntities;
};

}  // namespace NEngine::ECS::Systems