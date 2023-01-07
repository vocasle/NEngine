#pragma once
#include <string>
namespace NEngine::Math {
struct Vec2D
{
    Vec2D()
        : X{0},
          Y{0}
    {
    }

    Vec2D(float x, float y)
        : X{x},
          Y{y}
    {
    }
    std::string ToString() const;
    float Length() const;

    float X;
    float Y;
};

bool operator==(const Vec2D &lhs, const Vec2D &rhs);
}  // namespace NEngine::Math