#pragma once

#include <cstdint>
#include <d3d11.h>

#include "DeviceResources.h"

namespace NEngine {
namespace Helpers {
#define R_MAX_SRV_NUM 7
#define R_MAX_CB_NUM 4
#define R_MAX_SAMPLERS 2

#define R_DEFAULT_PRIMTIVE_TOPOLOGY D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

enum class BindTargets { PixelShader, VertexShader };

class Renderer {
public:
    Renderer();
    ~Renderer();

    void SetDeviceResources(DeviceResources *dr);
    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
    void SetInputLayout(ID3D11InputLayout *inputLayout);
    void SetRasterizerState(ID3D11RasterizerState *rasterizerState);
    void SetSamplerState(ID3D11SamplerState *state, uint32_t slot);
    void SetIndexBuffer(ID3D11Buffer *buffer, uint32_t startIndexLocation);
    void SetVertexBuffer(ID3D11Buffer *buffer,
                         uint32_t strides,
                         uint32_t offsets);
    void SetDepthStencilState(ID3D11DepthStencilState *depthStencilState);
    void SetViewport(D3D11_VIEWPORT viewport);
    void SetRenderTargets(ID3D11RenderTargetView *rtv,
                          ID3D11DepthStencilView *dsv);
    void SetRenderTargets(ID3D11RenderTargetView **rtvs,
                          uint32_t numRtvs,
                          ID3D11DepthStencilView *dsv);
    void SetBlendState(ID3D11BlendState *blendState);

    void BindPixelShader(ID3D11PixelShader *shader);
    void BindVertexShader(ID3D11VertexShader *shader);
    void BindShaderResources(enum BindTargets bindTarget,
                             ID3D11ShaderResourceView **SRVs,
                             uint32_t numSRVs);
    void BindConstantBuffers(enum BindTargets bindTarget,
                             ID3D11Buffer **CBs,
                             uint32_t numCBs);
    void BindShaderResource(enum BindTargets bindTarget,
                            ID3D11ShaderResourceView *srv,
                            uint32_t slot);
    void BindConstantBuffer(enum BindTargets bindTarget,
                            ID3D11Buffer *cb,
                            uint32_t slot);

    void DrawIndexed(uint32_t indexCount,
                     uint32_t startIndexLocation,
                     uint32_t baseVertexLocation);

    void Clear(const float *color);

    void Present();

    void ClearRenderTargetView(ID3D11RenderTargetView *rtv,
                               const float *color);
    void ClearDepthStencilView(ID3D11DepthStencilView *dsv,
                               uint32_t mask,
                               float depth,
                               uint8_t stencil);

    void GenerateMips(ID3D11ShaderResourceView *srv);

private:
    DeviceResources *m_DR;
};

}
}
