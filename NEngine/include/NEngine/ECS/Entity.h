#pragma once

#include <string>

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

struct GameObject
{
    std::string Name;
    Entity ID;
    long ComponentMask;
};
}  // namespace NEngine::ECS
