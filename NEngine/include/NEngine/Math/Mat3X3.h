#pragma once

#include "Vec3D.h"

namespace NEngine::Math {

class Mat3X3
{
public:
    Mat3X3();
    explicit Mat3X3(float n);
    Mat3X3(float v0x,
           float v0y,
           float v0z,
           float v1x,
           float v1y,
           float v1z,
           float v2x,
           float v2y,
           float v2z);

    static Mat3X3 Add(const Mat3X3 &lhs, const Mat3X3 &rhs);
    static Mat3X3 Mult(const Mat3X3 &lhs, const Mat3X3 &rhs);
    static Mat3X3 Mult(const Mat3X3 &lhs, float s);
    static Vec3D Mult(const Mat3X3 &lhs, const Vec3D &rhs);
    float Determinant() const;
    Mat3X3 Inverse() const;
    Mat3X3 Transpose() const;
    float &operator()(size_t i, size_t j);
    float operator()(size_t i, size_t j) const;
    Vec3D operator[](size_t i) const;

    static Mat3X3 RotX(float phi);
    static Mat3X3 RotY(float phi);
    static Mat3X3 RotZ(float phi);

    std::string ToString() const;

private:
    float mData[3][3];
};

Mat3X3 operator+(const Mat3X3 &lhs, const Mat3X3 &rhs);
Mat3X3 operator-(const Mat3X3 &lhs, const Mat3X3 &rhs);
Mat3X3 operator*(const Mat3X3 &lhs, const Mat3X3 &rhs);
Mat3X3 operator*(const Mat3X3 &lhs, float s);
Mat3X3 operator*(float s, const Mat3X3 &rhs);
Mat3X3 operator/(const Mat3X3 &lhs, float s);
Mat3X3 operator/(float s, const Mat3X3 &rhs);
Vec3D operator*(const Mat3X3 &lhs, const Vec3D &rhs);
bool operator==(const Mat3X3 &lhs, const Mat3X3 &rhs);
}  // namespace NEngine::Math