#pragma once

#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/EntityManager.h"

namespace NEngine::ECS::Systems {

class CollisionSystem
{
public:
    explicit CollisionSystem(ECS::DefaultEntityManager &em);
    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;

private:
    bool IsOverlaping(const Math::vec3 &point,
                      const Components::CollisionComponent &box) const;

    bool IsOverlaping(const Components::CollisionComponent &lhs,
                      const Components::CollisionComponent &rhs) const;

    Components::CollisionComponent MoveBox(
        const Math::vec3 &offset, const Components::CollisionComponent &box) const;

    ECS::DefaultEntityManager *mEntityManager;
    std::vector<Entity> mEntities;
};

}  // namespace NEngine::ECS::Systems
