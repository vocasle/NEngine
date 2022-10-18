#include "NEngine/Renderer/Mesh.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/VertexBuffer.h"
#include "NEngine/Utils/Utils.h"

#include <cstdlib>

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

NEngine::Renderer::Mesh::Mesh(Helpers::DeviceResources &deviceResources,
                              const std::vector<VertexPositionNormalTangent> &vertices, const std::vector<unsigned int> &indices)
    : mVertices(vertices),
      mIndices(indices)
{
    mIndexBuffer = std::make_unique<IndexBuffer>(deviceResources, mIndices);
    std::unique_ptr<VertexBuffer<VertexPositionNormalTangent>> vb =
        std::make_unique<VertexBuffer<VertexPositionNormalTangent>>(
            deviceResources,
            vertices);

    mBinds.push_back(std::move(vb));
    
    char compileCommand[] = "\"" FXCOMP_PATH "\" " // Path to FXC
                            "/Od /Zi /T vs_5_1 "   // Shader Arguments
                            "/Fo"
                            "\"" ENGINE_ROOT "/x64/Debug/ColorVS.cso\" "         // Output file
                            "\"" ENGINE_ROOT "/Sandbox/shader/ColorVS.hlsl\"";  // Input file
    std::system( compileCommand );

    auto binaryBlob = UtilsReadData( ENGINE_ROOT "/x64/Debug/ColorVS.cso" );

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
