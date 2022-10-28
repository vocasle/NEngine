#pragma once

#include "NEngine/Math/Math.h"

namespace NEngine::Helpers {
class Transform
{
public:
    Transform() = default;

    const Math::Mat4X4 &GetTranslate() const;
    const Math::Mat4X4 &GetRotate() const;
    const Math::Mat4X4 &GetScale() const;
    const Math::Mat4X4 GetTransform() const;

    void SetTranslate(const Math::Mat4X4 &translate);
    void SetRotate(const Math::Mat4X4 &rotate);
    void SetScale(const Math::Mat4X4 &scale);

    void Translate(float x, float y, float z);
    void Translate(const Math::Vec3D &offset);

    void Rotate(float pitch, float roll, float yaw);
    void Rotate(const Math::Vec3D &angles);

    void Scale(float factor);

private:
    Math::Mat4X4 mTranslate;
    Math::Mat4X4 mRotate;
    Math::Mat4X4 mScale;
};
}  // namespace NEngine::Helpers