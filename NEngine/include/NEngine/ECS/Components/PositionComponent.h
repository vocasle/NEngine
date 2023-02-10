#pragma once

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
};

}  // namespace NEngine::ECS::Components
