#pragma once

#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct CollisionComponent
{
    NEngine::Math::Vec3D BoxMin;
    NEngine::Math::Vec3D BoxMax;
};

}  // namespace NEngine::ECS::Components
