#include "NEngine/Helpers/Camera.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"

using namespace NEngine::Math;

namespace NEngine::Helpers {

static const Math::vec3 UP = Math::vec3(0, 1, 0);

void
Camera::Follow(const Math::vec3 &target_pos)
{
    pos = target_pos - vec3(0, 2, 5);
    focus_pos = target_pos;
}

Math::Mat4X4
Camera::GetViewMat() const
{
    return LookAt(vec3(0, 10, -10), vec3(0, 0, 1), UP);
}

Math::Mat4X4
Camera::GetProjMat() const
{
    return PerspectiveFov(ToRadians(fov), aspect_ratio, z_near, z_far);
}

std::string
Camera::ToString() const
{
    std::ostringstream out;
    out << "pos: " << pos.ToString() << "\nfocus_pos: " << focus_pos.ToString()
        << "\nfov: " << fov << "\naspect_ratio: " << aspect_ratio
        << "\nz_near: " << z_near << "\nz_far: " << z_far;
    return out.str();
}
}  // namespace NEngine::Helpers
