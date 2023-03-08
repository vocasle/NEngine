#include "NEngine/Math/mat4.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "NEngine/Math/MathUtils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif


namespace NEngine::Math {
mat4::mat4()
    : mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
{
}
mat4::mat4(float n)
    : mat4(n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n)
{
}
mat4::mat4(float v0x,
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
               float v3w)
    : mData{{v0x, v0y, v0z, v0w},
            {v1x, v1y, v1z, v1w},
            {v2x, v2y, v2z, v2w},
            {v3x, v3y, v3z, v3w}}
{
}
float &
mat4::operator()(size_t i, size_t j)
{
    return mData[i][j];
}
float
mat4::operator()(size_t i, size_t j) const
{
    return mData[i][j];
}
vec4
mat4::operator[](size_t i) const
{
    return {this->operator()(i, 0),
            this->operator()(i, 1),
            this->operator()(i, 2),
            this->operator()(i, 3)};
}
mat4
mat4::Add(const mat4 &lhs, const mat4 &rhs)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) +
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
mat4
mat4::Subtract(const mat4 &lhs, const mat4 &rhs)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) +
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
mat4
mat4::Mult(const mat4 &lhs, const mat4 &rhs)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) *
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), (mat));
#endif
    return ret;
}
mat4
mat4::Mult(const mat4 &lhs, float s)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&ret)) * s;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
vec4
mat4::Mult(const mat4 &lhs, const vec4 &rhs)
{
    auto ret = vec4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs));
    const auto vec = XMVector4Transform(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&rhs)), mat);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
#endif
    return ret;
}
float
mat4::Determinant() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMMatrixDeterminant(
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(this)));
    auto det = 0.0f;
    XMStoreFloat(&det, vec);
    return det;
#endif
}
mat4
mat4::Inverse() const
{
#if NENGINE_USE_DIRECTXMATH
    XMVECTOR det;
    const auto mat = XMMatrixInverse(
        &det, XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(this)));
    auto ret = mat4();
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
    return ret;
#endif
}
mat4
mat4::Transpose() const
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = *this;
    const auto mat = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&ret));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(mat));
    return ret;
#endif
}
mat4
mat4::RotX(float phi)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixRotationX(phi));
#endif
    return ret;
}
mat4
mat4::RotY(float phi)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixRotationY(phi));
#endif
    return ret;
}
mat4
mat4::RotZ(float phi)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixRotationZ(phi));
#endif
    return ret;
}

mat4
mat4::Translate(const vec3 &v)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixTranslationFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&v)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

mat4
mat4::Scale(const vec3 &v)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixScalingFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&v)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

mat4
mat4::Rotate(const vec3 &eulerAngles)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixRotationRollPitchYawFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&eulerAngles)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

std::string
mat4::ToString() const
{
    std::ostringstream out;
    const auto &self = *this;

    out << std::fixed << std::setprecision(4) << "\n"

        << std::setw(4) << self(0, 0) << ' ' << std::setw(4) << self(0, 1)
        << ' ' << std::setw(4) << self(0, 2) << ' ' << std::setw(4)
        << self(0, 3) << "\n"

        << std::setw(4) << self(1, 0) << ' ' << std::setw(4) << self(1, 1)
        << ' ' << std::setw(4) << self(1, 2) << ' ' << std::setw(4)
        << self(1, 3) << "\n"

        << std::setw(4) << self(2, 0) << ' ' << std::setw(4) << self(2, 1)
        << ' ' << std::setw(4) << self(2, 2) << ' ' << std::setw(4)
        << self(2, 3) << "\n"

        << std::setw(4) << self(3, 0) << ' ' << std::setw(4) << self(3, 1)
        << ' ' << std::setw(4) << self(3, 2) << ' ' << std::setw(4)
        << self(3, 3) << "\n";

    return out.str();
}

mat4
NEngine::Math::operator+(const mat4 &lhs, const mat4 &rhs)
{
    return mat4::Add(lhs, rhs);
}

mat4
operator-(const mat4 &lhs, const mat4 &rhs)
{
    return mat4::Subtract(lhs, rhs);
}

mat4
operator*(const mat4 &lhs, const mat4 &rhs)
{
    return mat4::Mult(lhs, rhs);
}

mat4
operator*(const mat4 &lhs, float s)
{
    return mat4::Mult(lhs, s);
}

mat4
operator*(float s, const mat4 &rhs)
{
    return mat4::Mult(rhs, s);
}

mat4
operator/(const mat4 &lhs, float s)
{
    return mat4::Mult(lhs, 1 / s);
}

vec4
operator*(const mat4 &lhs, const vec4 &rhs)
{
    return mat4::Mult(lhs, rhs);
}

bool
operator==(const mat4 &lhs, const mat4 &rhs)
{
    bool isEqual = true;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            isEqual = isEqual && NearlyEqual(lhs(i, j), rhs(i, j));
        }
    }
    return isEqual;
}

}  // namespace NEngine::Math