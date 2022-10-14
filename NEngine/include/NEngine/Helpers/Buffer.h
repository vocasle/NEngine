#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {
struct BufferDescription {
    BufferDescription(D3D11_USAGE usage,
                      unsigned int bindFlags,
                      unsigned int cpuAccessFlags = 0,
                      unsigned int miscFlags = 0)
        : usage(usage),
          bindFlags(bindFlags),
          cpuAccessFlags(cpuAccessFlags),
          miscFlags(miscFlags) {
    }

    D3D11_USAGE usage;
    unsigned int bindFlags;
    unsigned int cpuAccessFlags;
    unsigned int miscFlags;
};

template <typename T>
class Buffer {
public:
    Buffer() = default;

    Buffer(std::vector<typename T> data, const BufferDescription &description)
        : m_data(data),
          m_description(description),
          m_bufferSize(sizeof(T) * data.size()) {
    }

    void
    Create(ID3D11Device *device) {
        using namespace Utils;
        const CD3D11_BUFFER_DESC desc(m_bufferSize,
                                      m_description.bindFlags,
                                      m_description.usage,
                                      m_description.cpuAccessFlags,
                                      m_description.miscFlags,
                                      sizeof(T));

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = &m_data[0];
        HR(device->CreateBuffer(&desc, &initData, m_buffer.
            ReleaseAndGetAddressOf()))
    }

    void Update(const T *data, size_t count);

    size_t
    Count() const {
        return m_data.size();
    }

    ID3D11Buffer *
    GetBuffer() const {
        return m_buffer.Get();
    }

    size_t
    GetValueSize() const {
        return sizeof(T);
    }

private:
    std::vector<typename T> m_data;
    BufferDescription m_description;
    size_t m_bufferSize;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};
}
}
