#include "NEngine/Renderer/IndexBuffer.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

NEngine::Renderer::IndexBuffer::IndexBuffer(
    Helpers::DeviceResources &deviceResources,
    const std::vector<unsigned> &indexData)
    : mIndexData(indexData)
{
    const CD3D11_BUFFER_DESC desc(sizeof(unsigned int) * mIndexData.size(),
                                  D3D11_BIND_INDEX_BUFFER);

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &mIndexData[0];

    HR(deviceResources.GetDevice()->CreateBuffer(&desc, &data, mIndexBuffer.
        ReleaseAndGetAddressOf()))
}

void
NEngine::Renderer::IndexBuffer::Bind(Helpers::DeviceResources &deviceResources)
{
    deviceResources.GetDeviceContext()->IASetIndexBuffer(
        mIndexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0);
}

void
NEngine::Renderer::IndexBuffer::Unbind(
    Helpers::DeviceResources &deviceResources)
{
}

size_t
NEngine::Renderer::IndexBuffer::GetIndexCount() const
{
    return mIndexData.size();
}
