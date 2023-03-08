#include "NEngine/Math/vec2.h"

#include <sstream>

#include "NEngine/Math/MathUtils.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {
float
vec2::Length() const
{
#if NENGINE_USE_DIRECTXMATH
    auto len = 0.0f;
    const auto vec =
        XMVector2Length(XMLoadFloat2(reinterpret_cast<const XMFLOAT2 *>(this)));
    XMStoreFloat(&len, vec);
    return len;
#else
    return sqrtf(X * X + Y * Y);
#endif
}

std::string
vec2::ToString() const
{
    std::stringstream out;
    out << "{ " << X << ", " << Y << " }";
    return out.str();
}
bool
operator==(const vec2 &lhs, const vec2 &rhs)
{
    return NearlyEqual(lhs.X, rhs.X) && NearlyEqual(lhs.Y, rhs.Y);
}
}  // namespace NEngine::Math