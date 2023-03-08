#pragma once

#include <string>

namespace NEngine::Math {
struct vec4;
struct vec3
{
    vec3();
    vec3(float x, float y, float z);
    vec3(const vec3 &rhs) = default;
    vec3(vec3 &&rhs) noexcept = default;
    vec3(const vec4 &rhs);
    vec3(float x);
    vec3 &operator=(const vec3 &rhs) = default;
    vec3 &operator=(vec3 &&rhs) noexcept = default;
    bool operator==(const vec3 &rhs) const;
    bool operator!=(const vec3 &rhs) const;
    float operator[](size_t i) const;
    float &operator[](size_t i);

    std::string ToString() const;

    static vec3 Cross(const vec3 &lhs, const vec3 &rhs);
    static float Dot(const vec3 &lhs, const vec3 &rhs);
    float Length() const;
    vec3 Normalize() const;
    vec3 Rotate(const vec4 &quat) const;

    float X;
    float Y;
    float Z;
};

vec3 operator+(const vec3 &lhs, const vec3 &rhs);
vec3 operator-(const vec3 &lhs, const vec3 &rhs);
vec3 operator*(const vec3 &lhs, const float s);
vec3 operator*(const float s, const vec3 &rhs);
vec3 operator/(const vec3 &lhs, const float s);
}  // namespace NEngine::Math
