#include "NEngine/Math/Vec2D.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"

namespace NEngine::Math {
float
Vec2D::Length() const
{
    return sqrtf(X * X + Y * Y);
}

std::string
Vec2D::ToString() const
{
    std::stringstream out;
    out << "{ " << X << ", " << Y << " }";
    return out.str();
}
bool
operator==(const Vec2D &lhs, const Vec2D &rhs)
{
    return NearlyEqual(lhs.X, rhs.X) && NearlyEqual(lhs.Y, rhs.Y);
}
}  // namespace NEngine::Math