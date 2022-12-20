#pragma once

namespace NEngine::ECS {
typedef long Entity;

template <typename T>
struct ComponentData
{
    ComponentData()
        : ComponentData(0)
    {
    }
    ComponentData(Entity entity)
        : Entity(entity),
          Component()
    {
    }
    Entity Entity;
    T Component;
};
}  // namespace NEngine::ECS
