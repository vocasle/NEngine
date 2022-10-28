#pragma once

#include "NEngine/Math/Math.h"

namespace NEngine::Helpers {
class Transform
{
public:
    Transform() = default;

    const Math::Mat4X4 &GetTranslation() const;
    const Math::Mat4X4 &GetRotation() const;
    const Math::Mat4X4 &GetScale() const;
    const Math::Mat4X4 GetTransform() const;

    void SetTranslation(const Math::Mat4X4 &translation);
    void SetRotation(const Math::Mat4X4 &rotation);
    void SetScale(const Math::Mat4X4 &scale);

    void Translate(float x, float y, float z);
    void Translate(const Math::Vec3D &offset);

    void Rotate(float pitch, float roll, float yaw);
    void Rotate(const Math::Vec3D &angles);

    void Scale(float factor);

private:
    Math::Mat4X4 mTranslation;
    Math::Mat4X4 mRotation;
    Math::Mat4X4 mScale;
};
}  // namespace NEngine::Helpers