#pragma once

#include "NEngine/Helpers/Transform.h"
#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct PositionComponent
{
    Math::Vec3D Velocity;
    bool Movable = false;
    bool IsTransformSet = false;
    NEngine::Helpers::Transform Transform;
};

}  // namespace NEngine::ECS::Components
