#include "NEngine/Math/Vec4D.h"

#include <sstream>

#include "NEngine/Utils/Utils.h"

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

bool
Vec4D::operator==(const Vec4D &rhs) const
{
    return X == rhs.X && Y == rhs.Y && Z == rhs.Z && W == rhs.W;
}

Vec4D
operator+(const Vec4D &lhs, const Vec4D &rhs)
{
    UTILS_ASSERT(lhs.W == rhs.W, "Cannot mix vector and point");
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z, lhs.W};
}

Vec4D
operator-(const Vec4D &lhs, const Vec4D &rhs)
{
    UTILS_ASSERT(lhs.W == rhs.W, "Cannot mix vector and point");
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z, lhs.W};
}

Vec4D
operator*(const Vec4D &lhs, const float s)
{
    return {lhs.X * s, lhs.Y * s, lhs.Z * s, lhs.W};
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