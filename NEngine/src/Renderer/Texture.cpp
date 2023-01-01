#include "NEngine/Renderer/Texture.h"

#include <d3d11.h>
#include <dxgiformat.h>

#include "NEngine/Utils/Utils.h"

namespace NEngine::Renderer {

using namespace Microsoft::WRL;
using namespace NEngine::Utils;

Texture::Texture(Helpers::DeviceResources &deviceResources,
                 unsigned int width,
                 unsigned int height,
                 TextureBindTarget bindTarget,
                 const std::string &name)
    : mBindTarget(bindTarget),
      mName(name),
      mSampler(deviceResources, {}, 0)
{
    D3D11_TEXTURE2D_DESC texDesc;
    memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 0;
    texDesc.ArraySize = 1;
    texDesc.Format = mBindTarget == TextureBindTarget::DepthStencilView
                         ? DXGI_FORMAT_R32_TYPELESS
                         : DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = mBindTarget == TextureBindTarget::DepthStencilView
                            ? D3D11_BIND_DEPTH_STENCIL
                            : D3D11_BIND_RENDER_TARGET;
    texDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags = 0;
    texDesc.CPUAccessFlags = 0;

    HR(deviceResources.GetDevice()->CreateTexture2D(
        &texDesc, nullptr, mTex.ReleaseAndGetAddressOf()))

    if (mBindTarget == TextureBindTarget::DepthStencilView) {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc;
        memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        desc.Format = DXGI_FORMAT_D32_FLOAT;
        desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;

        HR(deviceResources.GetDevice()->CreateDepthStencilView(
            mTex.Get(), &desc, mDSV.ReleaseAndGetAddressOf()))
    }
    else if (mBindTarget == TextureBindTarget::RenderTargetView) {
        D3D11_RENDER_TARGET_VIEW_DESC desc;
        memset(&desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipSlice = 0;

        HR(deviceResources.GetDevice()->CreateRenderTargetView(
            mTex.Get(), &desc, mRTV.ReleaseAndGetAddressOf()))
    }
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        memset(&desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        desc.Format = mBindTarget == TextureBindTarget::DepthStencilView
                          ? DXGI_FORMAT_R32_FLOAT
                          : DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = 1;
        desc.Texture2D.MostDetailedMip = 0;

        HR(deviceResources.GetDevice()->CreateShaderResourceView(
            mTex.Get(), &desc, mSRV.ReleaseAndGetAddressOf()))
    }
}

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
                const glm::vec2 &size)
{
    auto tmp = Texture(deviceResources, size.x, size.y, mBindTarget, mName);
    *this = std::move(tmp);
}

}  // namespace NEngine::Renderer