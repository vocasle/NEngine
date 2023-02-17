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
    return mMeshPrimitives;
}

const std::vector<Animation> &
Mesh::GetAnimations() const
{
    return m_animations;
}

std::vector<Animation> &
Mesh::GetAnimations()
{
    return m_animations;
}

Mesh::Mesh(DeviceResources &deviceResources,
           std::vector<MeshPrimitive> meshes,
           std::vector<Animation> animations)
    : mMeshPrimitives(std::move(meshes)),
      m_animations(std::move(animations))
{
}

}  // namespace NEngine::Renderer