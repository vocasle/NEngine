#include "NEngine/Renderer/PixelShader.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

NEngine::Renderer::PixelShader::PixelShader(
    Helpers::DeviceResources &deviceResources, const std::string &path)
{
    const auto shaderBlob = UtilsReadData(path.c_str());
    HR(deviceResources.GetDevice()->CreatePixelShader(
        &shaderBlob[0],
        shaderBlob.size(),
        nullptr,
        mPixelShader.ReleaseAndGetAddressOf()))
}

void
NEngine::Renderer::PixelShader::Bind(Helpers::DeviceResources &deviceResources)
{
    deviceResources.GetDeviceContext()->PSSetShader(
        mPixelShader.Get(), nullptr, 0);
}

void
NEngine::Renderer::PixelShader::Unbind(
    Helpers::DeviceResources &deviceResources)
{
}
