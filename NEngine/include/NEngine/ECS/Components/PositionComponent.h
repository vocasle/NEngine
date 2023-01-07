#pragma once

#include "NEngine/Math/Mat4X4.h"
#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct PositionComponent
{
    Math::Mat4X4 World;
    Math::Vec3D Velocity;
    Math::Vec3D Position;
    Math::Vec3D Direction;
    float Yaw;
    float Pitch;
};

}  // namespace NEngine::ECS::Components
