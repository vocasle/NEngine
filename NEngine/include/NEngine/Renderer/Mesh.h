#pragma once
#include <vector>

#include "Mesh.h"
#include "NEngine/Renderer/MeshPrimitive.h"

namespace NEngine {
namespace Renderer {

class Mesh
{
public:
    Mesh() = default;
    Mesh(Helpers::DeviceResources &deviceResources,
         std::vector<MeshPrimitive> meshes);

    const std::vector<MeshPrimitive> &GetMeshPrimitives() const;

private:
    std::vector<MeshPrimitive> mMeshPrimitives;

    friend class FileWriter;
};
}  // namespace Renderer
}  // namespace NEngine
