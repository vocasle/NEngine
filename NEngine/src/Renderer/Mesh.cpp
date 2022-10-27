#include "NEngine/Renderer/Mesh.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

const std::vector<std::unique_ptr<NEngine::Renderer::MeshPrimitive>> &
NEngine::Renderer::Mesh::GetMeshPrimitives() const
{
    return mMeshPrimitives;
}

NEngine::Renderer::Mesh::Mesh(
    DeviceResources &deviceResources,
    std::vector<std::unique_ptr<Renderer::MeshPrimitive>> meshes)
    : mMeshPrimitives(std::move(meshes))
{
    // for (auto &mesh : meshes) {
    //     mMeshes.push_back(std::move(mesh));
    // }
}
