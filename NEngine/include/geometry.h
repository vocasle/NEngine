#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "misc.h"

namespace NEngine {
namespace Geometry {
struct vertex;

struct Mesh
{
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

};

struct Model
{
    std::vector<Mesh> meshes;
};

void CreatePlane(std::vector<vertex>   &vertices,
                 std::vector<uint32_t> &indices,
                 float                  width,
                 float                  height,
                 const glm::vec3       &center,
                 Axis                   axis);

void CreatePlane(std::vector<vertex>   &vertices,
                 std::vector<uint32_t> &indices,
                 float                  width,
                 float                  height);
}  // namespace Geometry
}  // namespace NEngine