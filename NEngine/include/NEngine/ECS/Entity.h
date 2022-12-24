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
    GameObject(Entity id, const std::string &name, unsigned long componentMask)
        : Name(name),
          ID(id),
          ComponentMask(componentMask)
    {
    }

    GameObject(Entity id, const std::string &name)
        : GameObject(id, name, 0)
    {
    }
    GameObject()
        : GameObject(0, "")
    {
    }

    std::string Name;
    Entity ID;
    unsigned long ComponentMask;
};
}  // namespace NEngine::ECS
