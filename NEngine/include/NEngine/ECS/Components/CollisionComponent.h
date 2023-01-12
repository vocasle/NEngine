#pragma once

#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct CollisionComponent
{
    NEngine::Math::Vec3D Origin;
    float Size;
};

}  // namespace NEngine::ECS::Components
