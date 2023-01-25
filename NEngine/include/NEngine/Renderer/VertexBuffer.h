#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Renderer {
template <typename Vertex>
class VertexBuffer : public Bindable
{
public:
    VertexBuffer(Helpers::DeviceResources &deviceResources,
                 const std::vector<Vertex> &vertexData);
    ~VertexBuffer() override = default;

    void Bind(Helpers::DeviceResources &deviceResources) override;
    void Unbind(Helpers::DeviceResources &deviceResources) override;

private:
    std::vector<Vertex> mVertexData;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
};

template <typename Vertex>
VertexBuffer<Vertex>::VertexBuffer(Helpers::DeviceResources &deviceResources,
                                   const std::vector<Vertex> &vertexData)
    : mVertexData(vertexData)
{
    const CD3D11_BUFFER_DESC desc(
        static_cast<unsigned int>(sizeof(Vertex) * mVertexData.size()),
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_USAGE_DEFAULT,
        0,
        0,
        static_cast<unsigned int>(sizeof(Vertex)));

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &mVertexData[0];

    using namespace Utils;

    HR(deviceResources.GetDevice()->CreateBuffer(
        &desc, &data, mVertexBuffer.ReleaseAndGetAddressOf()))
}

template <typename Vertex>
void
VertexBuffer<Vertex>::Bind(Helpers::DeviceResources &deviceResources)
{
    const unsigned int strides = sizeof(Vertex);
    constexpr unsigned int offsets = 0;
    deviceResources.GetDeviceContext()->IASetVertexBuffers(
        0, 1, mVertexBuffer.GetAddressOf(), &strides, &offsets);
}

template <typename Vertex>
void
VertexBuffer<Vertex>::Unbind(Helpers::DeviceResources &deviceResources)
{
}
}  // namespace Renderer
}  // namespace NEngine
