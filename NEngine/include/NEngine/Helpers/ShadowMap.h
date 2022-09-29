#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

namespace NEngine {
namespace Helpers {
class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();

    void InitResources(ID3D11Device *device,
                       uint32_t texWidth,
                       uint32_t texHeight);

    void Bind(ID3D11DeviceContext *ctx);
    void Unbind(ID3D11DeviceContext *ctx);

    ID3D11ShaderResourceView *
    GetDepthMapSRV() const {
        return m_pOutputTextureSRV.Get();
    }

    ID3D11SamplerState *
    GetShadowSampler() const {
        return m_ShadowSampler.Get();
    }

    ID3D11RasterizerState *GetRasterizerState() const;

private:
    void CreateRasterizerState(ID3D11Device *device);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pOutputTextureSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pOutputTextureDSV;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_ShadowSampler;
    D3D11_VIEWPORT m_OutputViewPort;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_cachedRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_cachedDSV;
    D3D11_VIEWPORT m_cachedViewPort;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
};
}
}
