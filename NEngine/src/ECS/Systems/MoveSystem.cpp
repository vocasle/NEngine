#include "NEngine/ECS/Systems/MoveSystem.h"

namespace NEngine::ECS::Systems {

using namespace NEngine::ECS::Components;

auto
MoveSystem::Update(float dt) -> void
{
    for (auto entity : mEntities) {
        auto &pc = *mEntityManager->GetComponent<PositionComponent>(entity);
        pc.Position.x += pc.Velocity.x;
        pc.Position.y += pc.Velocity.y;
        pc.Position.z += pc.Velocity.z;
    }
}

auto
MoveSystem::RegisterEntity(Entity entity) -> void
{
    if (mEntityManager->HasComponent<PositionComponent>(entity)) {
        mEntities.push_back(entity);
    }
}

auto
MoveSystem::UnregisterEntity(Entity entity) -> void
{
    if (mEntityManager->HasComponent<PositionComponent>(entity)) {
        auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
        mEntities.erase(it);
    }
}

}  // namespace NEngine::ECS::Systems