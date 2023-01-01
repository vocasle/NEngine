#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <glm/vec3.hpp>

#include "Camera.h"

namespace NEngine {
namespace Helpers {
class DynamicCubeMap
{
public:
    DynamicCubeMap();
    ~DynamicCubeMap();

    void Init(ID3D11Device *device);
    void BuildCubeFaceCamera(const glm::vec3 &origin);

    D3D11_VIEWPORT
    GetViewport() const
    {
        return m_cubeMapViewport;
    }

    ID3D11RenderTargetView *
    GetRTV(const uint32_t idx) const
    {
        return m_dynamicCubeMapRTV[idx];
    }

    ID3D11DepthStencilView *
    GetDSV() const
    {
        return m_dynamicCubeMapDSV.Get();
    }

    ID3D11ShaderResourceView *
    GetSRV() const
    {
        return m_dynamicCubeMapSRV.Get();
    }

    const Camera &
    GetCamera(const uint32_t idx) const
    {
        return m_cubeMapCamera[idx];
    }

private:
    void CreateRenderTargets(ID3D11Device *device);
    void CreateDepthBuffer(ID3D11Device *device);
    void CreateViewport(ID3D11Device *device);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dynamicCubeMapSRV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dynamicCubeMapDSV;
    D3D11_VIEWPORT m_cubeMapViewport;
    ID3D11RenderTargetView *m_dynamicCubeMapRTV[6];
    Camera m_cubeMapCamera[6];

    static const int CUBEMAP_SIZE = 256;
};

}  // namespace Helpers
}  // namespace NEngine
