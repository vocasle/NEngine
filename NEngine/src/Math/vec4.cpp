#include "NEngine/Math/vec4.h"

#include <sstream>

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {
vec4::vec4()
    : X{0},
      Y{0},
      Z{0},
      W{0}
{
}

vec4::vec4(float x, float y, float z, float w)
    : X{x},
      Y{y},
      Z{z},
      W{w}
{
}
vec4::vec4(const vec3 &v, float w)
    : vec4(v.X, v.Y, v.Z, w)
{
}

std::string
vec4::ToString() const
{
    std::stringstream out;
    out << "{ " << X << ", " << Y << ", " << Z << ", " << W << " }";
    return out.str();
}

float
vec4::Length() const
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
vec4::operator==(const vec4 &rhs) const
{
    return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
}

float
vec4::operator[](size_t i) const
{
    return operator[](i);
}

float &
vec4::operator[](size_t i)
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
            throw std::invalid_argument("Index out of bounds");
    }
}

vec3
vec4::ToVec3D() const
{
    return vec3(X, Y, Z);
}

vec4
vec4::Normalize() const
{
#if NENGINE_USE_DIRECTXMATH
    const auto vec = XMVector4Normalize(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(this)));
    auto ret = vec4();
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

vec4
operator+(const vec4 &lhs, const vec4 &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = vec4();
    const auto vec =
        XMVectorAdd(XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&lhs)),
                    XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&rhs)));
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W + rhs.W};
#endif
}

vec4
operator-(const vec4 &lhs, const vec4 &rhs)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = vec4();
    const auto vec = XMVectorSubtract(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&lhs)),
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&rhs)));
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W - rhs.W};
#endif
}

vec4
operator*(const vec4 &lhs, const float s)
{
#if NENGINE_USE_DIRECTXMATH
    auto ret = vec4();
    const auto vec = XMVectorScale(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&lhs)), s);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), vec);
    return ret;
#else
    return {lhs.X * s, lhs.Y * s, lhs.Z * s, lhs.W};
#endif
}
vec4
operator*(const float s, const vec4 &rhs)
{
    return rhs * s;
}
vec4
operator/(const vec4 &lhs, const float s)
{
    return lhs * (1 / s);
}
vec4
operator/(const float s, const vec4 &rhs)
{
    return rhs / s;
}
}  // namespace NEngine::Math