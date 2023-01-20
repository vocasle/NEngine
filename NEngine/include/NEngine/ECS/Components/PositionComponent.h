#pragma once

#include "NEngine/Math/Mat4X4.h"
#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct PositionComponent
{
    Math::Vec3D Velocity;
    Math::Vec3D Position;
    float Yaw;
    float Pitch;
    bool Movable;
    float Scale = 1;
};

}  // namespace NEngine::ECS::Components
