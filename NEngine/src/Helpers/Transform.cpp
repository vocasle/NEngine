#include "NEngine/Helpers/Transform.h"

#include "NEngine/Math/NEMath.h"
#include "NEngine/Math/MathUtils.h"

using namespace NEngine::Math;

namespace NEngine::Helpers {

[[nodiscard]] Math::Mat4X4
Transform::get_transform() const
{
    if (use_matrix) {
        return transform;
    }
    if (updated) {
        return Mat4X4::Scale(anim_scale) * QuatToMat(anim_rotation) *
            Mat4X4::Translate(anim_translation);
    }
    return Mat4X4::Scale(scale) * QuatToMat(rotation) *
           Mat4X4::Translate(translation);
}

}  // namespace NEngine::Helpers