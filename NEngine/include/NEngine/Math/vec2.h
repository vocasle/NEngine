#pragma once
#include <string>
namespace NEngine::Math {
struct vec2
{
    vec2()
        : X{0},
          Y{0}
    {
    }

    vec2(float x, float y)
        : X{x},
          Y{y}
    {
    }
    std::string ToString() const;
    float Length() const;

    float X;
    float Y;
};

bool operator==(const vec2 &lhs, const vec2 &rhs);
}  // namespace NEngine::Math