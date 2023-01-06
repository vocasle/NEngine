#include "NEngine/Math/MathUtils.h"

#include "NEngine/Math/Vec3D.h"

namespace NEngine::Math {
float
ToRadians(float degrees)
{
    return degrees * (PI / 180.0);
}
float
ToDegrees(float radians)
{
    return radians * (180.0 / PI);
}

bool
NearlyEqual(float lhs, float rhs)
{
    return Abs(lhs - rhs) < EPSILON;
}

bool
NearlyEqual(const Vec3D &lhs, const Vec3D &rhs)
{
    return NearlyEqual(lhs.X, rhs.X) && NearlyEqual(lhs.Y, rhs.Y) &&
           NearlyEqual(lhs.Z, rhs.Z);
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
