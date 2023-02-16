#include "NEngine/Renderer/BasePass.h"

#include <d3d11.h>
#include <d3dcommon.h>

#include <memory>

#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Renderer/RasterizerState.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;
using namespace NEngine::Renderer;
using namespace NEngine::Math;
using namespace Microsoft::WRL;

void
NEngine::Renderer::BasePass::Draw(Helpers::DeviceResources &deviceResources,
                                  std::vector<NEngine::Renderer::Mesh> &meshes)
{
    deviceResources.PIXBeginEvent(L"BasePass");

    auto rtv = mRenderTarget ? mRenderTarget->GetRenderTargetView()
                             : deviceResources.GetRenderTargetView();
    auto dsv = mDepthTarget ? mDepthTarget->GetDepthStencilView()
                            : deviceResources.GetDepthStencilView();

    auto ctx = deviceResources.GetDeviceContext();
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
        const auto world = mesh.GetTransform().GetTransform();
        mPerObjectBuffer->SetValue("world", world);
        const auto invWorld = world.Inverse().Transpose();
        mPerObjectBuffer->SetValue("worldInvTranspose", invWorld);

        for (auto &meshPrimitive : mesh.GetMeshPrimitives()) {
            DrawMeshPrimitive(meshPrimitive, deviceResources);
        }
    }

    deviceResources.PIXEndEvent();
}

void
BasePass::draw(Helpers::DeviceResources &device_resources,
               const NEngine::Renderer::RenderModel &model,
               Helpers::Transform &transform)
{
    device_resources.PIXBeginEvent(L"BasePass");

    auto rtv = mRenderTarget ? mRenderTarget->GetRenderTargetView()
                             : device_resources.GetRenderTargetView();
    auto dsv = mDepthTarget ? mDepthTarget->GetDepthStencilView()
                            : device_resources.GetDepthStencilView();

    auto ctx = device_resources.GetDeviceContext();
    auto viewport = mViewport ? *mViewport : device_resources.GetViewport();
    ctx->RSSetViewports(1, &viewport);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    mRasterizerState->Bind(device_resources);

    mVertexShader->Bind(device_resources);
    mPixelShader->Bind(device_resources);
    mInputLayout->Bind(device_resources);

    UpdatePerFrameBuffer();

    mPerSceneBuffer->Bind(device_resources);
    mPerFrameBuffer->Bind(device_resources);

    for (auto &node : model.nodes) {
        draw_node(device_resources, node, transform.GetTransform());
    }

    device_resources.PIXEndEvent();
}

void
BasePass::draw_node(Helpers::DeviceResources &device_resources,
                    const NEngine::Renderer::RenderNode &node,
                    const Mat4X4 &parent_world)
{
    const auto world = parent_world * node.transform.GetTransform();
    mPerObjectBuffer->SetValue("world", world);
    const auto invWorld = world.Inverse().Transpose();
    mPerObjectBuffer->SetValue("worldInvTranspose", invWorld);

    for (auto &meshPrimitive : node.mesh.GetMeshPrimitives()) {
        DrawMeshPrimitive(meshPrimitive, device_resources);
    }

    for (const auto &child : node.children) {
        draw_node(device_resources, child, world);
    }
}

void
NEngine::Renderer::BasePass::DrawMeshPrimitive(
    const Renderer::MeshPrimitive &meshPrimitive,
    Helpers::DeviceResources &deviceResources)
{
    // Get transform (world matrix), textures and samplers and set it to per
    // object const buffer
    const Material &mat = meshPrimitive.GetMaterial();
    mPerObjectBuffer->SetValue("material.BaseColor", mat.BaseColor);
    mPerObjectBuffer->SetValue("material.Metalness", mat.Metalness);
    mPerObjectBuffer->SetValue("material.Roughness", mat.Roughness);
    mPerObjectBuffer->SetValue("material.NormalScale", mat.NormalScale);
    mPerObjectBuffer->SetValue("material.OcclusionStrength",
                               mat.OcclusionStrength);
    mPerObjectBuffer->SetValue("material.EmissiveFactor", mat.EmissiveFactor);
    mPerObjectBuffer->SetValue(
        "material.HasBaseColorTex",
        static_cast<int>(mat.BaseColorTexture.IsValid()));
    mPerObjectBuffer->SetValue(
        "material.HasMetallicRoughnessTex",
        static_cast<int>(mat.MetallicRoughnessTexture.IsValid()));
    mPerObjectBuffer->SetValue(
        "material.HasOcclusionTex",
        static_cast<int>(mat.OcclusionTexture.IsValid()));
    mPerObjectBuffer->SetValue("material.HasEmissiveTex",
                               static_cast<int>(mat.EmissiveTexture.IsValid()));
    mPerObjectBuffer->SetValue("material.HasNormalTex",
                               static_cast<int>(mat.NormalTexture.IsValid()));

    mPerObjectBuffer->Bind(deviceResources);
    meshPrimitive.Bind(deviceResources);
    deviceResources.GetDeviceContext()->DrawIndexed(
        static_cast<unsigned int>(meshPrimitive.GetIndexNum()), 0, 0);
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
        material.AddChild("OcclusionStrength", NodeType::Float);
        material.AddChild("EmissiveFactor", NodeType::Float3);
        material.AddChild("HasBaseColorTex", NodeType::Bool);
        material.AddChild("HasMetallicRoughnessTex", NodeType::Bool);
        material.AddChild("HasOcclusionTex", NodeType::Bool);
        material.AddChild("HasEmissiveTex", NodeType::Bool);
        material.AddChild("HasNormalTex", NodeType::Bool);
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
        dirLight.AddChild("Ambient", NodeType::Float4);
        dirLight.AddChild("Diffuse", NodeType::Float4);
        dirLight.AddChild("Specular", NodeType::Float4);
        dirLight.AddChild("Direction", NodeType::Float4);

        Node spotLight("spotLight", NodeType::Struct);
        spotLight.AddChild("Ambient", NodeType::Float4);
        spotLight.AddChild("Diffuse", NodeType::Float4);
        spotLight.AddChild("Specular", NodeType::Float4);
        spotLight.AddChild("Position", NodeType::Float4);
        spotLight.AddChild("Direction", NodeType::Float4);

        Node pointLight("pointLight", NodeType::Struct);
        pointLight.AddChild("Ambient", NodeType::Float4);
        pointLight.AddChild("Diffuse", NodeType::Float4);
        pointLight.AddChild("Specular", NodeType::Float4);
        pointLight.AddChild("Position", NodeType::Float4);

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

    {
        DirectionalLight dirLight;
        dirLight.Direction = {0.0, 0.5, -0.5, 20};
        dirLight.Diffuse = {1, 1, 1, 1};
        dirLight.Specular = {1, 1, 1, 1};
        mPerSceneBuffer->SetValue("dirLight", dirLight);
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
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "PBRPassPS.cso");
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