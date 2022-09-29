#include "NEngine/Helpers/Renderer.h"
#include "NEngine/Utils/Utils.h"

#include <cassert>

namespace NEngine {
namespace Helpers {
Renderer::Renderer()
    : m_DR{nullptr} {
}

Renderer::~Renderer() {
}

void
Renderer::SetDeviceResources(DeviceResources *dr) {
    m_DR = dr;
}

void
Renderer::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) {
    m_DR->GetDeviceContext()->IASetPrimitiveTopology(topology);
}

void
Renderer::SetInputLayout(ID3D11InputLayout *inputLayout) {
    m_DR->GetDeviceContext()->IASetInputLayout(inputLayout);
}

void
Renderer::SetRasterizerState(ID3D11RasterizerState *rasterizerState) {
    m_DR->GetDeviceContext()->RSSetState(rasterizerState);
}

void
Renderer::BindPixelShader(ID3D11PixelShader *shader) {
    m_DR->GetDeviceContext()->PSSetShader(shader, nullptr, 0);
}

void
Renderer::BindVertexShader(ID3D11VertexShader *shader) {
    m_DR->GetDeviceContext()->VSSetShader(shader, nullptr, 0);
}

void
Renderer::SetSamplerState(ID3D11SamplerState *state, uint32_t slot) {
    assert(slot < R_MAX_SAMPLERS);
    m_DR->GetDeviceContext()->PSSetSamplers(slot, 1, &state);
}

void
Renderer::SetIndexBuffer(ID3D11Buffer *buffer, uint32_t startIndexLocation) {
    m_DR->GetDeviceContext()->IASetIndexBuffer(buffer,
                                               DXGI_FORMAT_R32_UINT,
                                               startIndexLocation);
}

void
Renderer::SetVertexBuffer(ID3D11Buffer *buffer,
                          uint32_t strides,
                          uint32_t offsets) {
    m_DR->GetDeviceContext()->IASetVertexBuffers(0,
                                                 1,
                                                 &buffer,
                                                 &strides,
                                                 &offsets);
}

void
Renderer::SetDepthStencilState(ID3D11DepthStencilState *depthStencilState) {
    m_DR->GetDeviceContext()->OMSetDepthStencilState(depthStencilState, 0);
}

void
Renderer::SetViewport(D3D11_VIEWPORT viewport) {
    m_DR->GetDeviceContext()->RSSetViewports(1, &viewport);
}

void
Renderer::SetRenderTargets(ID3D11RenderTargetView *rtv,
                           ID3D11DepthStencilView *dsv) {
    m_DR->GetDeviceContext()->OMSetRenderTargets(1, &rtv, dsv);
}

void
Renderer::SetRenderTargets(ID3D11RenderTargetView **rtvs,
                           uint32_t numRtvs,
                           ID3D11DepthStencilView *dsv) {
    m_DR->GetDeviceContext()->OMSetRenderTargets(numRtvs, rtvs, dsv);
}

void
Renderer::SetBlendState(ID3D11BlendState *blendState) {
    unsigned int sampleMask = 0xffffffff;
    m_DR->GetDeviceContext()->OMSetBlendState(blendState,
                                              nullptr,
                                              sampleMask);
}

void
Renderer::BindShaderResources(enum BindTargets bindTarget,
                              ID3D11ShaderResourceView **SRVs,
                              uint32_t numSRVs) {
    assert(numSRVs <= R_MAX_SRV_NUM && "numSRVs is above limit!");
    m_DR->GetDeviceContext()->PSSetShaderResources(0, numSRVs, SRVs);
}

void
Renderer::BindConstantBuffers(enum BindTargets bindTarget,
                              ID3D11Buffer **CBs,
                              uint32_t numCBs) {
    assert(numCBs <= R_MAX_CB_NUM && "numCBs is above limit!");
    if (bindTarget == BindTargets::PixelShader) {
        m_DR->GetDeviceContext()->PSSetConstantBuffers(0, numCBs, CBs);
    } else {
        m_DR->GetDeviceContext()->VSSetConstantBuffers(0, numCBs, CBs);
    }
}

void
Renderer::BindShaderResource(enum BindTargets bindTarget,
                             ID3D11ShaderResourceView *srv,
                             uint32_t slot) {
    assert(slot < R_MAX_SRV_NUM);
    m_DR->GetDeviceContext()->PSSetShaderResources(slot, 1, &srv);
}

void
Renderer::BindConstantBuffer(enum BindTargets bindTarget,
                             ID3D11Buffer *cb,
                             uint32_t slot) {
    assert(slot < R_MAX_CB_NUM);
    if (bindTarget == BindTargets::PixelShader) {
        m_DR->GetDeviceContext()->PSSetConstantBuffers(slot, 1, &cb);
    } else {
        m_DR->GetDeviceContext()->VSSetConstantBuffers(slot, 1, &cb);
    }
}

void
Renderer::DrawIndexed(uint32_t indexCount,
                      uint32_t startIndexLocation,
                      uint32_t baseVertexLocation) {
    m_DR->GetDeviceContext()->DrawIndexed(indexCount,
                                          startIndexLocation,
                                          baseVertexLocation);
}

void
Renderer::Clear(const float *color) {
    ID3D11DeviceContext *ctx = m_DR->GetDeviceContext();

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
    ctx->OMGetRenderTargets(1, rtv.GetAddressOf(), dsv.GetAddressOf());

    D3D11_VIEWPORT viewport;
    unsigned int numViewports = 1;
    ctx->RSGetViewports(&numViewports, &viewport);

    if (!rtv) {
        // UtilsDebugPrint("WARN: render target view is not set!\n");
        return;
    }
    if (!dsv) {
        // UtilsDebugPrint("WARN: depth stencil view is not set!\n");
        return;
    }

    static const float CLEAR_COLOR[4] = {0, 0, 0, 1.000000000f};

    static ID3D11ShaderResourceView *nullSRVs[R_MAX_SRV_NUM] = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
    };
    BindShaderResources(BindTargets::PixelShader, nullSRVs, R_MAX_SRV_NUM);

    ctx->ClearRenderTargetView(rtv.Get(), color ? color : CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);
    //	ctx->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
    ctx->RSSetViewports(1, &viewport);
}

void
Renderer::Present() {
    const HRESULT hr = m_DR->GetSwapChain()->Present(1, 0);

    ID3D11DeviceContext1 *ctx = reinterpret_cast<ID3D11DeviceContext1 *>(
        m_DR->GetDeviceContext());
    ctx->DiscardView((ID3D11View *)m_DR->GetRenderTargetView());
    if (m_DR->GetDepthStencilView()) {
        ctx->DiscardView((ID3D11View *)m_DR->GetDepthStencilView());
    }

    if (FAILED(hr)) {
        OutputDebugStringA("ERROR: Failed to present\n");
        ExitProcess(EXIT_FAILURE);
    }
}

void
Renderer::ClearRenderTargetView(ID3D11RenderTargetView *rtv,
                                const float *color) {
    m_DR->GetDeviceContext()->ClearRenderTargetView(rtv, color);
}

void
Renderer::ClearDepthStencilView(ID3D11DepthStencilView *dsv,
                                uint32_t mask,
                                float depth,
                                uint8_t stencil) {
    m_DR->GetDeviceContext()->ClearDepthStencilView(dsv,
                                                    mask,
                                                    depth,
                                                    stencil);
}

void
Renderer::GenerateMips(ID3D11ShaderResourceView *srv) {
    m_DR->GetDeviceContext()->GenerateMips(srv);
}

}
}
