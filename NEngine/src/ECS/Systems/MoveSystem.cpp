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
        if (pc.Movable) {
            pc.Transform.translation = pc.Transform.translation + pc.Velocity;
            if (mEntityManager->HasComponent<CameraComponent>(entity)) {
                UpdateCamera(dt, entity, pc);
            }
        }
        else if (!pc.IsTransformSet) {
            pc.IsTransformSet = true;
        }
    }
}

auto
MoveSystem::RegisterEntity(Entity entity) -> void
{
    if (mEntityManager->HasComponent<PositionComponent>(entity) &&
        std::ranges::find(mEntities, entity) == std::end(mEntities)) {
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
    return;
    auto &camComp = *mEntityManager->GetComponent<CameraComponent>(entity);

    // camComp.Camera.m_Yaw = ToRadians(pc.Yaw);
    // camComp.Camera.m_Pitch = ToRadians(45.0f);
    //  camComp.Camera.Arcball(dt);
    camComp.Camera.Follow(pc.Transform.translation);
    UTILS_PRINTLN("pc: %s, cam: %s",
                  pc.Transform.translation.ToString().c_str(),
                  camComp.Camera.ToString().c_str());
}

}  // namespace NEngine::ECS::Systems