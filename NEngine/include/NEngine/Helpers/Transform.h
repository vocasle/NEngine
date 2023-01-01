#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace NEngine::Helpers {
class Transform
{
public:
    Transform();

    const glm::mat4x4 &GetTranslation() const;
    const glm::mat4x4 &GetRotation() const;
    const glm::mat4x4 &GetScale() const;
    const glm::mat4x4 GetTransform() const;

    void SetTranslation(const glm::mat4x4 &translation);
    void SetRotation(const glm::mat4x4 &rotation);
    void SetScale(const glm::mat4x4 &scale);

    void Translate(float x, float y, float z);
    void Translate(const glm::vec3 &offset);

    void Rotate(float pitch, float roll, float yaw);
    void Rotate(const glm::vec3 &angles);

    void Scale(float factor);

private:
    glm::mat4x4 mTranslation;
    glm::mat4x4 mRotation;
    glm::mat4x4 mScale;
};
}  // namespace NEngine::Helpers