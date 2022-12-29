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
    return mData[i][j];
}
float
Mat3X3::operator()(size_t i, size_t j) const
{
    return mData[i][j];
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
    auto ret = Mat3X3();

    const auto col0 = rhs[0];
    const auto col1 = rhs[1];
    const auto col2 = rhs[2];

    const auto v0 = Mult(lhs, col0);
    const auto v1 = Mult(lhs, col1);
    const auto v2 = Mult(lhs, col2);
    ret(0, 0) = v0.X;
    ret(1, 0) = v0.Y;
    ret(2, 0) = v0.Z;

    ret(0, 1) = v1.X;
    ret(1, 1) = v1.Y;
    ret(2, 1) = v1.Z;

    ret(0, 2) = v2.X;
    ret(1, 2) = v2.Y;
    ret(2, 2) = v2.Z;
    return ret;
}

Mat3X3
Mat3X3::Transpose() const
{
    auto ret = *this;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(j, i) = ret(i, j);
        }
    }
    return ret;
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
    const auto row0 = Vec3D(lhs(0, 0), lhs(0, 1), lhs(0, 2));
    const auto row1 = Vec3D(lhs(1, 0), lhs(1, 1), lhs(1, 2));
    const auto row2 = Vec3D(lhs(2, 0), lhs(2, 1), lhs(2, 2));
    ret.X = Vec3D::Dot(row0, rhs);
    ret.Y = Vec3D::Dot(row1, rhs);
    ret.Z = Vec3D::Dot(row2, rhs);
    return ret;
}

Mat3X3
Mat3X3::Subtract(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    auto ret = Mat3X3();
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = lhs(i, j) - rhs(i, j);
        }
    }
    return ret;
}

Mat3X3
Mat3X3::RotZ(float phi)
{
    const auto cos = ClampToZero(cosf(phi));
    const auto sin = ClampToZero(sinf(phi));
    auto ret = Mat3X3();
    ret(0, 0) = cos;
    ret(1, 0) = sin;
    ret(0, 1) = -sin;
    ret(1, 1) = cos;
    return ret;
}

Vec3D
Mat3X3::operator[](size_t i) const
{
    return {
        this->operator()(0, i), this->operator()(1, i), this->operator()(2, i)};
}

bool
operator==(const Mat3X3 &lhs, const Mat3X3 &rhs)
{
    bool isEqual = true;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            isEqual = isEqual && NearlyEqual(lhs(i, j), rhs(i, j));
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