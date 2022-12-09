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

    float X;
    float Y;
};
}  // namespace NEngine::Math