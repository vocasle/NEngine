#include "NEngine/Renderer/Mesh.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/VertexBuffer.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

NEngine::Renderer::Mesh::Mesh(Helpers::DeviceResources &deviceResources,
                              std::vector<VertexPositionNormalTangent> vertices,
                              std::vector<unsigned int> indices)
    : mVertices(std::move(vertices)),
      mIndices(std::move(indices))
{
    mIndexBuffer = std::make_unique<IndexBuffer>(deviceResources, indices);
    std::unique_ptr<VertexBuffer<VertexPositionNormalTangent>> vb =
        std::make_unique<VertexBuffer<VertexPositionNormalTangent>>(
            deviceResources,
            vertices);

    mBinds.push_back(std::move(vb));

    auto binaryBlob = UtilsReadData(
        R"(C:\Users\vocasle\source\repos\NEngine\x64\Debug\ColorVS.cso)");

    auto il = CreateInputLayout<VertexPositionNormalTangent>(
        deviceResources,
        binaryBlob);

    mBinds.push_back(std::move(il));
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
