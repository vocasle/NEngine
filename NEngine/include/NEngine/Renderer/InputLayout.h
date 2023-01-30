#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Math/NEMath.h"

namespace NEngine {
namespace Renderer {

struct PosNormTangTex
{
    PosNormTangTex() = default;
    PosNormTangTex(const Math::Vec3D &position,
                                const Math::Vec3D &normal,
                                const Math::Vec4D &tangent,
                                const Math::Vec2D &texCoords)
        : Position(position),
          Normal(normal),
          Tangent(tangent),
          TexCoords(texCoords)
    {
    }
    Math::Vec3D Position;
    Math::Vec3D Normal;
    Math::Vec4D Tangent;  // Z is a sign of cross(normal, tangent)
    Math::Vec2D TexCoords;
};

class InputLayout : public Bindable
{
public:
    InputLayout() = default;

    ~InputLayout() override = default;

    void Bind(Helpers::DeviceResources &deviceResources) const override;
    void Unbind(Helpers::DeviceResources &deviceResources) const override;

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
CreateInputLayout<PosNormTangTex>(
    Helpers::DeviceResources &deviceResources,
    const std::vector<uint8_t> &vertexShaderBlob)
{
    auto il = std::make_unique<InputLayout>();

    const D3D11_INPUT_ELEMENT_DESC desc[4] = {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"NORMAL",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         offsetof(PosNormTangTex, Normal),
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"TANGENT",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         offsetof(PosNormTangTex, Tangent),
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"TEXCOORDS",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         offsetof(PosNormTangTex, TexCoords),
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
