#include "NEngine/Renderer/BasePass.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;

void
NEngine::Renderer::BasePass::Draw(Helpers::DeviceResources &deviceResources,
                                  std::vector<Model *> &models)
{
    mVertexShader->Bind(deviceResources);
    mPixelShader->Bind(deviceResources);
    mInputLayout->Bind(deviceResources);

    for (auto &model : models) {
        model->Draw(deviceResources);
    }
}

NEngine::Renderer::BasePass::BasePass(Helpers::DeviceResources &deviceResources)
{
    const auto path =
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "ColorVS.cso");
    auto binaryBlob = UtilsReadData(path.c_str());
    mInputLayout = CreateInputLayout<VertexPositionNormalTangent>(
        deviceResources, binaryBlob);

    // TODO: Get shader blob out of constructor
    mVertexShader = std::make_unique<VertexShader>(deviceResources, path);

    const auto pixelPath =
        UtilsFormatStr("%s/%s", NENGINE_SHADER_BUILD_DIR, "PhongPS.cso");
    mPixelShader = std::make_unique<PixelShader>(deviceResources, pixelPath);

    {
        DynamicConstBufferDesc desc;
        desc.AddNode(Node("worldInvTranspose", NodeType::Float4X4));
        desc.AddNode(Node("world", NodeType::Float4X4));
        desc.AddNode(Node("shadowTransform", NodeType::Float4X4));
        Node material("material", NodeType::Struct);
        material.AddChild("roughness", NodeType::Float);
        desc.AddNode(material);

        mPerObjectBuffer = std::make_unique<DynamicConstBuffer>(desc, deviceResources);
    }

    {
        DynamicConstBufferDesc desc;
        desc.AddNode(Node("view", NodeType::Float4X4));
        desc.AddNode(Node("proj", NodeType::Float4X4));
        desc.AddNode(Node("cameraPosW", NodeType::Float3));

        mPerFrameBuffer = std::make_unique<DynamicConstBuffer>(desc, deviceResources);
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
        
        mPerSceneBuffer = std::make_unique<DynamicConstBuffer>(desc, deviceResources);
    }
}