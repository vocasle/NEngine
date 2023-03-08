#pragma once
#include <string>

#include "vec3.h"

namespace NEngine::Math {
struct vec4
{
    vec4();
    vec4(float x, float y, float z, float w);
    vec4(const vec3 &v, float w);

    vec4(const vec4 &rhs) = default;
    vec4(vec4 &&rhs) noexcept = default;
    vec4 &operator=(const vec4 &rhs) = default;
    vec4 &operator=(vec4 &&rhs) noexcept = default;
    bool operator==(const vec4 &rhs) const;
    float operator[](size_t i) const;
    float &operator[](size_t i);
    vec3 ToVec3D() const;
    vec4 Normalize() const;

    std::string ToString() const;
    float Length() const;

    float X;
    float Y;
    float Z;
    float W;
};

vec4 operator+(const vec4 &lhs, const vec4 &rhs);
vec4 operator-(const vec4 &lhs, const vec4 &rhs);
vec4 operator*(const vec4 &lhs, const float s);
vec4 operator*(const float s, const vec4 &rhs);
vec4 operator/(const vec4 &lhs, const float s);
}  // namespace NEngine::Math