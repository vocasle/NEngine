#pragma once

#include <vector>

#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/EntityManager.h"

namespace NEngine::ECS::Systems {

class InputSystem
{
public:
    explicit InputSystem(DefaultEntityManager &entityManager);
    auto Update(float dt) -> void;

    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;

private:
    std::vector<Entity> mEntities;
    DefaultEntityManager *mEntityManager;
};
}  // namespace NEngine::ECS::Systems