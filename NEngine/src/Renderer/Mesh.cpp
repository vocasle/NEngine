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
}

const Transform &
NEngine::Renderer::Mesh::GetTransform() const
{
    return mTransform;
}

Transform &
NEngine::Renderer::Mesh::GetTransform()
{
    return mTransform;
}

void
NEngine::Renderer::Mesh::SetTransform(const Helpers::Transform &t)
{
    mTransform = t;
}


NEngine::Renderer::Mesh::Mesh(const Mesh &rhs)
    : mTransform(rhs.mTransform)
{
    mMeshPrimitives.reserve(rhs.mMeshPrimitives.size());
    for (auto &mp : rhs.mMeshPrimitives) {
        auto meshPrim = std::make_unique<MeshPrimitive>(*mp);
        mMeshPrimitives.push_back(std::move(meshPrim));
    }
}