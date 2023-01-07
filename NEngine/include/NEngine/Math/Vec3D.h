#pragma once

#include <string>

namespace NEngine::Math {
struct Vec3D
{
    Vec3D();
    Vec3D(float x, float y, float z);
    Vec3D(const Vec3D &rhs) = default;
    Vec3D(Vec3D &&rhs) noexcept = default;
    Vec3D &operator=(const Vec3D &rhs) = default;
    Vec3D &operator=(Vec3D &&rhs) noexcept = default;
    bool operator==(const Vec3D &rhs) const;
    bool operator!=(const Vec3D &rhs) const;
    float operator[](size_t i) const;
    float &operator[](size_t i);

    std::string ToString() const;

    static Vec3D Cross(const Vec3D &lhs, const Vec3D &rhs);
    static float Dot(const Vec3D &lhs, const Vec3D &rhs);
    float Length() const;
    Vec3D &Normalize();
    
    float X;
    float Y;
    float Z;
};

Vec3D operator+(const Vec3D &lhs, const Vec3D &rhs);
Vec3D operator-(const Vec3D &lhs, const Vec3D &rhs);
Vec3D operator*(const Vec3D &lhs, const float s);
Vec3D operator*(const float s, const Vec3D &rhs);
Vec3D operator/(const Vec3D &lhs, const float s);
}  // namespace NEngine::Math
