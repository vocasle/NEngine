#pragma once

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Math/Math.h"

#include <wrl/client.h>
#include <d3d11.h>

#include <vector>


namespace NEngine {
namespace Renderer {

struct VertexPositionOnly
{
    Math::Vec4D Position;
};

struct VertexPositionNormal
{
    Math::Vec4D Position; // Z coordinate for U coordinate of UV coordinates
    Math::Vec4D Normal;   // Z coordinate for V coordinate of UV coordinates
};

struct VertexPositionNormalTangent
{
    Math::Vec4D Position; // Z coordinate for U coordinate of UV coordinates
    Math::Vec4D Normal;   // Z coordinate for V coordinate of UV coordinates
    // Z coordinate is a sign of cross product between normal and tangent
    Math::Vec4D Tangent;
};

template <typename Vertex>
void
CreateInputLayout(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &data,
    ID3D11InputLayout **pInputLayout)
{
}

class InputLayout : public Bindable
{
public:
    template <typename Vertex>
    InputLayout(Helpers::DeviceResources &deviceResources,
                const std::vector<uint8_t> &data)
    {
        CreateInputLayout<Vertex>(deviceResources,
                                  data,
                                  mInputLayout.ReleaseAndGetAddressOf());
    }

    ~InputLayout() override = default;

    void Bind(Helpers::DeviceResources &deviceResources) override;
    void Unbind(Helpers::DeviceResources &deviceResources) override;

private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
};

template <>
inline void
CreateInputLayout<VertexPositionOnly>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &data,
    ID3D11InputLayout **pInputLayout)
{
    constexpr D3D11_INPUT_ELEMENT_DESC desc[1] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    deviceResources.GetDevice()->CreateInputLayout(
        desc,
        _countof(desc),
        &data[0],
        data.size(),
        pInputLayout);
}


template <>
inline void
CreateInputLayout<VertexPositionNormal>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &data,
    ID3D11InputLayout **pInputLayout)
{
    const D3D11_INPUT_ELEMENT_DESC desc[2] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
         offsetof(VertexPositionNormal, Normal),
         D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    deviceResources.GetDevice()->CreateInputLayout(
        desc,
        _countof(desc),
        &data[0],
        data.size(),
        pInputLayout);
}

template <>
inline void
CreateInputLayout<VertexPositionNormalTangent>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &data,
    ID3D11InputLayout **pInputLayout)
{
    const D3D11_INPUT_ELEMENT_DESC desc[3] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
         offsetof(VertexPositionNormalTangent, Normal),
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
         offsetof(VertexPositionNormalTangent, Tangent),
         D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    deviceResources.GetDevice()->CreateInputLayout(
        desc,
        _countof(desc),
        &data[0],
        data.size(),
        pInputLayout);
}

}
}
