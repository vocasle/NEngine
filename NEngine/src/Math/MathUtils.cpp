#include "NEngine/Math/MathUtils.h"

#include "NEngine/Math/Vec3D.h"

namespace NEngine::Math {
float
ToRadians(float degrees)
{
    return degrees * (PI / 180);
}
float
ToDegrees(float radians)
{
    return radians * (180 / PI);
}

bool
NearlyEqual(float lhs, float rhs)
{
    return Abs(lhs - rhs) < EPSILON;
}

bool
NearlyEqual(const Vec3D &lhs, const Vec3D &rhs)
{
    const auto lhsLen = lhs.Length();
    const auto rhsLen = rhs.Length();
    const auto isLenEqual = NearlyEqual(lhsLen, rhsLen);
    const auto dot = Vec3D::Dot(lhs, rhs);
    return isLenEqual && NearlyEqual(lhsLen, dot) && NearlyEqual(rhsLen, dot);
    // const auto cosPhi = dot / (lhsLen * rhsLen);
    // return isLenEqual && NearlyEqual(1, cosPhi);
}

float
ClampToZero(float n)
{
    return NearlyEqual(n, 0) ? 0 : n;
}

float
Abs(float n)
{
    return n < 0 ? -n : n;
}
}  // namespace NEngine::Math