#pragma once

#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/EntityManager.h"

namespace NEngine::ECS::Systems {
class MoveSystem
{
public:
    explicit MoveSystem(ECS::DefaultEntityManager &entityManager)
        : mEntityManager(&entityManager)
    {
    }

    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;

private:
    auto UpdateCamera(float dt, Entity entity, const Components::PositionComponent &pc)
        -> void;

    ECS::DefaultEntityManager *mEntityManager;
    std::vector<Entity> mEntities;
};
}  // namespace NEngine::ECS::Systems