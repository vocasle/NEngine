#include "NEngine/Math/mat3.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "NEngine/Math/MathUtils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {

mat3::mat3()
    : mat3(1, 0, 0, 0, 1, 0, 0, 0, 1)
{
}

mat3::mat3(float n)
    : mat3(n, n, n, n, n, n, n, n, n)
{
}

mat3::mat3(float v0x,
           float v0y,
           float v0z,
           float v1x,
           float v1y,
           float v1z,
           float v2x,
           float v2y,
           float v2z)
    : mData{{v0x, v0y, v0z}, {v1x, v1y, v1z}, {v2x, v2y, v2z}}
{
}

float &
mat3::operator()(size_t i, size_t j)
{
    return mData[i][j];
}
float
mat3::operator()(size_t i, size_t j) const
{
    return mData[i][j];
}

mat3
mat3::Add(const mat3 &lhs, const mat3 &rhs)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&lhs)) +
        XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&rhs));
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret), mat);
#else
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = lhs(i, j) + rhs(i, j);
        }
    }
#endif
    return ret;
}

mat3
mat3::Mult(const mat3 &lhs, const mat3 &rhs)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    const auto m1 = XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&lhs));
    const auto m2 = XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&rhs));
    const auto mat = m1 * m2;
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret), mat);
#else
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
#endif
    return ret;
}

mat3
mat3::Transpose() const
{
    auto ret = *this;
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&ret));
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret),
                    XMMatrixTranspose(mat));
#else
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = operator()(j, i);
        }
    }
#endif
    return ret;
}

mat3
mat3::Mult(const mat3 &lhs, float s)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&lhs)) * s;
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret), mat);
#else
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = lhs(i, j) * s;
        }
    }
#endif
    return ret;
}

vec3
mat3::Mult(const mat3 &lhs, const vec3 &rhs)
{
    auto ret = vec3();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&lhs));
    const auto vec = XMVector3Transform(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)), mat);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
#else
    const auto row0 = vec3(lhs(0, 0), lhs(0, 1), lhs(0, 2));
    const auto row1 = vec3(lhs(1, 0), lhs(1, 1), lhs(1, 2));
    const auto row2 = vec3(lhs(2, 0), lhs(2, 1), lhs(2, 2));
    ret.X = vec3::Dot(row0, rhs);
    ret.Y = vec3::Dot(row1, rhs);
    ret.Z = vec3::Dot(row2, rhs);
#endif
    return ret;
}

mat3
mat3::Subtract(const mat3 &lhs, const mat3 &rhs)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&lhs)) +
        XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(&rhs));
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret), mat);
#else
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ret(i, j) = lhs(i, j) - rhs(i, j);
        }
    }
#endif
    return ret;
}

mat3
mat3::RotZ(float phi)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixRotationZ(phi);
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret), mat);
#else
    const auto cos = std::cos(phi);
    const auto sin = std::sin(phi);
    ret(0, 0) = cos;
    ret(0, 1) = -sin;
    ret(1, 0) = sin;
    ret(1, 1) = cos;
#endif
    return ret;
}

float
mat3::Determinant() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMMatrixDeterminant(
        XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(this)));
    auto det = 0.0f;
    XMStoreFloat(&det, vec);
    return det;
#else
    const auto &self = *this;
    const auto minor00 = self(1, 1) * self(2, 2) - self(2, 1) * self(1, 2);
    const auto minor01 = self(1, 0) * self(2, 2) - self(2, 0) * self(1, 2);
    const auto minor02 = self(1, 0) * self(2, 1) - self(2, 0) * self(1, 1);
    return self(0, 0) * minor00 - self(0, 1) * minor01 + self(0, 2) * minor02;
#endif
}

mat3
mat3::Inverse() const
{
#if NENGINE_USE_DIRECTXMATH
    XMVECTOR det;
    const auto mat = XMMatrixInverse(
        &det, XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3 *>(this)));
    auto ret = mat3();
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret), mat);
    return ret;
#else
    const auto det = Determinant();
    UTILS_ASSERT(det != 0, "Determinant is zero, matrix has no inverse!");
    const auto &self = *this;
    auto ret = mat3();

    /*
     * | a00 a01 a02 |
     * | a10 a11 a12 |
     * | a20 a21 a22 |
     */

    const auto A00 = self(1, 1) * self(2, 2) - self(2, 1) * self(1, 2);
    const auto A01 = self(1, 0) * self(2, 2) - self(2, 0) * self(1, 2);
    const auto A02 = self(1, 0) * self(2, 1) - self(2, 0) * self(1, 1);
    const auto A10 = self(0, 1) * self(2, 2) - self(2, 1) * self(0, 2);
    const auto A11 = self(0, 0) * self(2, 2) - self(2, 0) * self(0, 2);
    const auto A12 = self(0, 0) * self(2, 1) - self(2, 0) * self(0, 1);
    const auto A20 = self(0, 1) * self(1, 2) - self(1, 1) * self(0, 2);
    const auto A21 = self(0, 0) * self(1, 2) - self(1, 0) * self(0, 2);
    const auto A22 = self(0, 0) * self(1, 1) - self(1, 0) * self(0, 1);

    ret(0, 0) = A00;
    ret(0, 1) = -A01;
    ret(0, 2) = A02;
    ret(1, 0) = -A10;
    ret(1, 1) = A11;
    ret(1, 2) = -A12;
    ret(2, 0) = A20;
    ret(2, 1) = -A21;
    ret(2, 2) = A22;
    ret = ret.Transpose();

    return (1 / det) * ret;
#endif
}

mat3
mat3::RotX(float phi)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret),
                    XMMatrixRotationX(phi));
#else
    const auto cos = std::cos(phi);
    const auto sin = std::sin(phi);
    ret(1, 1) = cos;
    ret(1, 2) = -sin;
    ret(2, 1) = sin;
    ret(2, 2) = cos;
#endif
    return ret;
}

mat3
mat3::RotY(float phi)
{
    auto ret = mat3();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat3x3(reinterpret_cast<XMFLOAT3X3 *>(&ret),
                    XMMatrixRotationY(phi));
#else
    const auto cos = std::cos(phi);
    const auto sin = std::sin(phi);
    ret(0, 0) = cos;
    ret(0, 2) = sin;
    ret(2, 0) = -sin;
    ret(2, 2) = cos;
#endif
    return ret;
}

vec3
mat3::operator[](size_t i) const
{
    return {
        this->operator()(i, 0), this->operator()(i, 1), this->operator()(i, 2)};
}

bool
operator==(const mat3 &lhs, const mat3 &rhs)
{
    bool isEqual = true;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            isEqual = isEqual && NearlyEqual(lhs(i, j), rhs(i, j));
        }
    }
    return isEqual;
}

mat3
operator+(const mat3 &lhs, const mat3 &rhs)
{
    return mat3::Add(lhs, rhs);
}
mat3
operator-(const mat3 &lhs, const mat3 &rhs)
{
    return lhs + (rhs * (-1));
}
mat3
operator*(const mat3 &lhs, const mat3 &rhs)
{
    return mat3::Mult(lhs, rhs);
}
mat3
operator*(const mat3 &lhs, float s)
{
    return mat3::Mult(lhs, s);
}
mat3
operator*(float s, const mat3 &rhs)
{
    return rhs * s;
}
mat3
operator/(const mat3 &lhs, float s)
{
    return lhs * (1 / s);
}
vec3
operator*(const mat3 &lhs, const vec3 &rhs)
{
    return mat3::Mult(lhs, rhs);
}

std::string
mat3::ToString() const
{
    std::ostringstream out;
    const auto &self = *this;

    out << std::fixed << std::setprecision(4) << "\n"
        << std::setw(4) << self(0, 0) << ' ' << std::setw(4) << self(0, 1)
        << ' ' << std::setw(4) << self(0, 2) << "\n"
        << std::setw(4) << self(1, 0) << ' ' << std::setw(4) << self(1, 1)
        << ' ' << std::setw(4) << self(1, 2) << "\n"
        << std::setw(4) << self(2, 0) << ' ' << std::setw(4) << self(2, 1)
        << ' ' << std::setw(4) << self(2, 2);

    return out.str();
}

}  // namespace NEngine::Math
