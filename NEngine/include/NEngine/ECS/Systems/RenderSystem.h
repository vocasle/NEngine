#pragma once

#include <memory>

#include "NEngine/ECS/EntityManager.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Renderer/BasePass.h"

namespace NEngine::ECS::Systems {

class RenderSystem
{
public:
    RenderSystem(NEngine::Helpers::DeviceResources &deviceResources,
                 ECS::DefaultEntityManager &entityManager,
                 NEngine::Helpers::Camera &camera);

    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;

    auto ReloadShaders() -> void;

private:
    auto Clear() -> void;

    NEngine::Helpers::DeviceResources *mDeviceResources;
    std::unique_ptr<NEngine::Renderer::BasePass> mBasePass;
    ECS::DefaultEntityManager *mEntityManager;
    std::vector<ECS::Entity> mEntities;
    NEngine::Helpers::Camera *mCamera;
};

}  // namespace NEngine::ECS::Systems