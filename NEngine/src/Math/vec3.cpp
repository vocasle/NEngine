#include "NEngine/Math/vec3.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/vec4.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {
std::string
vec3::ToString() const
{
    std::stringstream out;
    out << "{ " << X << ", " << Y << ", " << Z << " }";
    return out.str();
}

vec3::vec3(float x, float y, float z)
    : X{x},
      Y{y},
      Z{z}
{
}

vec3::vec3(const vec4 &rhs)
    : X(rhs.X),
      Y(rhs.Y),
      Z(rhs.Z)
{
}

vec3::vec3(float x)
    : vec3(x, x, x)
{
}

vec3::vec3()
    : vec3(0, 0, 0)
{
}

float
vec3::Length() const
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

vec3
vec3::Cross(const vec3 &lhs, const vec3 &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec =
        XMVector3Cross(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
                       XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    auto ret = vec3();
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.Y * rhs.Z - lhs.Z * rhs.Y,
            lhs.Z * rhs.X - lhs.X * rhs.Z,
            lhs.X * rhs.Y - lhs.Y * rhs.X};
#endif
}

float
vec3::Dot(const vec3 &lhs, const vec3 &rhs)
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

vec3
vec3::Normalize() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMVector3Normalize(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(this)));
    auto ret = vec3();
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

vec3
vec3::Rotate(const vec4 &quat) const
{
    auto ret = *this;
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMVector3Rotate(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(this)),
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&quat)));
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
#endif
    return ret;
}

bool
vec3::operator==(const vec3 &rhs) const
{
    return NearlyEqual(X, rhs.X) && NearlyEqual(Y, rhs.Y) &&
           NearlyEqual(Z, rhs.Z);
}

bool
vec3::operator!=(const vec3 &rhs) const
{
    return !(*this == rhs);
}

vec3
operator+(const vec3 &lhs, const vec3 &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = vec3();
    const auto vec =
        XMVectorAdd(XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
                    XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z};
#endif
}

vec3
operator-(const vec3 &lhs, const vec3 &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = vec3();
    const auto vec = XMVectorSubtract(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)),
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&rhs)));
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z};
#endif
}

vec3
operator*(const vec3 &lhs, const float s)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = vec3();
    const auto vec = XMVectorScale(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&lhs)), s);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3 *>(&ret), vec);
    return ret;
#else
    return {lhs.X * s, lhs.Y * s, lhs.Z * s};
#endif
}

vec3
operator*(const float s, const vec3 &rhs)
{
    return rhs * s;
}

vec3
operator/(const vec3 &lhs, const float s)
{
    return lhs * (1 / s);
}

float
vec3::operator[](size_t i) const
{
    return operator[](i);
}

float &
vec3::operator[](size_t i)
{
    switch (i) {
        case 0:
            return X;
        case 1:
            return Y;
        case 2:
            return Z;
        default:
            throw std::invalid_argument("Index out of bounds");
    }
}

}  // namespace NEngine::Math
