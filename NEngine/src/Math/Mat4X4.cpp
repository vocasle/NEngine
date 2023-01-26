#include "NEngine/Math/Mat4X4.h"

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
    : mData{{v0x, v1x, v2x, v3x},
            {v0y, v1y, v2y, v3y},
            {v0z, v1z, v2z, v3z},
            {v0w, v1w, v2w, v3w}}
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
    return {this->operator()(0, i),
            this->operator()(1, i),
            this->operator()(2, i),
            this->operator()(3, i)};
}
Mat4X4
Mat4X4::Add(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat =
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs)) +
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rhs));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(mat));
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
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(mat));
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
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    (XMMatrixTranspose(mat)));
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
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(mat));
#endif
    return ret;
}
Vec4D
Mat4X4::Mult(const Mat4X4 &lhs, const Vec4D &rhs)
{
    auto ret = Vec4D();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&lhs));
    const auto vec = XMVector3Transform(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)), mat);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
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
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(mat));
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
                    XMMatrixTranspose(XMMatrixRotationX(phi)));
#endif
    return ret;
}
Mat4X4
Mat4X4::RotY(float phi)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(XMMatrixRotationY(phi)));
#endif
    return ret;
}
Mat4X4
Mat4X4::RotZ(float phi)
{
    auto ret = Mat4X4();
#if NENGINE_USE_DIRECTXMATH
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret),
                    XMMatrixTranspose(XMMatrixRotationZ(phi)));
#endif
    return ret;
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