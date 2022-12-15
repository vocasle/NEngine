#include "NEngine/ECS/EntityManager.h"

namespace NEngine::ECS {

auto
ComponentTypeToBitmask(ComponentType componentType) -> std::bitset<64>
{
    auto bitmask = std::bitset<64>();
    bitmask[static_cast<uint8_t>(componentType)] = true;
    return bitmask;
}

auto
EntityManager::HasCompoment(Entity entity, ComponentType componentType) const
    -> bool
{
    auto bitmask = ComponentTypeToBitmask(componentType);
    return (bitmask.to_ulong() & mEntityCompMap.at(entity).to_ulong()) ==
           bitmask.to_ulong();
}

auto
EntityManager::GetComponentMask(Entity entity) -> std::bitset<64>
{
    return mEntityCompMap.at(entity);
}

}  // namespace NEngine::ECS