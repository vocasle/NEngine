#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <string>
#include <vector>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Utils/Image.h"

namespace NEngine::Renderer {

enum class TextureBindTarget {
    ShaderResourceView,
    DepthStencilView,
    RenderTargetView,
};

class Texture : public Bindable
{
public:
    Texture(Helpers::DeviceResources &deviceResources,
            unsigned int bindSlot,
            TextureBindTarget bindTarget,
            const Utils::Image &image);

    virtual void Bind(Helpers::DeviceResources &deviceResources) override;
    virtual void Unbind(Helpers::DeviceResources &deviceResources) override;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;

    TextureBindTarget mBindTarget;
    unsigned int mBindSlot;
};
}  // namespace NEngine::Renderer
