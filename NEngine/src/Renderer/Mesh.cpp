#include "NEngine/Renderer/Mesh.h"

using namespace NEngine::Helpers;

NEngine::Renderer::Mesh::Mesh(Helpers::DeviceResources &deviceResources)
{
}

void
NEngine::Renderer::Mesh::Draw(Helpers::DeviceResources &deviceResources)
{
    for (auto &bind : mBinds) {
        bind->Bind(deviceResources);
    }

    deviceResources.GetDeviceContext()->DrawIndexed(3, 0, 0);
}
