#pragma once

#include <stdint.h>

#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/NEMath.h"

namespace NEngine {
namespace Helpers {
#define CAMERA_SENSITIVITY 0.01f
#define MOUSE_SENSITIVITY 0.0001f

struct Camera
{
    void Follow(const Math::vec3 &target_pos);
    Math::Mat4X4 GetViewMat() const;
    Math::Mat4X4 GetProjMat() const;
    std::string ToString() const;

    Math::vec3 pos;
    Math::vec3 focus_pos;

    float fov;
    float aspect_ratio;
    float z_near;
    float z_far;
};
}  // namespace Helpers
}  // namespace NEngine
