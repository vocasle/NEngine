#include "NEngine/ECS/Systems/MoveSystem.h"

#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Systems {

using namespace NEngine::ECS::Components;
using namespace NEngine::Math;

auto
MoveSystem::Update(float dt) -> void
{
    for (auto entity : mEntities) {
        auto &pc = *mEntityManager->GetComponent<PositionComponent>(entity);
        pc.Position.x += pc.Velocity.x;
        pc.Position.y += pc.Velocity.y;
        pc.Position.z += pc.Velocity.z;

        if (mEntityManager->HasComponent<CameraComponent>(entity)) {
            UpdateCamera(dt, entity, pc);
        }
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
        if (it != std::end(mEntities)) {
            mEntities.erase(it);
        }
    }
}

auto
MoveSystem::UpdateCamera(float dt, Entity entity, const PositionComponent &pc)
    -> void
{
    auto &camComp = *mEntityManager->GetComponent<CameraComponent>(entity);
    auto entityPos = Vec3D(pc.Position.x, pc.Position.y, pc.Position.z);
    auto at = Vec3D(0, 0, 10);
    const auto v = Vec3D(pc.Velocity.x, pc.Velocity.y, pc.Velocity.z);

    camComp.Camera.LookAt(Vec3D(entityPos.X, entityPos.Y + 2, entityPos.Z - 10),
                          entityPos,
                          Vec3D(0, 1, 0));
}

}  // namespace NEngine::ECS::Systems