#include "NEngine/Helpers/InputLayout.h"
#include "NEngine/Utils/Utils.h"

#include <d3d11shader.h>
#include <d3dcompiler.h>

namespace NEngine {
namespace Helpers {

using namespace Utils;

HRESULT
InputLayout::CreateInputLayoutDescFromVertexShaderSignature(
    const void *shaderData,
    const size_t sz,
    ID3D11Device *pD3DDevice) {
    // Reflect shader info
    ID3D11ShaderReflection *pVertexShaderReflection = nullptr;
    if (FAILED(D3DReflect(
        shaderData, sz, IID_ID3D11ShaderReflection,
        reinterpret_cast<void **>(&pVertexShaderReflection)))) {
        return S_FALSE;
    }

    // Get shader info
    D3D11_SHADER_DESC shaderDesc;
    pVertexShaderReflection->GetDesc(&shaderDesc);

    // Read input layout description from shader info
    for (uint32_t i = 0; i < shaderDesc.InputParameters; i++) {
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
        pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

        // fill out input element desc
        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = paramDesc.SemanticName;
        elementDesc.SemanticIndex = paramDesc.SemanticIndex;
        elementDesc.InputSlot = 0;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;

        // determine DXGI format
        if (paramDesc.Mask == 1) {
            if (paramDesc.ComponentType ==
                D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32_UINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32_SINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
        } else if (paramDesc.Mask <= 3) {
            if (paramDesc.ComponentType ==
                D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        } else if (paramDesc.Mask <= 7) {
            if (paramDesc.ComponentType ==
                D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format =
                    DXGI_FORMAT_R32G32B32_FLOAT;
        } else if (paramDesc.Mask <= 15) {
            if (paramDesc.ComponentType ==
                D3D_REGISTER_COMPONENT_UINT32)
                elementDesc.Format =
                    DXGI_FORMAT_R32G32B32A32_UINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_SINT32)
                elementDesc.Format =
                    DXGI_FORMAT_R32G32B32A32_SINT;
            else if (paramDesc.ComponentType ==
                     D3D_REGISTER_COMPONENT_FLOAT32)
                elementDesc.Format =
                    DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        // save element desc
        m_inputDescriptions.push_back(elementDesc);
    }

    // Try to create Input Layout
    const HRESULT hr = pD3DDevice->CreateInputLayout(
        &m_inputDescriptions[0],
        static_cast<uint32_t>(m_inputDescriptions.size()),
        shaderData,
        sz,
        m_inputLayout.ReleaseAndGetAddressOf());

    for (const D3D11_INPUT_ELEMENT_DESC &desc : m_inputDescriptions) {
        switch (desc.Format) {
            case DXGI_FORMAT_R32_FLOAT:
                m_strides += 4;
                break;
            case DXGI_FORMAT_R32G32_FLOAT:
                m_strides += 8;
                break;
            case DXGI_FORMAT_R32G32B32_FLOAT:
                m_strides += 12;
                break;
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                m_strides += 16;
                break;
            default:
                UtilsDebugPrint(
                    "WARN: DXGI_FORMAT(%d) is not handled properly!\n",
                    desc.Format);
        }
    }

    // Free allocation shader reflection memory
    pVertexShaderReflection->Release();
    return hr;
}

InputLayout::InputLayout(ID3D11Device *device,
                         const void *vsBytes,
                         const size_t sz)
    : m_strides(0) {
    HR(CreateInputLayoutDescFromVertexShaderSignature(
        vsBytes, sz, device))
}

InputLayout::InputLayout(const InputLayout &rhs) {
    m_inputLayout = rhs.m_inputLayout;
    m_strides = rhs.m_strides;
    m_inputDescriptions = rhs.m_inputDescriptions;
}

InputLayout::InputLayout(InputLayout &&rhs) noexcept {
    std::swap(m_inputLayout, rhs.m_inputLayout);
    std::swap(m_strides, rhs.m_strides);
    std::swap(m_inputDescriptions, rhs.m_inputDescriptions);
}

InputLayout &
InputLayout::operator=(const InputLayout &rhs) {
    if (this != &rhs) {
        m_inputLayout = rhs.m_inputLayout;
        m_strides = rhs.m_strides;
        m_inputDescriptions = rhs.m_inputDescriptions;
    }
    return *this;
}

InputLayout &
InputLayout::operator=(InputLayout &&rhs) noexcept {
    if (this != &rhs) {
        std::swap(m_inputLayout, rhs.m_inputLayout);
        std::swap(m_strides, rhs.m_strides);
        std::swap(m_inputDescriptions, rhs.m_inputDescriptions);
    }
    return *this;
}

InputLayout::~InputLayout() {
}

}
}
