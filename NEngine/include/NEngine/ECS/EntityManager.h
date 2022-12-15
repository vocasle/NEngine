#pragma once

#include <bitset>
#include <unordered_map>

#include "ECS.h"

namespace NEngine::ECS {
class EntityManager
{
public:
    auto CreateEntity() -> Entity;
    template <typename Component>
    auto AddComponent(Entity entity, ComponentType componentType)
        -> Component &;
    template <typename Component>
    auto GetComponent(Entity entity) const -> Component *;

    template <typename Component>
    auto RemoveComponent(Entity entity) -> void;

    auto DestroyEntity(Entity entity) -> void;

    auto GetComponentMask(Entity entity) -> std::bitset<64>;

    auto HasCompoment(Entity entity, ComponentType componentType) const -> bool;

private:
    std::unordered_map<Entity, std::bitset<64>> mEntityCompMap;
};
}  // namespace NEngine::ECS