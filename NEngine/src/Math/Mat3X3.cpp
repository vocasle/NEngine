#include "NEngine/Math/Mat3X3.h"

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine::Math {

Mat3X3::Mat3X3()
    : Mat3X3(1, 0, 0, 0, 1, 0, 0, 0, 1)
{
}

Mat3X3::Mat3X3(float n)
    : Mat3X3(n, n, n, n, n, n, n, n, n)
{
}

Mat3X3::Mat3X3(float v0x,
               float v0y,
               float v0z,
               float v1x,
               float v1y,
               float v1z,
               float v2x,
               float v2y,
               float v2z)
    : mData{{v0x, v1x, v2x}, {v0y, v1y, v2y}, {v0z, v1z, v2z}}
{
}

float &
Mat3X3::operator()(size_t i, size_t j)
{
    return mData[j][i];
}
float
Mat3X3::operator()(size_t i, size_t j) const
{
    return mData[j][i];
}

Mat3X3
Mat3X3::Add(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    auto ret = Mat3X3();
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = lhs(i, j) + rhs(i, j);
        }
    }
    return ret;
}

Mat3X3
Mat3X3::Mult(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    return Mat3X3();
}

Mat3X3
Mat3X3::Mult(const Mat3X3 &lhs, float s)
{
    auto ret = Mat3X3();
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = lhs(i, j) * s;
        }
    }
    return ret;
}

Vec3D
Mat3X3::Mult(const Mat3X3 &lhs, const Vec3D &rhs)
{
    auto ret = Vec3D();
    ret.X = Vec3D::Dot(lhs[0], rhs);
    ret.Y = Vec3D::Dot(lhs[1], rhs);
    ret.Z = Vec3D::Dot(lhs[2], rhs);
    return ret;
}

// Mat3X3
// Mat3X3::RotX(float phi)
//{
//     const auto cos = cosf(phi);
//     const auto sin = sinf(phi);
//
// }

Mat3X3
Mat3X3::RotZ(float phi)
{
    const auto cos = ClampToZero(cosf(phi));
    const auto sin = ClampToZero(sinf(phi));
    auto ret = Mat3X3();
    ret(0, 0) = cos;
    ret(0, 1) = sin;
    ret(1, 0) = -sin;
    ret(1, 1) = cos;
    return ret;
}

Vec3D
Mat3X3::operator[](size_t i) const
{
    return {
        this->operator()(i, 0), this->operator()(i, 1), this->operator()(i, 2)};
}

bool
operator==(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    bool isEqual = true;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            isEqual = isEqual && lhs(i, j) == rhs(i, j);
        }
    }
    return isEqual;
}

Mat3X3
operator+(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    return Mat3X3::Add(lhs, rhs);
}
Mat3X3
operator-(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    return lhs + (rhs * (-1));
}
Mat3X3
operator*(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    return Mat3X3::Mult(lhs, rhs);
}
Mat3X3
operator*(const Mat3X3 &lhs, float s)
{
    return Mat3X3::Mult(lhs, s);
}
Mat3X3
operator*(float s, const Mat3X3 &rhs)
{
    return rhs * s;
}
Mat3X3
operator/(const Mat3X3 &lhs, float s)
{
    return lhs * (1 / s);
}
Mat3X3
operator/(float s, const Mat3X3 &rhs)
{
    return rhs / s;
}
Vec3D
operator*(const Mat3X3 &lhs, const Vec3D &rhs)
{
    return Mat3X3::Mult(lhs, rhs);
}
}  // namespace NEngine::Math