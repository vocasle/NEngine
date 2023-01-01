#include "NEngine/Helpers/Transform.h"

#include "glm/ext.hpp"
#include "glm/gtx/quaternion.hpp"

namespace NEngine::Helpers {
const glm::mat4x4 &
Transform::GetTranslation() const
{
    return mTranslation;
}
const glm::mat4x4 &
Transform::GetRotation() const
{
    return mRotation;
}
const glm::mat4x4 &
Transform::GetScale() const
{
    return mScale;
}
const glm::mat4x4
Transform::GetTransform() const
{
    return mScale * mRotation * mTranslation;
}

Transform::Transform()
    : mTranslation(1),
      mRotation(1),
      mScale(1)
{
}

void
Transform::SetTranslation(const glm::mat4x4 &translation)
{
    mTranslation = translation;
}
void
Transform::SetRotation(const glm::mat4x4 &rotation)
{
    mRotation = rotation;
}
void
Transform::SetScale(const glm::mat4x4 &scale)
{
    mScale = scale;
}

void
Transform::Translate(float x, float y, float z)
{
    Translate({x, y, z});
}
void
Transform::Translate(const glm::vec3 &offset)
{
    mTranslation = glm::translate(glm::mat4x4(1), offset);
}

void
Transform::Rotate(float pitch, float roll, float yaw)
{
    Rotate({pitch, roll, yaw});
}

void
Transform::Rotate(const glm::vec3 &angles)
{
    mRotation = glm::toMat4(glm::quat(angles));
}

void
Transform::Scale(float factor)
{
    mScale = glm::scale(glm::mat4x4(1), glm::vec3(factor));
}
}  // namespace NEngine::Helpers