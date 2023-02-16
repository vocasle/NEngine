#pragma once

#include "NEngine/Helpers/Transform.h"
#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct PositionComponent
{
    Math::Vec3D Velocity;
    Math::Vec3D Position;
    float Yaw = 0;
    float Pitch = 0;
    bool Movable = false;
    float Scale = 1;
    bool IsTransformSet = false;

    NEngine::Helpers::Transform Transform;
};

}  // namespace NEngine::ECS::Components
