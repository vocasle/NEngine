#include "NEngine/Renderer/Texture.h"

#include <d3d11.h>
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
                 unsigned int width,
                 unsigned int height,
                 const std::string &name)
    : mBindTarget(bindTarget),
      mBindSlot(bindSlot),
      mName(name),
      mSampler(deviceResources, SamplerDescription(), bindSlot)
{
    auto texture = CreateTexture2D(deviceResources, width, height);
    assert(texture.Get() && "Failed to create Texture2D");

    if (mBindTarget == TextureBindTarget::RenderTargetView) {
        CreateRenderTargetView(deviceResources, texture.Get());
    }
    else if (mBindTarget == TextureBindTarget::DepthStencilView) {
        CreateDepthStencilView(deviceResources, texture.Get());
    }
    CreateShaderResourceView(deviceResources, texture.Get());
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
    const int mipLevels = 0;
    const int cpuAccessFlags = 0;
    const int sampleCount = 1;
    const int sampleQuality = 0;
    const int miscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    const CD3D11_TEXTURE2D_DESC desc(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        image.GetWidth(),
        image.GetHeight(),
        arraySize,
        mipLevels,
        TextureBindTargetToBindFlag(mBindTarget) | D3D11_BIND_RENDER_TARGET,
        D3D11_USAGE_DEFAULT,
        cpuAccessFlags,
        sampleCount,
        sampleQuality,
        miscFlags);

    ComPtr<ID3D11Texture2D> texture;
    HR(deviceResources.GetDevice()->CreateTexture2D(
        &desc, nullptr, texture.GetAddressOf()))
    assert(texture.Get() && "Failed to create Texture2D");

    if (mBindTarget == TextureBindTarget::ShaderResourceView) {
        CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
            texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format, 0, -1);
        HR(deviceResources.GetDevice()->CreateShaderResourceView(
            texture.Get(), &srvDesc, mSRV.ReleaseAndGetAddressOf()))

        deviceResources.GetDeviceContext()->UpdateSubresource(
            texture.Get(),
            0,
            nullptr,
            image.GetBytes(),
            image.GetChannels() * image.GetWidth(),
            image.GetChannels() * image.GetWidth() * image.GetHeight());

        deviceResources.GetDeviceContext()->GenerateMips(mSRV.Get());
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

ID3D11ShaderResourceView *
Texture::GetShaderResourceView() const
{
    return mSRV.Get();
}
ID3D11RenderTargetView *
Texture::GetRenderTargetView() const
{
    return mRTV.Get();
}
ID3D11DepthStencilView *
Texture::GetDepthStencilView() const
{
    return mDSV.Get();
}

void
Texture::Resize(Helpers::DeviceResources &deviceResources,
                DirectX::XMFLOAT2 size)
{
    auto texture = CreateTexture2D(deviceResources, size.x, size.y);
    if (mBindTarget == TextureBindTarget::DepthStencilView) {
        CreateDepthStencilView(deviceResources, texture.Get());
    }
    else if (mBindTarget == TextureBindTarget::RenderTargetView) {
        CreateRenderTargetView(deviceResources, texture.Get());
    }
    CreateShaderResourceView(deviceResources, texture.Get());
}

ComPtr<ID3D11Texture2D>
Texture::CreateTexture2D(Helpers::DeviceResources &deviceResources,
                         float width,
                         float height)
{
    const int arraySize = 1;
    const int mipLevels = 0;
    const int cpuAccessFlags = 0;
    const int sampleCount = 1;
    const int sampleQuality = 0;
    const int miscFlags = mBindTarget == TextureBindTarget::ShaderResourceView
                              ? D3D11_RESOURCE_MISC_GENERATE_MIPS
                              : 0;

    DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    if (mBindTarget == TextureBindTarget::DepthStencilView) {
        dxgiFormat = DXGI_FORMAT_R32_TYPELESS;
    }

    int bindFlags =
        TextureBindTargetToBindFlag(mBindTarget) | D3D11_BIND_SHADER_RESOURCE;

    const CD3D11_TEXTURE2D_DESC desc(dxgiFormat,
                                     width,
                                     height,
                                     arraySize,
                                     mipLevels,
                                     bindFlags,
                                     D3D11_USAGE_DEFAULT,
                                     cpuAccessFlags,
                                     sampleCount,
                                     sampleQuality,
                                     miscFlags);

    ComPtr<ID3D11Texture2D> texture;
    HR(deviceResources.GetDevice()->CreateTexture2D(
        &desc, nullptr, texture.GetAddressOf()))
    assert(texture.Get() && "Failed to create Texture2D");
    return texture;
}

void
Texture::CreateDepthStencilView(Helpers::DeviceResources &deviceResources,
                                ID3D11Texture2D *texture)
{
    CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(
        texture, D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
    HR(deviceResources.GetDevice()->CreateDepthStencilView(
        texture, &dsvDesc, mDSV.ReleaseAndGetAddressOf()))
}

void
Texture::CreateRenderTargetView(Helpers::DeviceResources &deviceResources,
                                ID3D11Texture2D *texture)
{
    CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(
        texture, D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);

    HR(deviceResources.GetDevice()->CreateRenderTargetView(
        texture, &rtvDesc, mRTV.ReleaseAndGetAddressOf()))
}

void
Texture::CreateShaderResourceView(Helpers::DeviceResources &deviceResources,
                                  ID3D11Texture2D *texture)
{
    CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(
        texture,
        D3D11_SRV_DIMENSION_TEXTURE2D,
        mBindTarget == TextureBindTarget::DepthStencilView
            ? DXGI_FORMAT_R32_FLOAT
            : DXGI_FORMAT_R8G8B8A8_UNORM,
        0,
        -1);
    HR(deviceResources.GetDevice()->CreateShaderResourceView(
        texture, &srvDesc, mSRV.ReleaseAndGetAddressOf()))
}

}  // namespace NEngine::Renderer