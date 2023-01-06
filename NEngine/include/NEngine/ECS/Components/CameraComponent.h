#pragma once

#include "NEngine/Helpers/Camera.h"
#include "NEngine/Math/Vec3D.h"

namespace NEngine::ECS::Components {

struct CameraComponent
{
    static constexpr const auto DEFAULT_OFFSET = 10.0f;
    Helpers::Camera Camera;
    Math::Vec3D Target;
    float Offset = DEFAULT_OFFSET;
};

}  // namespace NEngine::ECS::Components
