#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <string>
#include <vector>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Utils/Image.h"
#include "Sampler.h"

namespace NEngine::Renderer {

enum class TextureBindTarget {
    ShaderResourceView,
    DepthStencilView,
    RenderTargetView,
    RenderTargetAndDepthStencilView
};

class Texture : public Bindable
{
public:
    Texture(Helpers::DeviceResources &deviceResources,
            unsigned int bindSlot,
            TextureBindTarget bindTarget,
            unsigned int width,
            unsigned int height,
            const std::string &name);
    Texture(Helpers::DeviceResources &deviceResources,
            unsigned int bindSlot,
            TextureBindTarget bindTarget,
            const Utils::Image &image,
            const std::string &name,
            const SamplerDescription &samplerDesc = SamplerDescription());

    virtual void Bind(Helpers::DeviceResources &deviceResources) override;
    virtual void Unbind(Helpers::DeviceResources &deviceResources) override;
    ID3D11ShaderResourceView *GetShaderResourceView() const;
    ID3D11RenderTargetView *GetRenderTargetView() const;
    ID3D11DepthStencilView *GetDepthStencilView() const;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;

    TextureBindTarget mBindTarget;
    unsigned int mBindSlot;
    std::string mName;
    Sampler mSampler;
};
}  // namespace NEngine::Renderer
