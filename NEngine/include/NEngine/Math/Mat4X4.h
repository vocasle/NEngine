#pragma once

#include <string>

#include "Vec4D.h"

namespace NEngine::Math {
class Mat4X4
{
public:
    Mat4X4();
    explicit Mat4X4(float n);
    Mat4X4(float v0x,
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
    Vec4D operator[](size_t i) const;

    static Mat4X4 Add(const Mat4X4 &lhs, const Mat4X4 &rhs);
    static Mat4X4 Subtract(const Mat4X4 &lhs, const Mat4X4 &rhs);
    static Mat4X4 Mult(const Mat4X4 &lhs, const Mat4X4 &rhs);
    static Mat4X4 Mult(const Mat4X4 &lhs, float s);
    static Vec4D Mult(const Mat4X4 &lhs, const Vec4D &rhs);
    float Determinant() const;
    Mat4X4 Inverse() const;
    Mat4X4 Transpose() const;

    static Mat4X4 RotX(float phi);
    static Mat4X4 RotY(float phi);
    static Mat4X4 RotZ(float phi);

    static Mat4X4 Translate(const Vec3D &v);
    static Mat4X4 Scale(const Vec3D &v);
    static Mat4X4 Rotate(const Vec3D &eulerAngles);

    std::string ToString() const;

private:
    float mData[4][4];
};

Mat4X4 operator+(const Mat4X4 &lhs, const Mat4X4 &rhs);
Mat4X4 operator-(const Mat4X4 &lhs, const Mat4X4 &rhs);
Mat4X4 operator*(const Mat4X4 &lhs, const Mat4X4 &rhs);
Mat4X4 operator*(const Mat4X4 &lhs, float s);
Mat4X4 operator*(float s, const Mat4X4 &rhs);
Mat4X4 operator/(const Mat4X4 &lhs, float s);
Vec4D operator*(const Mat4X4 &lhs, const Vec4D &rhs);
bool operator==(const Mat4X4 &lhs, const Mat4X4 &rhs);
}  // namespace NEngine::Math