#include "NEngine/Helpers/Texture.h"

#include "NEngine/Utils/Image.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {
using namespace Utils;

Texture::Texture(DXGI_FORMAT format,
                 int width,
                 int height,
                 ID3D11Device *device)
    : mWidth(width),
      mHeight(height),
      mFormat(format) {
    CD3D11_TEXTURE2D_DESC desc(
        format,
        width,
        height,
        1,
        0,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
        D3D11_USAGE_DEFAULT,
        0,
        1,
        0);

    HR(device->CreateTexture2D(
        &desc, nullptr, mTexture.ReleaseAndGetAddressOf()))

    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR(device->CreateShaderResourceView(
            mTexture.Get(), &srvDesc, mSRV.ReleaseAndGetAddressOf()))
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    HR(device->CreateRenderTargetView(
        mTexture.Get(), &rtvDesc, mRTV.ReleaseAndGetAddressOf()))

    {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // TODO: Convert to 32 bit
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.ArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.BindFlags =
            D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

        HR(device->CreateTexture2D(
            &texDesc, nullptr, mDepthTexture.ReleaseAndGetAddressOf()))

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        HR(device->CreateDepthStencilView(
            mDepthTexture.Get(), &dsvDesc, mDSV.ReleaseAndGetAddressOf()))

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;

        HR(device->CreateShaderResourceView(
            mDepthTexture.Get(), &srvDesc, mDepthSRV.ReleaseAndGetAddressOf()))
    }
}

void
Texture::GenerateMips(ID3D11DeviceContext *context) {
    context->GenerateMips(mSRV.Get());
}

Texture::Texture(const std::string &filepath,
                 ID3D11Device *device,
                 ID3D11DeviceContext *context) {
    Image img(filepath);
    if (!img.IsValid()) {
        UtilsFormatStr("ERROR: Failed to load texture from %s\n",
                       filepath.c_str());
    }

    mWidth = static_cast<int>(img.GetWidth());
    mHeight = static_cast<int>(img.GetHeight());
    mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    CD3D11_TEXTURE2D_DESC desc(
        mFormat,
        mWidth,
        mHeight,
        1,
        0,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
        D3D11_USAGE_DEFAULT,
        0,
        1,
        0,
        D3D11_RESOURCE_MISC_GENERATE_MIPS);

    HR(device->CreateTexture2D(
        &desc, nullptr, mTexture.ReleaseAndGetAddressOf()))

    context->UpdateSubresource(
        mTexture.Get(),
        0,
        nullptr,
        img.GetBytes(),
        mWidth * 4,
        0);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;

    HR(device->CreateShaderResourceView(
        mTexture.Get(), &srvDesc, mSRV.ReleaseAndGetAddressOf()))

    context->GenerateMips(mSRV.Get());
}

Texture::Texture(const Texture &rhs) {
    mTexture = rhs.mTexture;
    mSRV = rhs.mSRV;
    mRTV = rhs.mRTV;
    mDSV = rhs.mDSV;
    mDepthSRV = rhs.mDepthSRV;
    mDepthTexture = rhs.mDepthTexture;
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mFormat = rhs.mFormat;
}

Texture &
Texture::operator=(const Texture &rhs) {
    if (this != &rhs) {
        mTexture = rhs.mTexture;
        mSRV = rhs.mSRV;
        mRTV = rhs.mRTV;
        mDSV = rhs.mDSV;
        mDepthSRV = rhs.mDepthSRV;
        mDepthTexture = rhs.mDepthTexture;
        mWidth = rhs.mWidth;
        mHeight = rhs.mHeight;
        mFormat = rhs.mFormat;
    }
    return *this;
}

Texture::Texture(DXGI_FORMAT format,
                 int width,
                 int height,
                 void *data,
                 size_t sysMemPitch,
                 size_t sysMemSlicePitch,
                 ID3D11Device *device)
    : mWidth(width),
      mHeight(height),
      mFormat(format) {
    CD3D11_TEXTURE2D_DESC desc(
        mFormat,
        mWidth,
        mHeight,
        1,
        0,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.SysMemPitch = sysMemPitch;
    initData.SysMemSlicePitch = sysMemSlicePitch;
    initData.pSysMem = data;

    HR(device->CreateTexture2D(
        &desc, &initData, mTexture.ReleaseAndGetAddressOf()))

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;

    HR(device->CreateShaderResourceView(
        mTexture.Get(), &srvDesc, mSRV.ReleaseAndGetAddressOf()))
}

}
}
