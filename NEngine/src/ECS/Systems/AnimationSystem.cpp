#include "NEngine/ECS/Systems/AnimationSystem.h"

namespace NEngine::ECS::Systems {

AnimationSystem::AnimationSystem(ECS::DefaultEntityManager &entity_manager)
    : m_entity_manager(&entity_manager)
{
}

auto
AnimationSystem::Update(float dt) -> void
{
}

auto
AnimationSystem::RegisterEntity(Entity entity) -> void
{
    if (m_entity_manager->GetComponent<Components::AnimationComponent>(entity) &&
        std::ranges::find(m_entities, entity) == std::end(m_entities)) {
        m_entities.push_back(entity);
    }
}

auto
AnimationSystem::UnregisterEntity(Entity entity) -> void
{
    const auto found = std::ranges::find(m_entities, entity);
    if (found != std::end(m_entities)) {
        m_entities.erase(found);
    }
}
}  // namespace NEngine::ECS::Systems
