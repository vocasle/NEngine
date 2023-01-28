#include "NEngine/Helpers/Transform.h"

#include "NEngine/Math/NEMath.h"

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
    return mScale * mRotation * mTranslation;
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
    mTranslation = Mat4X4::Translate(offset);
}

void
Transform::Rotate(float pitch, float roll, float yaw)
{
    Rotate({pitch, roll, yaw});
}

void
Transform::Rotate(const Math::Vec3D &angles)
{
    mRotation = Mat4X4::Rotate(angles);
}

void
Transform::Scale(float factor)
{
    const Vec3D scale(factor, factor, factor);
    mScale = Mat4X4::Scale(scale);
}
void
Transform::SetWorld(const Math::Mat4X4 &world)
{
    mWorld = world;
}
Math::Mat4X4
Transform::GetWorld() const
{
    return mWorld;
}
}  // namespace NEngine::Helpers