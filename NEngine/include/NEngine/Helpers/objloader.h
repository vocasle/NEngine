#pragma once

#include "NEngine/Math/Math.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace NEngine {
namespace Helpers {
struct Face {
    Face()
        : posIdx{0},
          texIdx{0},
          normIdx{0} {
    }

    Face(uint32_t pIdx, uint32_t tIdx, uint32_t nIdx)
        : posIdx{pIdx},
          texIdx{tIdx},
          normIdx{nIdx} {
    }

    uint32_t posIdx;
    uint32_t texIdx;
    uint32_t normIdx;
};

struct Mesh {
    std::string Name;
    std::vector<Math::Vec3D> Positions;
    std::vector<Math::Vec2D> TexCoords;
    std::vector<Math::Vec3D> Normals;
    std::vector<Face> Faces;
};

struct Model {
    std::vector<Mesh> Meshes;
    std::string Directory;
};

std::unique_ptr<Model> OLLoad(const char *filename);
void OLDumpModelToFile(const Model *model, const char *filename);

}
}
