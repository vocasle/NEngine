#include "NEngine/Helpers/DynamicConstBuffer.h"

#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace NEngine {
namespace Helpers {

std::string
NodeTypeToString(NodeType inType)
{
    switch (inType) {
        case NodeType::Struct:
            return "Struct";
        case NodeType::Array:
            return "Array";
        case NodeType::Float:
            return "Float";
        case NodeType::Float2:
            return "Float2";
        case NodeType::Float3:
            return "Float3";
        case NodeType::Float4:
            return "Float4";
        case NodeType::Float3X3:
            return "Float3X3";
        case NodeType::Float4X4:
            return "Float4X4";
        default:
            return "None";
    }
}

void
DynamicConstBuffer::UpdateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};

    if (FAILED(mDeviceResources->GetDeviceContext()->Map(
            mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        UtilsFatalError("ERROR: Failed to map constant buffer\n");
    }
    memcpy(mapped.pData, mBytes.data(), mBytes.size());
    mDeviceResources->GetDeviceContext()->Unmap(mBuffer.Get(), 0);
}

void
DynamicConstBuffer::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.ByteWidth = mBytes.size();
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = mBytes.data();

    if (FAILED(mDeviceResources->GetDevice()->CreateBuffer(
            &bufferDesc, &initData, mBuffer.ReleaseAndGetAddressOf()))) {
        UtilsFatalError(
            "ERROR: Failed to create per frame constants cbuffer\n");
    }
}

[[nodiscard]] const std::vector<uint8_t> &
DynamicConstBuffer::GetBytes() const
{
    return mBytes;
}

ID3D11Buffer *
DynamicConstBuffer::Get() const
{
    return mBuffer.Get();
}

DynamicConstBuffer::DynamicConstBuffer(const DynamicConstBufferDesc &desc,
                                       DeviceResources &deviceResources)
    : mDeviceResources(&deviceResources),
      mBindSlot(desc.GetBindSlot())
{
    size_t sz = 0;
    for (auto &node : desc.GetNodes()) {
        auto visitor = [&sz](const Node &node) -> void
        {
            if (node.Type != NodeType::Array && node.Type != NodeType::Struct) {
                sz += static_cast<size_t>(node.Type);
            }
        };
        node.Visit(visitor);
    }
    if (sz % 16 != 0) {
        UtilsDebugPrint(
            "WARN: Invalid alignment for cbuffer. Adding padding\n");
        sz = static_cast<size_t>(std::ceil(sz / 16.0f) * 16);
    }
    mBytes.resize(sz);

    size_t offset = 0;
    for (auto &node : desc.GetNodes()) {
        auto visitor = [this, &offset](const Node &node) -> void
        {
            if (node.Type != NodeType::Struct && node.Type != NodeType::Array) {
                mValues[node.Name] = {node.Type, &mBytes[offset]};
                offset += static_cast<size_t>(node.Type);
            }
            else {
                mValues[node.Name] = {node.Type, &mBytes[offset]};
            }
        };
        node.Visit(visitor);
    }

    InitializeDefaults();
    CreateConstantBuffer();
    UpdateConstantBuffer();
}

void
DynamicConstBuffer::Bind(Helpers::DeviceResources &deviceResources)
{
    UpdateConstantBuffer();
    deviceResources.GetDeviceContext()->VSSetConstantBuffers(
        mBindSlot, 1, mBuffer.GetAddressOf());
    deviceResources.GetDeviceContext()->PSSetConstantBuffers(
        mBindSlot, 1, mBuffer.GetAddressOf());
}

void
DynamicConstBuffer::Unbind(Helpers::DeviceResources &deviceResources)
{
}

void
DynamicConstBuffer::InitializeDefaults()
{
    static_assert(
        sizeof glm::vec2 == static_cast<unsigned int>(NodeType::Float2),
        "Size mismatch!");
    static_assert(
        sizeof glm::vec3 == static_cast<unsigned int>(NodeType::Float3),
        "Size mismatch!");
    static_assert(
        sizeof glm::vec4 == static_cast<unsigned int>(NodeType::Float4),
        "Size mismatch!");
    static_assert(
        sizeof glm::mat3x3 == static_cast<unsigned int>(NodeType::Float3X3),
        "Size mismatch!");
    static_assert(
        sizeof glm::mat4x4 == static_cast<unsigned int>(NodeType::Float4X4),
        "Size mismatch!");

    for (const auto &[k, v] : mValues) {
        switch (v.Type) {
            case NodeType::Struct:
            case NodeType::Array:
                break;
            case NodeType::Bool:
                *reinterpret_cast<bool *>(v.Ptr) = false;
                break;
            case NodeType::Float2:
                *reinterpret_cast<glm::vec2 *>(v.Ptr) = {0, 0};
                break;
            case NodeType::Float3:
                *reinterpret_cast<glm::vec3 *>(v.Ptr) = {0, 0, 0};
                break;
            case NodeType::Float4:
                *reinterpret_cast<glm::vec4 *>(v.Ptr) = {0, 0, 0, 0};
                break;
            case NodeType::Float3X3:
                *reinterpret_cast<glm::mat3x3 *>(v.Ptr) = glm::mat3x3(1);
                break;
            case NodeType::Float4X4:
                *reinterpret_cast<glm::mat4x4 *>(v.Ptr) = glm::mat4x4(1);
                break;
            case NodeType::None:
                break;
        }
    }
}

}  // namespace Helpers
}  // namespace NEngine
