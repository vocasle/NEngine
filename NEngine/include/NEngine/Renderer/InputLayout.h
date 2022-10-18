#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Math/Math.h"

namespace NEngine {
namespace Renderer {

struct VertexPositionOnly
{
    Math::Vec4D Position;
};

struct VertexPositionNormal
{
    Math::Vec4D Position;  // Z coordinate for U coordinate of UV coordinates
    Math::Vec4D Normal;    // Z coordinate for V coordinate of UV coordinates
};

struct VertexPositionNormalTangent
{
    VertexPositionNormalTangent()
        : Position(),
          Normal(),
          Tangent()
    {
    }
    VertexPositionNormalTangent(const Math::Vec4D &position,
                                const Math::Vec4D &normal,
                                const Math::Vec4D &tangent)
        : Position(position),
          Normal(normal),
          Tangent(tangent)
    {
    }
    Math::Vec4D Position;  // Z coordinate for U coordinate of UV coordinates
    Math::Vec4D Normal;    // Z coordinate for V coordinate of UV coordinates
    // Z coordinate is a sign of cross product between normal and tangent
    Math::Vec4D Tangent;
};

class InputLayout : public Bindable
{
public:
    InputLayout() = default;

    ~InputLayout() override = default;

    void Bind(Helpers::DeviceResources &deviceResources) override;
    void Unbind(Helpers::DeviceResources &deviceResources) override;

private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;

    template <typename Vertex>
    friend std::unique_ptr<InputLayout> CreateInputLayout(
        Helpers::DeviceResources &deviceResources,
        const std::vector<uint8_t> &vertexShaderBlob);
};

template <typename Vertex>
std::unique_ptr<InputLayout>
CreateInputLayout(Helpers::DeviceResources &deviceResources,
                  const std::vector<uint8_t> &vertexShaderBlob)
{
    return nullptr;
}

template <>
inline std::unique_ptr<InputLayout>
CreateInputLayout<VertexPositionOnly>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &vertexShaderBlob)
{
    auto il = std::make_unique<InputLayout>();

    constexpr D3D11_INPUT_ELEMENT_DESC desc[1] = {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0}};

    deviceResources.GetDevice()->CreateInputLayout(
        desc,
        _countof(desc),
        &vertexShaderBlob[0],
        vertexShaderBlob.size(),
        il->mInputLayout.ReleaseAndGetAddressOf());

    return std::move(il);
}

template <>
inline std::unique_ptr<InputLayout>
CreateInputLayout<VertexPositionNormal>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &vertexShaderBlob)
{
    auto il = std::make_unique<InputLayout>();

    const D3D11_INPUT_ELEMENT_DESC desc[2] = {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"NORMAL",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         offsetof(VertexPositionNormal, Normal),
         D3D11_INPUT_PER_VERTEX_DATA,
         0}};

    deviceResources.GetDevice()->CreateInputLayout(
        desc,
        _countof(desc),
        &vertexShaderBlob[0],
        vertexShaderBlob.size(),
        il->mInputLayout.ReleaseAndGetAddressOf());

    return std::move(il);
}

template <>
inline std::unique_ptr<InputLayout>
CreateInputLayout<VertexPositionNormalTangent>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &vertexShaderBlob)
{
    auto il = std::make_unique<InputLayout>();

    const D3D11_INPUT_ELEMENT_DESC desc[3] = {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"NORMAL",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         offsetof(VertexPositionNormalTangent, Normal),
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"TANGENT",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         offsetof(VertexPositionNormalTangent, Tangent),
         D3D11_INPUT_PER_VERTEX_DATA,
         0}};

    deviceResources.GetDevice()->CreateInputLayout(
        desc,
        _countof(desc),
        &vertexShaderBlob[0],
        vertexShaderBlob.size(),
        il->mInputLayout.ReleaseAndGetAddressOf());

    return std::move(il);
}

}  // namespace Renderer
}  // namespace NEngine
