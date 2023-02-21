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

Mesh::Mesh(DeviceResources &deviceResources, std::vector<MeshPrimitive> meshes)
    : mMeshPrimitives(std::move(meshes))
{
}

}  // namespace NEngine::Renderer