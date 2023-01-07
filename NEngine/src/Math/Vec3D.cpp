#include "NEngine/Math/Vec3D.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"
#include "NEngine/Utils/Utils.h"

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

Vec3D::Vec3D()
    : Vec3D(0, 0, 0)
{
}

float
Vec3D::Length() const
{
    return sqrtf(X * X + Y * Y + Z * Z);
}

Vec3D
Vec3D::Cross(const Vec3D &lhs, const Vec3D &rhs)
{
    return {lhs.Y * rhs.Z - lhs.Z * rhs.Y,
            lhs.Z * rhs.X - lhs.X * rhs.Z,
            lhs.X * rhs.Y - lhs.Y * rhs.X};
}

float
Vec3D::Dot(const Vec3D &lhs, const Vec3D &rhs)
{
    return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
}

Vec3D
Vec3D::Normalize() const
{
    auto ret = *this;
    const auto length = Length();
    if (length != 0) {
        ret = ret / length;
    }
    return ret;
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
    return {lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z};
}

Vec3D
operator-(const Vec3D &lhs, const Vec3D &rhs)
{
    return {lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z};
}

Vec3D
operator*(const Vec3D &lhs, const float s)
{
    return {lhs.X * s, lhs.Y * s, lhs.Z * s};
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
    UTILS_ASSERT(i < 3, "Invalid index");
    if (i == 0)
        return X;
    else if (i == 1)
        return Y;
    else
        return Z;
}
float &
Vec3D::operator[](size_t i)
{
    UTILS_ASSERT(i < 3, "Invalid index");
    if (i == 0)
        return X;
    else if (i == 1)
        return Y;
    else
        return Z;
}

}  // namespace NEngine::Math
