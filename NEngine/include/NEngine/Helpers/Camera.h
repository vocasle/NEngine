#pragma once

#include <stdint.h>

#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/NEMath.h"
#include "Transform.h"

namespace NEngine {
namespace Helpers {
#define CAMERA_SENSITIVITY 0.01f
#define MOUSE_SENSITIVITY 0.0001f

struct Camera
{
    void Follow(const Transform &transform);
    Math::Mat4X4 GetViewMat() const;
    Math::Mat4X4 GetProjMat() const;

    Math::vec3 pos;
    Transform target_transform;
    float fov;
    float aspect_ratio;
    float z_near;
    float z_far;

    float target_offset_y = 10;
    float target_offset_z = 10;
    bool arcball_mode = false;
};
}  // namespace Helpers
}  // namespace NEngine
