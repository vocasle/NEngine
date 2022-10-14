#pragma once

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"

#include <wrl/client.h>
#include <d3d11.h>

#include <vector>

namespace NEngine {
namespace Renderer {
class IndexBuffer : public Bindable
{
public:
    IndexBuffer(Helpers::DeviceResources &deviceResources,
                const std::vector<unsigned int> &indexData);
    ~IndexBuffer() override = default;

    void Bind(Helpers::DeviceResources &deviceResources) override;
    void Unbind(Helpers::DeviceResources &deviceResources) override;

    [[nodiscard]] size_t GetIndexCount() const;

private:
    std::vector<unsigned int> mIndexData;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
};
}
}
