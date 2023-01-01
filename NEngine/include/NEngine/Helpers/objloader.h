#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace NEngine {
namespace Helpers {
struct Face
{
    Face()
        : posIdx{0},
          texIdx{0},
          normIdx{0}
    {
    }

    Face(uint32_t pIdx, uint32_t tIdx, uint32_t nIdx)
        : posIdx{pIdx},
          texIdx{tIdx},
          normIdx{nIdx}
    {
    }

    uint32_t posIdx;
    uint32_t texIdx;
    uint32_t normIdx;
};

struct Mesh
{
    std::string Name;
    std::vector<glm::vec3> Positions;
    std::vector<glm::vec2> TexCoords;
    std::vector<glm::vec3> Normals;
    std::vector<Face> Faces;
    std::vector<unsigned int> Indices;
};

struct Model
{
    std::vector<Mesh> Meshes;
    std::string Directory;
};

std::unique_ptr<Model> OLLoad(const char *filename);
void OLDumpModelToFile(const Model *model, const char *filename);

}  // namespace Helpers
}  // namespace NEngine
