#pragma once

#include "NEngine/Math/NEMath.h"

namespace NEngine {
namespace Helpers {

using namespace Math;

struct DirectionalLight
{
    DirectionalLight()
        : Ambient{},
          Diffuse{},
          Specular{},
          Direction{}
    {
    }

    Vec4D Ambient;
    Vec4D Diffuse;
    Vec4D Specular;
    Vec4D Direction;  // W is radius
};

struct PointLight
{
    PointLight()
        : Ambient{},
          Diffuse{},
          Specular{},
          Position{}
    {
    }

    Vec4D Ambient;
    Vec4D Diffuse;
    Vec4D Specular;
    Vec4D Position;  // W - is range
};

struct SpotLight
{
    SpotLight()
        : Ambient{},
          Diffuse{},
          Specular{},
          Position{},
          Direction{}
    {
    }

    Vec4D Ambient;
    Vec4D Diffuse;
    Vec4D Specular;
    Vec4D Position;   // W - is range
    Vec4D Direction;  // W - is spot
};

struct Material
{
    Material()
        : BaseColor(),
          Metalness(0),
          Roughness(0)
    {
    }

    Vec4D BaseColor;
    float Metalness;
    float Roughness;
};

}  // namespace Helpers
}  // namespace NEngine
