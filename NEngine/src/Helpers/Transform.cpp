#include "NEngine/Helpers/Transform.h"

#include "NEngine/Math/Math.h"

using namespace NEngine::Math;

namespace NEngine::Helpers {
const Math::Mat4X4 &
Transform::GetTranslation() const
{
    return mTranslation;
}
const Math::Mat4X4 &
Transform::GetRotation() const
{
    return mRotation;
}
const Math::Mat4X4 &
Transform::GetScale() const
{
    return mScale;
}
const Math::Mat4X4
Transform::GetTransform() const
{
    return mTranslation * mRotation * mScale;
}

Transform::Transform()
    : mTranslation(MathMat4X4Identity()),
      mRotation(MathMat4X4Identity()),
      mScale(MathMat4X4Identity())
{
}

void
Transform::SetTranslation(const Math::Mat4X4 &translation)
{
    mTranslation = translation;
}
void
Transform::SetRotation(const Math::Mat4X4 &rotation)
{
    mRotation = rotation;
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
    mTranslation = MathMat4X4TranslateFromVec3D(&offset);
}

void
Transform::Rotate(float pitch, float roll, float yaw)
{
    Rotate({pitch, roll, yaw});
}

void
Transform::Rotate(const Math::Vec3D &angles)
{
    mRotation = MathMat4X4RotateFromVec3D(&angles);
}

void
Transform::Scale(float factor)
{
    const Vec3D scale(factor, factor, factor);
    mScale = MathMat4X4ScaleFromVec3D(&scale);
}
}  // namespace NEngine::Helpers