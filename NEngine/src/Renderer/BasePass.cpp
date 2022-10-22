#include "NEngine/Renderer/BasePass.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

void
NEngine::Renderer::BasePass::Draw(Helpers::DeviceResources &deviceResources,
                                  std::vector<Model> &models)
{
    mVertexShader->Bind(deviceResources);
    mPixelShader->Bind(deviceResources);
    mInputLayout->Bind(deviceResources);

    for (auto &model : models) {
        model.Draw(deviceResources);
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
}