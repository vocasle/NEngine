#pragma once

#include "NEngine/ECS/Entity.h"

namespace NEngine::ECS {

class System
{
public:
    virtual ~System() = default;
    virtual auto Update(float dt) -> void = 0;
    // This way we can add components to the system so that system starts to
    // process components of this entity
    virtual auto RegisterEntity(Entity entity) -> void = 0;
    // This way we can remove components from the system so that system stops to
    // process components of this entity
    virtual auto UnregisterEntity(Entity entity) -> void = 0;
};
}  // namespace NEngine::ECS