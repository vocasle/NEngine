#pragma once

#include "NEngine/Math/NEMath.h"

namespace NEngine::Helpers {
struct Transform
{
    [[nodiscard]] Math::Mat4X4 get_transform() const;
    Math::mat4 transform;
    Math::vec4 rotation = Math::vec4(0, 0, 0, 1);
    Math::vec3 translation;
    Math::vec3 scale = Math::vec3(1);
    bool use_matrix = true;
};
}  // namespace NEngine::Helpers