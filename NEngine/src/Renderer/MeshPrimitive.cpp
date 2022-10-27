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
            deviceResources, vertices);
    mBinds.push_back(std::move(vb));
}

void
NEngine::Renderer::MeshPrimitive::SetBaseColorFactor(
    const Math::Vec4D &baseColorFactor)
{
    mBaseColorFactor = baseColorFactor;
}

void
NEngine::Renderer::MeshPrimitive::SetMetallicFactor(float metallicFactor)
{
    mMetallicFactor = metallicFactor;
}