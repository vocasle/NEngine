#pragma once

#include "NEngine/ECS/Entity.h"

namespace NEngine::ECS::Systems {

class CollisionSystem
{
public:
    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;
};

}  // namespace NEngine::ECS::Systems
