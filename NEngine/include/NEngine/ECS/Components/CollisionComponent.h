#pragma once

#include <functional>

#include "NEngine/ECS/Entity.h"
#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct CollisionComponent
{
    NEngine::Math::Vec3D BoxMin;
    NEngine::Math::Vec3D BoxMax;
    std::function<void(ECS::Entity e1, ECS::Entity e2)> OnCollision;
};

}  // namespace NEngine::ECS::Components
