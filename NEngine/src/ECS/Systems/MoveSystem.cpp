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

            // for (auto &node : rc.Model.nodes) {
            //// TODO: world matrix should be precomputed somewhere else.
            // auto translate = Mat4X4::Translate(pc.Position);
            // auto axis = vec3(0, 1, 0);
            // auto rotate = RotateAxis(ToRadians(pc.Yaw), axis);
            // auto scales = vec3(pc.Scale, pc.Scale, pc.Scale);
            // auto scale = Mat4X4::Scale(scales);
            // node.mesh.GetTransform().SetScale(scale);
            // node.mesh.GetTransform().SetRotation(rotate);
            // node.mesh.GetTransform().SetTranslation(translate);

            // mBasePass->Draw(*mDeviceResources, node.mesh);
            //}

            // for (auto &mesh : rc.Mesh) {
            //     // TODO: world matrix should be precomputed somewhere else.
            //     auto translate = Mat4X4::Translate(pc.Position);
            //     auto axis = vec3(0, 1, 0);
            //     auto rotate = RotateAxis(ToRadians(pc.Yaw), axis);
            //     auto scales = vec3(pc.Scale, pc.Scale, pc.Scale);
            //     auto scale = Mat4X4::Scale(scales);
            //     mesh.GetTransform().SetScale(scale);
            //     mesh.GetTransform().SetRotation(rotate);
            //     mesh.GetTransform().SetTranslation(translate);

            //    for (auto &anim : mesh.GetAnimations()) {
            //        anim.Advance(dt, mesh.GetTransform());
            //    }
            //}
        }
        else if (!pc.IsTransformSet) {
            pc.Transform.SetTranslation(Mat4X4::Translate(pc.Position));
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