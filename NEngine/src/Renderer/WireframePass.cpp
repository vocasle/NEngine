#include "NEngine/Renderer/WireframePass.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

namespace NEngine::Renderer {
WireframePass::WireframePass(Helpers::DeviceResources &deviceResources)
    : BasePass(deviceResources)
{
    const auto path =
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "BasePassVS.cso");
    auto binaryBlob = UtilsReadData(path.c_str());
    mInputLayout =
        CreateInputLayout<PosNormTangTex>(deviceResources, binaryBlob);
    mVertexShader = std::make_unique<VertexShader>(
        deviceResources,
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "BasePassVS.cso"));
    mPixelShader = std::make_unique<PixelShader>(
        deviceResources,
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "WireframePS.cso"));
    mRasterizerState->SetFillMode(FillMode::WireFrame);
}
void
WireframePass::Draw(
    Helpers::DeviceResources &deviceResources,
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &meshes)
{
    mRasterizerState->Bind(deviceResources);
    mVertexShader->Bind(deviceResources);
    mPixelShader->Bind(deviceResources);
    mInputLayout->Bind(deviceResources);
    deviceResources.GetDeviceContext()->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UpdatePerFrameBuffer();

    mPerSceneBuffer->Bind(deviceResources);
    mPerFrameBuffer->Bind(deviceResources);

    for (auto &mesh : meshes) {
        const auto world = mesh->GetTransform().GetWorld();
        mPerObjectBuffer->SetValue("world", world);
        auto invWorld = MathMat4X4Inverse(world);
        MathMat4X4Transpose(&invWorld);
        mPerObjectBuffer->SetValue("worldInvTranspose", invWorld);

        for (auto &meshPrimitive : mesh->GetMeshPrimitives()) {
            DrawMeshPrimitive(meshPrimitive.get(), deviceResources);
        }
    }
}
}  // namespace NEngine::Renderer