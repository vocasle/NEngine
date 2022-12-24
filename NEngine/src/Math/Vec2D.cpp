#include "NEngine/Math/Vec2D.h"

#include <sstream>

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
}  // namespace NEngine::Math