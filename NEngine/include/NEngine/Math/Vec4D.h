#pragma once
#include <string>

#include "Vec3D.h"

namespace NEngine::Math {
struct Vec4D
{
    Vec4D();
    Vec4D(float x, float y, float z, float w);
    Vec4D(const Vec3D &v, float w);

    Vec4D(const Vec4D &rhs) = default;
    Vec4D(Vec4D &&rhs) noexcept = default;
    Vec4D &operator=(const Vec4D &rhs) = default;
    Vec4D &operator=(Vec4D &&rhs) noexcept = default;
    bool operator==(const Vec4D &rhs) const;
    float operator[](size_t i) const;
    float &operator[](size_t i);
    Vec3D ToVec3D() const;
    Vec4D Normalize() const;

    std::string ToString() const;
    float Length() const;

    float X;
    float Y;
    float Z;
    float W;
};

Vec4D operator+(const Vec4D &lhs, const Vec4D &rhs);
Vec4D operator-(const Vec4D &lhs, const Vec4D &rhs);
Vec4D operator*(const Vec4D &lhs, const float s);
Vec4D operator*(const float s, const Vec4D &rhs);
Vec4D operator/(const Vec4D &lhs, const float s);
}  // namespace NEngine::Math