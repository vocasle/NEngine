//
// Created by vocasle on 10/19/2022.
//

#include "NEngine/Renderer/VertexShader.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

void
NEngine::Renderer::VertexShader::Bind(
    NEngine::Helpers::DeviceResources &deviceResources)
{
	deviceResources.GetDeviceContext()->VSSetShader(mVertexShader.Get(), nullptr, 0);
}
void
NEngine::Renderer::VertexShader::Unbind(
    NEngine::Helpers::DeviceResources &deviceResources)
{
}

NEngine::Renderer::VertexShader::VertexShader(
    NEngine::Helpers::DeviceResources &deviceResources, const std::string &path)
{
	const auto shaderBlob = UtilsReadData(path.c_str());
	HR(deviceResources.GetDevice()->CreateVertexShader(&shaderBlob[0], shaderBlob.size(), nullptr, mVertexShader.ReleaseAndGetAddressOf()))
}
