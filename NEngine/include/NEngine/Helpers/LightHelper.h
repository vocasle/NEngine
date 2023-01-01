#pragma once

#include "glm/vec4.hpp"

namespace NEngine {
namespace Helpers {

struct DirectionalLight
{
    DirectionalLight()
        : Ambient{},
          Diffuse{},
          Specular{},
          Direction{}
    {
    }

    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    glm::vec4 Direction;  // W is radius
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

    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    glm::vec4 Position;  // W - is range
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

    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    glm::vec4 Position;   // W - is range
    glm::vec4 Direction;  // W - is spot
};

struct Material
{
    Material()
        : BaseColor(),
          Metalness(0),
          Roughness(0)
    {
    }

    glm::vec4 BaseColor;
    float Metalness;
    float Roughness;
};

}  // namespace Helpers
}  // namespace NEngine
