#include "NEngine/Math/Vec3D.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/Vec4D.h"
#include "NEngine/Utils/Utils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {
std::string
Vec3D::ToString() const
{
    std::stringstream out;
    out << "{ " << X << ", " << Y << ", " << Z << " }";
    return out.str();
}

Vec3D::Vec3D(float x, float y, float z)
    : X{x},
      Y{y},
      Z{z}
{
}

Vec3D::Vec3D(const Vec4D &rhs)
    : X(rhs.X),
      Y(rhs.Y),
      Z(rhs.Z)
{
}

Vec3D::Vec3D(float x)
    : Vec3D(x, x, x)
{
}

Vec3D::Vec3D()
    : Vec3D(0, 0, 0)
{
}

float
Vec3D::Length() const
{
#if NENGINE_USE_DIRECTXMATH
    auto len = 0.0f;
    const auto vec =
        XMVector3Length(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(this)));
    XMStoreFloat(&len, vec);
    return len;
#else
    return sqrtf(X * X + Y * Y + Z * Z);
#endif
}

Vec3D
Vec3D::Cross(const Vec3D &lhs, const Vec3D &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec =
        XMVector3Cross(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
                       XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    auto ret = Vec3D();
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.Y * rhs.Z - lhs.Z * rhs.Y,
            lhs.Z * rhs.X - lhs.X * rhs.Z,
            lhs.X * rhs.Y - lhs.Y * rhs.X};
#endif
}

float
Vec3D::Dot(const Vec3D &lhs, const Vec3D &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec =
        XMVector3Dot(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
                     XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    auto ret = 0.0f;
    XMStoreFloat(&ret, vec);
    return ret;
#else
    return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
#endif
}

Vec3D
Vec3D::Normalize() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMVector3Normalize(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(this)));
    auto ret = Vec3D();
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
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

bool
Vec3D::operator==(const Vec3D &rhs) const
{
    return NearlyEqual(X, rhs.X) && NearlyEqual(Y, rhs.Y) &&
           NearlyEqual(Z, rhs.Z);
}

bool
Vec3D::operator!=(const Vec3D &rhs) const
{
    return !(*this == rhs);
}

Vec3D
operator+(const Vec3D &lhs, const Vec3D &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = Vec3D();
    const auto vec =
        XMVectorAdd(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
                    XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z};
#endif
}

Vec3D
operator-(const Vec3D &lhs, const Vec3D &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = Vec3D();
    const auto vec = XMVectorSubtract(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z};
#endif
}

Vec3D
operator*(const Vec3D &lhs, const float s)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = Vec3D();
    const auto vec = XMVectorScale(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)), s);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.X * s, lhs.Y * s, lhs.Z * s};
#endif
}

Vec3D
operator*(const float s, const Vec3D &rhs)
{
    return rhs * s;
}

Vec3D
operator/(const Vec3D &lhs, const float s)
{
    return lhs * (1 / s);
}

float
Vec3D::operator[](size_t i) const
{
    return operator[](i);
}

float &
Vec3D::operator[](size_t i)
{
    switch (i) {
        case 0:
            return X;
        case 1:
            return Y;
        case 2:
            return Z;
        default:
            throw std::invalid_argument(
                Utils::UtilsFormatStr("%llu is out of bounds", i));
    }
}

}  // namespace NEngine::Math
