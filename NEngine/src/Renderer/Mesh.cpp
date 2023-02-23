#include "NEngine/Renderer/Mesh.h"

#include <fstream>

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;
using namespace NEngine::Renderer;

namespace NEngine::Renderer {
const std::vector<MeshPrimitive> &
Mesh::GetMeshPrimitives() const
{
    return m_primitives;
}

const std::vector<float> &
Mesh::GetWeights() const
{
    return m_weights;
}

Mesh::Mesh(DeviceResources &device_resources,
           std::vector<MeshPrimitive> primitives,
           std::vector<float> weights)
    : m_primitives(std::move(primitives)),
      m_weights(std::move(weights))
{
}

}  // namespace NEngine::Renderer