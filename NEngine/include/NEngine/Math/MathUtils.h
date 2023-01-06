#pragma once

#include <limits>
#include <numbers>

namespace NEngine::Math {
struct Vec3D;

constexpr static const auto PI = std::numbers::pi;
constexpr static const auto EPSILON = 5.0f * std::numeric_limits<float>::epsilon();
float ToRadians(float degrees);
float ToDegrees(float radians);
bool NearlyEqual(float lhs, float rhs);
bool NearlyEqual(const Vec3D &lhs, const Vec3D &rhs);
float ClampToZero(float n);
float Abs(float n);

}  // namespace NEngine::Math
