#pragma once

#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/EntityManager.h"
#include "NEngine/ECS/System.h"

namespace NEngine::ECS::Systems {
class MoveSystem : public NEngine::ECS::System
{
public:
    explicit MoveSystem(ECS::DefaultEntityManager &entityManager)
        : mEntityManager(&entityManager)
    {
    }

    virtual auto Update(float dt) -> void override;
    virtual auto RegisterEntity(Entity entity) -> void override;
    virtual auto UnregisterEntity(Entity entity) -> void override;

private:
    ECS::DefaultEntityManager *mEntityManager;
    std::vector<Entity> mEntities;
};
}  // namespace NEngine::ECS::Systems