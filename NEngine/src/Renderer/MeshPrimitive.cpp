#include "NEngine/Renderer/MeshPrimitive.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/VertexBuffer.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

NEngine::Renderer::MeshPrimitive::MeshPrimitive(
    Helpers::DeviceResources &deviceResources,
    std::vector<VertexPositionNormalTangent> vertices,
    std::vector<unsigned int> indices)
    : mVertices(std::move(vertices)),
      mIndices(std::move(indices))
{
    auto indexBuffer = std::make_unique<IndexBuffer>(deviceResources, mIndices);
    mIndexBuffer = indexBuffer.get();
    mBinds.push_back(std::move(indexBuffer));

    std::unique_ptr<VertexBuffer<VertexPositionNormalTangent>> vb =
        std::make_unique<VertexBuffer<VertexPositionNormalTangent>>(
            deviceResources, mVertices);
    mBinds.push_back(std::move(vb));
}

void
NEngine::Renderer::MeshPrimitive::SetMaterial(Material material)
{
    mMaterial = std::move(material);
}

void
NEngine::Renderer::MeshPrimitive::Bind(
    Helpers::DeviceResources &deviceResources) const
{
    for (auto &bind : mBinds) {
        bind->Bind(deviceResources);
    }
}
size_t
NEngine::Renderer::MeshPrimitive::GetIndexNum() const
{
    return mIndices.size();
}