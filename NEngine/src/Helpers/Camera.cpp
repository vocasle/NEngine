#include "NEngine/Helpers/Camera.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Math;

namespace NEngine::Helpers {

static const vec3 UP = Math::vec3(0, 1, 0);
static const vec3 ZERO = vec3(0);

void
Camera::Follow(const Transform &transform)
{
    target_transform = transform;
    // Calculate camera pos for lighting
    pos = Mat4X4::Mult(transform.get_transform(),
                       vec4(vec3(0, target_offset_y, -target_offset_z), 1.0))
              .ToVec3D();
}

Math::Mat4X4
Camera::GetViewMat() const
{
    const auto view_mat =
        LookAt(vec3(0, target_offset_y, -target_offset_z), ZERO, UP);
    const auto tmp = target_transform.get_transform().Inverse() * view_mat;
    return tmp;
}

Math::Mat4X4
Camera::GetProjMat() const
{
    return PerspectiveFov(ToRadians(fov), aspect_ratio, z_near, z_far);
}
}  // namespace NEngine::Helpers
