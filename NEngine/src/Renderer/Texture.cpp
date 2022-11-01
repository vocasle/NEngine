#include "NEngine/Renderer/Texture.h"

#include <dxgiformat.h>

namespace NEngine::Renderer {

using namespace Microsoft::WRL;
using namespace NEngine::Utils;

D3D11_BIND_FLAG
TextureBindTargetToBindFlag(TextureBindTarget bindTarget)
{
    if (bindTarget == TextureBindTarget::ShaderResourceView) {
        return D3D11_BIND_SHADER_RESOURCE;
    }
    else if (bindTarget == TextureBindTarget::DepthStencilView) {
        return D3D11_BIND_DEPTH_STENCIL;
    }
    return D3D11_BIND_RENDER_TARGET;
}

Texture::Texture(Helpers::DeviceResources &deviceResources,
                 unsigned int bindSlot,
                 TextureBindTarget bindTarget,
                 const Utils::Image &image,
                 const std::string &name,
                 const SamplerDescription &samplerDesc)
    : mBindTarget(bindTarget),
      mBindSlot(bindSlot),
      mName(name),
      mSampler(deviceResources, samplerDesc, bindSlot)
{
    const int arraySize = 1;
    const int mipLevels = 1;
    const int cpuAccessFlags = 0;
    const int sampleCount = 1;
    const int sampleQuality = 0;
    const int miscFlags = 0;

    const CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                                     image.GetWidth(),
                                     image.GetHeight(),
                                     arraySize,
                                     mipLevels,
                                     TextureBindTargetToBindFlag(mBindTarget),
                                     D3D11_USAGE_IMMUTABLE,
                                     cpuAccessFlags,
                                     sampleCount,
                                     sampleQuality,
                                     miscFlags);

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = image.GetBytes();
    initData.SysMemPitch = image.GetChannels() * image.GetWidth();

    ComPtr<ID3D11Texture2D> texture;
    HR(deviceResources.GetDevice()->CreateTexture2D(
        &desc, &initData, texture.GetAddressOf()))
    assert(texture.Get() && "Failed to create Texture2D");

    if (mBindTarget == TextureBindTarget::ShaderResourceView) {
        CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
            texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format);
        HR(deviceResources.GetDevice()->CreateShaderResourceView(
            texture.Get(), &srvDesc, mSRV.ReleaseAndGetAddressOf()))
    }
    else {
        UTILS_FATAL_ERROR("Not implemented");
    }
}

void
Texture::Bind(Helpers::DeviceResources &deviceResources)
{
    if (mBindTarget == TextureBindTarget::ShaderResourceView) {
        deviceResources.GetDeviceContext()->VSSetShaderResources(
            mBindSlot, 1, mSRV.GetAddressOf());
        deviceResources.GetDeviceContext()->PSSetShaderResources(
            mBindSlot, 1, mSRV.GetAddressOf());

        mSampler.Bind(deviceResources);
    }
}

void
Texture::Unbind(Helpers::DeviceResources &deviceResources)
{
}
}  // namespace NEngine::Renderer