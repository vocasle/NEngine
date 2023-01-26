#include "NEngine/Math/Vec4D.h"

#include <sstream>

#include "NEngine/Utils/Utils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {
Vec4D::Vec4D()
    : X{0},
      Y{0},
      Z{0},
      W{0}
{
}

Vec4D::Vec4D(float x, float y, float z, float w)
    : X{x},
      Y{y},
      Z{z},
      W{w}
{
}
Vec4D::Vec4D(const Vec3D &v, float w)
    : Vec4D(v.X, v.Y, v.Z, w)
{
}

std::string
Vec4D::ToString() const
{
    std::stringstream out;
    out << "{ " << X << ", " << Y << ", " << Z << ", " << W << " }";
    return out.str();
}

float
Vec4D::Length() const
{
#if NENGINE_USE_DIRECTXMATH
    auto len = 0.0f;
    const auto vec =
        XMVector4Length(XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(this)));
    XMStoreFloat(&len, vec);
    return len;
#else
    return sqrtf(X * X + Y * Y + Z * Z + W * W);
#endif
}

bool
Vec4D::operator==(const Vec4D &rhs) const
{
    return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
}

float
Vec4D::operator[](size_t i) const
{
    return operator[](i);
}

float &
Vec4D::operator[](size_t i)
{
    switch (i) {
        case 0:
            return X;
        case 1:
            return Y;
        case 2:
            return Z;
        case 3:
            return W;
        default:
            throw std::invalid_argument(
                Utils::UtilsFormatStr("%llu is out of bounds", i));
    }
}

Vec3D
Vec4D::ToVec3D() const
{
    return Vec3D(X, Y, Z);
}

Vec4D
Vec4D::Normalize() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMVector4Normalize(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(this)));
    auto ret = Vec4D();
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    auto ret = *this;
    const auto length = Length();
    if (length != 0) {
        ret = ret / length;
    }
    return ret;
#endif
}

Vec4D
operator+(const Vec4D &lhs, const Vec4D &rhs)
{
    UTILS_ASSERT(lhs.W == rhs.W, "Cannot mix vector and point");
#if NENGINE_USE_DIRECTXMATH
    auto ret = Vec4D();
    const auto vec =
        XMVectorAdd(XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&lhs)),
                    XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&rhs)));
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W};
#endif
}

Vec4D
operator-(const Vec4D &lhs, const Vec4D &rhs)
{
    UTILS_ASSERT(lhs.W == rhs.W, "Cannot mix vector and point");
#if NENGINE_USE_DIRECTXMATH
    auto ret = Vec4D();
    const auto vec = XMVectorSubtract(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&lhs)),
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&rhs)));
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W};
#endif
}

Vec4D
operator*(const Vec4D &lhs, const float s)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = Vec4D();
    const auto vec = XMVectorScale(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&lhs)), s);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    return {lhs.X * s, lhs.Y * s, lhs.Z * s, lhs.W};
#endif
}
Vec4D
operator*(const float s, const Vec4D &rhs)
{
    return rhs * s;
}
Vec4D
operator/(const Vec4D &lhs, const float s)
{
    return lhs * (1 / s);
}
Vec4D
operator/(const float s, const Vec4D &rhs)
{
    return rhs / s;
}
}  // namespace NEngine::Math