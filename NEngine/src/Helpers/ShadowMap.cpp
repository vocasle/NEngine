#include "NEngine/Helpers/ShadowMap.h"

#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {
using namespace Utils;

ShadowMap::ShadowMap()
    : m_OutputViewPort{},
      m_cachedViewPort{} {
}

ShadowMap::~ShadowMap() {
}

void
ShadowMap::InitResources(ID3D11Device *device,
                         uint32_t texWidth,
                         uint32_t texHeight) {
    // TODO: Use logirithmic depth function instead of linear
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // TODO: Convert to 32 bit
    texDesc.Width = texWidth;
    texDesc.Height = texHeight;
    texDesc.ArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D *depthTex;
    HR(device->CreateTexture2D(&texDesc, NULL, &depthTex))

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    HR(device->CreateDepthStencilView(
        depthTex, &dsvDesc, m_pOutputTextureDSV.ReleaseAndGetAddressOf()))

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    HR(device->CreateShaderResourceView(
        depthTex, &srvDesc, m_pOutputTextureSRV.ReleaseAndGetAddressOf()))

    COM_FREE(depthTex);

    m_OutputViewPort.TopLeftX = 0.0f;
    m_OutputViewPort.TopLeftY = 0.0f;
    m_OutputViewPort.Width = (float)texWidth;
    m_OutputViewPort.Height = (float)texHeight;
    m_OutputViewPort.MinDepth = 0.0f;
    m_OutputViewPort.MaxDepth = 1.0f;

    D3D11_SAMPLER_DESC sampDesc = {};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.BorderColor[0] = {1.0f};
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HR(device->CreateSamplerState(&sampDesc,
        m_ShadowSampler.ReleaseAndGetAddressOf()))

    CreateRasterizerState(device);
}

void
ShadowMap::Bind(ID3D11DeviceContext *ctx) {
    ctx->OMGetRenderTargets(1,
                            m_cachedRTV.ReleaseAndGetAddressOf(),
                            m_cachedDSV.ReleaseAndGetAddressOf());
    uint32_t numViewports = 1;
    ctx->RSGetViewports(&numViewports, &m_cachedViewPort);

    ctx->ClearDepthStencilView(
        m_pOutputTextureDSV.Get(),
        D3D11_CLEAR_DEPTH,
        1.0f,
        0);
    ctx->OMSetRenderTargets(0, nullptr, m_pOutputTextureDSV.Get());
    ctx->RSSetViewports(1, &m_OutputViewPort);
}

void
ShadowMap::Unbind(ID3D11DeviceContext *ctx) {
    ctx->OMSetRenderTargets(0, nullptr, nullptr);
    ctx->OMSetRenderTargets(1, m_cachedRTV.GetAddressOf(), m_cachedDSV.Get());
    ctx->RSSetViewports(1, &m_cachedViewPort);

    m_cachedDSV.Reset();
    m_cachedRTV.Reset();
}

void
ShadowMap::CreateRasterizerState(ID3D11Device *device) {
    CD3D11_RASTERIZER_DESC desc{CD3D11_DEFAULT()};
    desc.DepthBias = 1000;
    desc.CullMode = D3D11_CULL_FRONT;

    HR(device->CreateRasterizerState(
        &desc, m_rasterizerState.ReleaseAndGetAddressOf()))
}

ID3D11RasterizerState *
ShadowMap::GetRasterizerState() const {
    return m_rasterizerState.Get();
}

}
}
