#include "NEngine/ECS/Systems/MoveSystem.h"

#include "NEngine/Math/MathUtils.h"
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
            pc.Position = pc.Position + pc.Velocity;

            if (mEntityManager->HasComponent<CameraComponent>(entity)) {
                UpdateCamera(dt, entity, pc);
            }

            pc.Transform.SetTranslation(Mat4X4::Translate(pc.Position));
            pc.Transform.SetRotation(
                RotateAxis(ToRadians(pc.Yaw), vec3(0, 1, 0)));
            pc.Transform.SetScale(Mat4X4::Scale(vec3(pc.Scale)));
        }
        else if (!pc.IsTransformSet) {
            pc.Transform.SetTranslation(Mat4X4::Translate(pc.Position));
            pc.Transform.SetRotation(
                RotateAxis(ToRadians(pc.Yaw), vec3(0, 1, 0)));
            pc.Transform.SetScale(Mat4X4::Scale(vec3(pc.Scale)));
            pc.IsTransformSet = true;
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

    camComp.Camera.m_At = pc.Position;
    camComp.Camera.m_Yaw = ToRadians(pc.Yaw);
    camComp.Camera.m_Pitch = ToRadians(45.0f);
    camComp.Camera.Arcball(dt);
}

}  // namespace NEngine::ECS::Systems