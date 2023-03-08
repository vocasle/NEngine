#pragma once

#include "vec3.h"

namespace NEngine::Math {

class mat3
{
public:
    mat3();
    explicit mat3(float n);
    mat3(float v0x,
           float v0y,
           float v0z,
           float v1x,
           float v1y,
           float v1z,
           float v2x,
           float v2y,
           float v2z);

    float &operator()(size_t i, size_t j);
    float operator()(size_t i, size_t j) const;
    vec3 operator[](size_t i) const;

    static mat3 Add(const mat3 &lhs, const mat3 &rhs);
    static mat3 Subtract(const mat3 &lhs, const mat3 &rhs);
    static mat3 Mult(const mat3 &lhs, const mat3 &rhs);
    static mat3 Mult(const mat3 &lhs, float s);
    static vec3 Mult(const mat3 &lhs, const vec3 &rhs);
    float Determinant() const;
    mat3 Inverse() const;
    mat3 Transpose() const;

    static mat3 RotX(float phi);
    static mat3 RotY(float phi);
    static mat3 RotZ(float phi);

    std::string ToString() const;

private:
    float mData[3][3];
};

mat3 operator+(const mat3 &lhs, const mat3 &rhs);
mat3 operator-(const mat3 &lhs, const mat3 &rhs);
mat3 operator*(const mat3 &lhs, const mat3 &rhs);
mat3 operator*(const mat3 &lhs, float s);
mat3 operator*(float s, const mat3 &rhs);
mat3 operator/(const mat3 &lhs, float s);
vec3 operator*(const mat3 &lhs, const vec3 &rhs);
bool operator==(const mat3 &lhs, const mat3 &rhs);
}  // namespace NEngine::Math