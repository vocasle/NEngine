#pragma once

#include <limits>
#include <numbers>

namespace NEngine::Math {
struct vec3;

constexpr static const auto PI = std::numbers::pi;
constexpr static const auto EPSILON = 0.000001f;
float ToRadians(float degrees);
float ToDegrees(float radians);
bool NearlyEqual(float lhs, float rhs);
bool NearlyEqual(const vec3 &lhs, const vec3 &rhs);
float ClampToZero(float n);
float Abs(float n);

template <typename T>
T
Clamp(const T &val, const T &min, const T &max)
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
