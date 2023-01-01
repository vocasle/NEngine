#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <memory>
#include <vector>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace NEngine {
namespace Renderer {

struct PosNormTangTex
{
    PosNormTangTex() = default;
    PosNormTangTex(const glm::vec3 &position,
                   const glm::vec3 &normal,
                   const glm::vec4 &tangent,
                   const glm::vec2 &texCoords)
        : Position(position),
          Normal(normal),
          Tangent(tangent),
          TexCoords(texCoords)
    {
    }
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec4 Tangent;  // Z is a sign of cross(normal, tangent)
    glm::vec2 TexCoords;
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
CreateInputLayout<PosNormTangTex>(Helpers::DeviceResources &deviceResources,
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
