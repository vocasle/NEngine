#pragma once

#include <string>

namespace NEngine::Math {
struct Vec3D
{
    Vec3D()
        : X{0},
          Y{0},
          Z{0}
    {
    }

    Vec3D(float x, float y, float z)
        : X{x},
          Y{y},
          Z{z}
    {
    }
    std::string ToString() const;
    Vec3D Cross(const Vec3D &rhs) const;
    float Dot(const Vec3D &rhs) const;
    float Length() const;
    void Normalize();
    bool operator==(const Vec3D &rhs) const;
    bool operator!=(const Vec3D &rhs) const;

    Vec3D(const Vec3D &rhs) = default;
    Vec3D(Vec3D &&rhs) noexcept = default;
    Vec3D &operator=(const Vec3D &rhs) = default;
    Vec3D &operator=(Vec3D &&rhs) noexcept = default;

    float X;
    float Y;
    float Z;
};

Vec3D operator+(const Vec3D &lhs, const Vec3D &rhs);
Vec3D operator-(const Vec3D &lhs, const Vec3D &rhs);
Vec3D operator*(const Vec3D &lhs, const float s);
Vec3D operator*(const float s, const Vec3D &rhs);
}  // namespace NEngine::Math