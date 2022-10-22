#include "NEngine/Renderer/Mesh.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/VertexBuffer.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

NEngine::Renderer::Mesh::Mesh(Helpers::DeviceResources &deviceResources,
                              const std::vector<VertexPositionNormalTangent> &vertices, const std::vector<unsigned int> &indices)
    : mVertices(vertices),
      mIndices(indices)
{
    auto indexBuffer = std::make_unique<IndexBuffer>(deviceResources, mIndices);
    mIndexBuffer = indexBuffer.get();
    mBinds.push_back(std::move(indexBuffer));

    std::unique_ptr<VertexBuffer<VertexPositionNormalTangent>> vb =
        std::make_unique<VertexBuffer<VertexPositionNormalTangent>>(
            deviceResources,
            vertices);
    mBinds.push_back(std::move(vb));
}

void
NEngine::Renderer::Mesh::Draw(Helpers::DeviceResources &deviceResources)
{
    for (auto &bind : mBinds) {
        bind->Bind(deviceResources);
    }

    deviceResources.GetDeviceContext()->DrawIndexed(
        mIndexBuffer->GetIndexCount(),
        0,
        0);
}
