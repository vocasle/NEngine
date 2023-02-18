#include "NEngine/Math/Mat4X4.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Utils/Utils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

using namespace NEngine::Utils;

namespace NEngine::Math {
Mat4X4::Mat4X4()
    : Mat4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
{
}
Mat4X4::Mat4X4(float n)
    : Mat4X4(n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n)
{
}
Mat4X4::Mat4X4(float v0x,
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
Mat4X4::operator()(size_t i, size_t j)
{
    UTILS_ASSERT(i < 4 && j < 4, "i and j must be in range [0, 4)");
    return mData[i][j];
}
float
Mat4X4::operator()(size_t i, size_t j) const
{
    UTILS_ASSERT(i < 4 && j < 4, "i and j must be in range [0, 4)");
    return mData[i][j];
}
Vec4D
Mat4X4::operator[](size_t i) const
{
    return {this->operator()(i, 0),
            this->operator()(i, 1),
            this->operator()(i, 2),
            this->operator()(i, 3)};
}
Mat4X4
Mat4X4::Add(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) +
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
Mat4X4
Mat4X4::Subtract(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) +
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
Mat4X4
Mat4X4::Mult(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) *
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), (mat));
#endif
    return ret;
}
Mat4X4
Mat4X4::Mult(const Mat4X4 &lhs, float s)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&ret)) * s;
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
Vec4D
Mat4X4::Mult(const Mat4X4 &lhs, const Vec4D &rhs)
{
    auto ret = Vec4D();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs));
    const auto vec = XMVector4Transform(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&rhs)), mat);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
#endif
    return ret;
}
float
Mat4X4::Determinant() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMMatrixDeterminant(
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(this)));
    auto det = 0.0f;
    XMStoreFloat(&det, vec);
    return det;
#endif
}
Mat4X4
Mat4X4::Inverse() const
{
#if NENGINE_USE_DIRECTXMATH
    XMVECTOR det;
    const auto mat = XMMatrixInverse(
        &det, XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(this)));
    auto ret = Mat4X4();
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
    return ret;
#endif
}
Mat4X4
Mat4X4::Transpose() const
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = *this;
    const auto mat = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&ret));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(mat));
    return ret;
#endif
}
Mat4X4
Mat4X4::RotX(float phi)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixRotationX(phi));
#endif
    return ret;
}
Mat4X4
Mat4X4::RotY(float phi)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixRotationY(phi));
#endif
    return ret;
}
Mat4X4
Mat4X4::RotZ(float phi)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixRotationZ(phi));
#endif
    return ret;
}

Mat4X4
Mat4X4::Translate(const Vec3D &v)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixTranslationFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&v)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

Mat4X4
Mat4X4::Scale(const Vec3D &v)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixScalingFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&v)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

Mat4X4
Mat4X4::Rotate(const Vec3D &eulerAngles)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixRotationRollPitchYawFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&eulerAngles)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

std::string
Mat4X4::ToString() const
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

Mat4X4
NEngine::Math::operator+(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    return Mat4X4::Add(lhs, rhs);
}

Mat4X4
operator-(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    return Mat4X4::Subtract(lhs, rhs);
}

Mat4X4
operator*(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    return Mat4X4::Mult(lhs, rhs);
}

Mat4X4
operator*(const Mat4X4 &lhs, float s)
{
    return Mat4X4::Mult(lhs, s);
}

Mat4X4
operator*(float s, const Mat4X4 &rhs)
{
    return Mat4X4::Mult(rhs, s);
}

Mat4X4
operator/(const Mat4X4 &lhs, float s)
{
    return Mat4X4::Mult(lhs, 1 / s);
}

Vec4D
operator*(const Mat4X4 &lhs, const Vec4D &rhs)
{
    return Mat4X4::Mult(lhs, rhs);
}

bool
operator==(const Mat4X4 &lhs, const Mat4X4 &rhs)
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