#include "NEngine/Renderer/Mesh.h"

#include <fstream>

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;
using namespace NEngine::Renderer;

namespace NEngine::Renderer {
const std::vector<MeshPrimitive> &
Mesh::get_mesh_primitives() const
{
    return m_primitives;
}

const std::vector<float> &
Mesh::get_weights() const
{
    return m_weights;
}

void
Mesh::set_weights(std::vector<float> weights)
{
    UTILS_ASSERT(weights.size() == m_weights.size(),
                 "Size mismatch. Weight interpolation error");
    m_weights = std::move(weights);
}

Mesh::Mesh(DeviceResources &device_resources,
           std::vector<MeshPrimitive> primitives,
           std::vector<float> weights)
    : m_primitives(std::move(primitives)),
      m_weights(std::move(weights))
{
}

}  // namespace NEngine::Renderer