#pragma once

#include <limits>
#include <numbers>

namespace NEngine::Math {
struct Vec3D;

constexpr static const auto PI = std::numbers::pi;
constexpr static const auto EPSILON = 0.000001f;
float ToRadians(float degrees);
float ToDegrees(float radians);
bool NearlyEqual(float lhs, float rhs);
bool NearlyEqual(const Vec3D &lhs, const Vec3D &rhs);
float ClampToZero(float n);
float Abs(float n);

template <typename T>
T
Clamp(const T &min, const T &max, const T &val)
{
    if (val >= max) {
        return max;
    }
    else if (val <= min) {
        return min;
    }
    return val;
}

}  // namespace NEngine::Math
