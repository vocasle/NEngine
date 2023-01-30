#include "NEngine/Renderer/Mesh.h"

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

const Transform &
Mesh::GetTransform() const
{
    return mTransform;
}

Transform &
Mesh::GetTransform()
{
    return mTransform;
}

void
Mesh::SetTransform(const Helpers::Transform &t)
{
    mTransform = t;
}

Mesh::Mesh(const Mesh &rhs)
    : mTransform(rhs.mTransform),
      mMeshPrimitives(std::move(rhs.mMeshPrimitives))
{
}

}  // namespace NEngine::Renderer