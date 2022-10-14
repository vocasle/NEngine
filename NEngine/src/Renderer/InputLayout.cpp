#include "NEngine/Renderer/InputLayout.h"

void
NEngine::Renderer::InputLayout::Bind(Helpers::DeviceResources &deviceResources)
{
	deviceResources.GetDeviceContext()->IASetInputLayout(mInputLayout.Get());
}

void
NEngine::Renderer::InputLayout::Unbind(
    Helpers::DeviceResources &deviceResources)
{
}
