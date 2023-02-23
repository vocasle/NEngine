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
    Mesh(Helpers::DeviceResources &device_resources,
         std::vector<MeshPrimitive> primitives,
         std::vector<float> weights);

    [[nodiscard]] const std::vector<MeshPrimitive> &GetMeshPrimitives() const;
    [[nodiscard]] const std::vector<float> &GetWeights() const;

private:
    std::vector<MeshPrimitive> m_primitives;
    std::vector<float> m_weights;
};
}  // namespace Renderer
}  // namespace NEngine
