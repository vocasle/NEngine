#pragma once
#include <string>

#include "Vec3D.h"


namespace NEngine::Math {
struct Vec4D
{
    Vec4D()
        : X{0},
          Y{0},
          Z{0},
          W{0}
    {
    }

    Vec4D(float x, float y, float z, float w)
        : X{x},
          Y{y},
          Z{z},
          W{w}
    {
    }
    Vec4D(const Vec3D &v, float w)
        : Vec4D(v.X, v.Y, v.Z, w)
    {
    }

    Vec4D(const Vec4D &rhs) = default;
    Vec4D(Vec4D &&rhs) noexcept = default;
    Vec4D &operator=(const Vec4D &rhs) = default;
    Vec4D &operator=(Vec4D &&rhs) noexcept = default;

    std::string ToString() const;
    Vec4D &operator*(const float scalar);
    Vec4D &operator/(const float scalar);
    bool operator==(const Vec4D &rhs) const;

    
    float X;
    float Y;
    float Z;
    float W;
};
}  // namespace NEngine::Math