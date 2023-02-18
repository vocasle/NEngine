#pragma once
#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/EntityManager.h"

namespace NEngine::ECS::Systems {
class AnimationSystem
{
public:
    explicit AnimationSystem(ECS::DefaultEntityManager &entity_manager);

    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;

private:
    ECS::DefaultEntityManager *m_entity_manager;
    std::vector<ECS::Entity> m_entities;
};
}  // namespace NEngine::ECS::Systems
