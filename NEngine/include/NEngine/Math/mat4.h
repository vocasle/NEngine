#pragma once

#include <string>

#include "vec4.h"

namespace NEngine::Math {
class mat4
{
public:
    mat4();
    explicit mat4(float n);
    mat4(float v0x,
           float v0y,
           float v0z,
           float v0w,
           float v1x,
           float v1y,
           float v1z,
           float v1w,
           float v2x,
           float v2y,
           float v2z,
           float v2w,
           float v3x,
           float v3y,
           float v3z,
           float v3w);

    float &operator()(size_t i, size_t j);
    float operator()(size_t i, size_t j) const;
    vec4 operator[](size_t i) const;

    static mat4 Add(const mat4 &lhs, const mat4 &rhs);
    static mat4 Subtract(const mat4 &lhs, const mat4 &rhs);
    static mat4 Mult(const mat4 &lhs, const mat4 &rhs);
    static mat4 Mult(const mat4 &lhs, float s);
    static vec4 Mult(const mat4 &lhs, const vec4 &rhs);
    float Determinant() const;
    mat4 Inverse() const;
    mat4 Transpose() const;

    static mat4 RotX(float phi);
    static mat4 RotY(float phi);
    static mat4 RotZ(float phi);

    static mat4 Translate(const vec3 &v);
    static mat4 Scale(const vec3 &v);
    static mat4 Rotate(const vec3 &eulerAngles);

    std::string ToString() const;

private:
    float mData[4][4];
};

mat4 operator+(const mat4 &lhs, const mat4 &rhs);
mat4 operator-(const mat4 &lhs, const mat4 &rhs);
mat4 operator*(const mat4 &lhs, const mat4 &rhs);
mat4 operator*(const mat4 &lhs, float s);
mat4 operator*(float s, const mat4 &rhs);
mat4 operator/(const mat4 &lhs, float s);
vec4 operator*(const mat4 &lhs, const vec4 &rhs);
bool operator==(const mat4 &lhs, const mat4 &rhs);
}  // namespace NEngine::Math