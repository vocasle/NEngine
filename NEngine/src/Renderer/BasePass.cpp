#include "NEngine/Renderer/BasePass.h"

#include <d3d11.h>
#include <d3dcommon.h>

#include <memory>

#include "NEngine/Renderer/RasterizerState.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;
using namespace NEngine::Renderer;
using namespace NEngine::Math;
using namespace Microsoft::WRL;

void
NEngine::Renderer::BasePass::Draw(
    Helpers::DeviceResources &deviceResources,
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &meshes)
{
    deviceResources.PIXBeginEvent(L"BasePass");

    // cache original views
    D3D11_VIEWPORT *originalViewport = nullptr;
    auto numViewports = 0u;
    ComPtr<ID3D11RenderTargetView> originalRtv;
    ComPtr<ID3D11DepthStencilView> originalDsv;
    deviceResources.GetDeviceContext()->OMGetRenderTargets(
        1, originalRtv.GetAddressOf(), originalDsv.GetAddressOf());
    deviceResources.GetDeviceContext()->RSGetViewports(&numViewports,
                                                       originalViewport);

    auto rtv = mRenderTarget ? mRenderTarget->GetRenderTargetView()
                             : deviceResources.GetRenderTargetView();
    auto dsv = mDepthTarget ? mDepthTarget->GetDepthStencilView()
                            : deviceResources.GetDepthStencilView();
    auto ctx = deviceResources.GetDeviceContext();
    constexpr float clearColor[] = {0.3f, 0.3f, 0.3f, 1.0f};
    ctx->ClearRenderTargetView(rtv, clearColor);
    ctx->ClearDepthStencilView(
        dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    auto viewport = mViewport ? *mViewport : deviceResources.GetViewport();
    ctx->RSSetViewports(1, &viewport);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mRasterizerState->Bind(deviceResources);

    mVertexShader->Bind(deviceResources);
    mPixelShader->Bind(deviceResources);
    mInputLayout->Bind(deviceResources);

    UpdatePerFrameBuffer();

    mPerSceneBuffer->Bind(deviceResources);
    mPerFrameBuffer->Bind(deviceResources);

    for (auto &mesh : meshes) {
        const auto world = mesh->GetTransform().GetTransform();
        mPerObjectBuffer->SetValue("world", world);
        auto invWorld = MathMat4X4Inverse(world);
        MathMat4X4Transpose(&invWorld);
        mPerObjectBuffer->SetValue("worldInvTranspose", invWorld);

        for (auto &meshPrimitive : mesh->GetMeshPrimitives()) {
            DrawMeshPrimitive(meshPrimitive.get(), deviceResources);
        }
    }

    if (originalRtv && originalDsv) {
        ctx->OMSetRenderTargets(
            1, originalRtv.GetAddressOf(), originalDsv.Get());
    }
    if (originalViewport) {
        ctx->RSSetViewports(numViewports, originalViewport);
    }

    deviceResources.PIXEndEvent();
}

void
NEngine::Renderer::BasePass::DrawMeshPrimitive(
    const Renderer::MeshPrimitive *meshPrimitive,
    Helpers::DeviceResources &deviceResources)
{
    // Get transform (world matrix), textures and samplers and set it to per
    // object const buffer
    const Material &mat = meshPrimitive->GetMaterial();
    mPerObjectBuffer->SetValue("material.BaseColor", mat.BaseColor);
    mPerObjectBuffer->SetValue("material.Metalness", mat.Metalness);
    mPerObjectBuffer->SetValue("material.Roughness", mat.Roughness);
    mPerObjectBuffer->SetValue("material.NormalScale", mat.NormalScale);

    mPerObjectBuffer->Bind(deviceResources);
    meshPrimitive->Bind(deviceResources);
    deviceResources.GetDeviceContext()->DrawIndexed(
        meshPrimitive->GetIndexNum(), 0, 0);
}

void
NEngine::Renderer::BasePass::SetCamera(const Helpers::Camera &camera)
{
    mCamera = &camera;
}

void
NEngine::Renderer::BasePass::UpdatePerFrameBuffer()
{
    assert(mCamera != nullptr && "Camera is not set to BasePass!");
    const auto viewMat = mCamera->GetViewMat();
    const auto projMat = mCamera->GetProjMat();
    const auto camPos = mCamera->GetPos();
    mPerFrameBuffer->SetValue("view", viewMat);
    mPerFrameBuffer->SetValue("proj", projMat);
    mPerFrameBuffer->SetValue("cameraPosW", camPos);
}

NEngine::Renderer::BasePass::BasePass(Helpers::DeviceResources &deviceResources)
    : mDeviceResources(&deviceResources)
{
    const auto path =
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "BasePassVS.cso");
    auto binaryBlob = UtilsReadData(path.c_str());
    mInputLayout =
        CreateInputLayout<PosNormTangTex>(deviceResources, binaryBlob);

    ReloadShaders();

    {
        DynamicConstBufferDesc desc;
        desc.AddNode(Node("worldInvTranspose", NodeType::Float4X4));
        desc.AddNode(Node("world", NodeType::Float4X4));
        desc.AddNode(Node("shadowTransform", NodeType::Float4X4));
        Node material("material", NodeType::Struct);
        material.AddChild("BaseColor", NodeType::Float4);
        material.AddChild("Metalness", NodeType::Float);
        material.AddChild("Roughness", NodeType::Float);
        material.AddChild("NormalScale", NodeType::Float);
        desc.AddNode(material);
        desc.SetBindSlot(0);

        mPerObjectBuffer =
            std::make_unique<DynamicConstBuffer>(desc, deviceResources);
    }

    {
        DynamicConstBufferDesc desc;
        desc.AddNode(Node("view", NodeType::Float4X4));
        desc.AddNode(Node("proj", NodeType::Float4X4));
        desc.AddNode(Node("cameraPosW", NodeType::Float3));
        desc.SetBindSlot(1);

        mPerFrameBuffer =
            std::make_unique<DynamicConstBuffer>(desc, deviceResources);
    }

    {
        DynamicConstBufferDesc desc;
        Node dirLight("dirLight", NodeType::Struct);
        dirLight.AddChild("Ambient", NodeType::Float);
        dirLight.AddChild("Diffuse", NodeType::Float);
        dirLight.AddChild("Specular", NodeType::Float);
        dirLight.AddChild("Position", NodeType::Float);

        Node spotLight("spotLight", NodeType::Struct);
        spotLight.AddChild("Ambient", NodeType::Float);
        spotLight.AddChild("Diffuse", NodeType::Float);
        spotLight.AddChild("Specular", NodeType::Float);
        spotLight.AddChild("Position", NodeType::Float);
        spotLight.AddChild("Direction", NodeType::Float);

        Node pointLight("pointLight", NodeType::Struct);
        pointLight.AddChild("Ambient", NodeType::Float);
        pointLight.AddChild("Diffuse", NodeType::Float);
        pointLight.AddChild("Specular", NodeType::Float);
        pointLight.AddChild("Position", NodeType::Float);

        Node spotLightArr("spotLights", NodeType::Array);
        spotLightArr.AddChildN(spotLight, 2);

        Node pointLightArr("pointLights", NodeType::Array);
        pointLightArr.AddChildN(pointLight, 6);

        desc.AddNode(dirLight);
        desc.AddNode(pointLightArr);
        desc.AddNode(spotLightArr);
        desc.SetBindSlot(2);

        mPerSceneBuffer =
            std::make_unique<DynamicConstBuffer>(desc, deviceResources);
    }

    {
        RasterizerDescription desc;
        desc.FrontCounterClockwise = true;
        mRasterizerState =
            std::make_unique<RasterizerState>(deviceResources, desc);
    }
}

void
NEngine::Renderer::BasePass::ReloadShaders()
{
    const auto path =
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "BasePassVS.cso");

    // TODO: Get shader blob out of constructor
    mVertexShader = std::make_unique<VertexShader>(*mDeviceResources, path);

    const auto pixelPath =
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "BasePassPS.cso");
    mPixelShader = std::make_unique<PixelShader>(*mDeviceResources, pixelPath);
}

void
NEngine::Renderer::BasePass::SetRenderTarget(Texture &renderTarget)
{
    mRenderTarget = &renderTarget;
}

void
NEngine::Renderer::BasePass::SetDepthTarget(Texture &depthTarget)
{
    mDepthTarget = &depthTarget;
}

void
NEngine::Renderer::BasePass::SetViewport(D3D11_VIEWPORT &viewport)
{
    mViewport = &viewport;
}