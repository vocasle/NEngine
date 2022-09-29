#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

namespace NEngine {
namespace Helpers {
class InputLayout {
public:
    InputLayout()
        : m_strides(0) {
    }

    InputLayout(ID3D11Device *device, const void *vsBytes, const size_t sz);
    InputLayout(const InputLayout &rhs);
    InputLayout(InputLayout &&rhs) noexcept;
    InputLayout &operator=(const InputLayout &rhs);
    InputLayout &operator=(InputLayout &&rhs) noexcept;
    ~InputLayout();

    ID3D11InputLayout *
    Get() const {
        return m_inputLayout.Get();
    }

    size_t
    GetStrides() const {
        return m_strides;
    }

    const std::vector<D3D11_INPUT_ELEMENT_DESC> &
    GetInputDescriptions() const {
        return m_inputDescriptions;
    }

private:
    HRESULT CreateInputLayoutDescFromVertexShaderSignature(
        const void *shaderData,
        const size_t sz,
        ID3D11Device *pD3DDevice);

private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputDescriptions;
    size_t m_strides;
};
}
}
