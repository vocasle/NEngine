#include "NEngine/Helpers/Transform.h"

#include "NEngine/Math/Math.h"

using namespace NEngine::Math;

namespace NEngine::Helpers {
const Math::Mat4X4 &
Transform::GetTranslate() const
{
    return mTranslate;
}
const Math::Mat4X4 &
Transform::GetRotate() const
{
    return mRotate;
}
const Math::Mat4X4 &
Transform::GetScale() const
{
    return mScale;
}
const Math::Mat4X4
Transform::GetTransform() const
{
    return mTranslate * mRotate * mScale;
}

void
Transform::SetTranslate(const Math::Mat4X4 &translate)
{
    mTranslate = translate;
}
void
Transform::SetRotate(const Math::Mat4X4 &rotate)
{
    mRotate = rotate;
}
void
Transform::SetScale(const Math::Mat4X4 &scale)
{
    mScale = scale;
}

void
Transform::Translate(float x, float y, float z)
{
    Translate({x, y, z});
}
void
Transform::Translate(const Math::Vec3D &offset)
{
    mTranslate = MathMat4X4TranslateFromVec3D(&offset);
}

void
Transform::Rotate(float pitch, float roll, float yaw)
{
    Rotate({pitch, roll, yaw});
}

void
Transform::Rotate(const Math::Vec3D &angles)
{
    mRotate = MathMat4X4RotateFromVec3D(&angles);
}

void
Transform::Scale(float factor)
{
    const Vec3D scale(factor, factor, factor);
    mScale = MathMat4X4ScaleFromVec3D(&scale);
}
}  // namespace NEngine::Helpers