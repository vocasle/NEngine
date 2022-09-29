#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <cstdint>

#include "NEngine/Math/Math.h"
#include "Actor.h"
#include "Camera.h"

namespace NEngine {
namespace Helpers {
class CubeMap {
public:
    CubeMap();
    ~CubeMap();

    void LoadCubeMap(ID3D11Device *device,
                     const std::vector<const char *> &filepaths);

    ID3D11ShaderResourceView *
    GetCubeMap() const {
        return m_cubeMap.Get();
    }

    ID3D11SamplerState *
    GetCubeMapSampler() const {
        return m_sampler.Get();
    }

    void CreateCube(const Actor &actor, ID3D11Device *device);

    ID3D11Buffer *
    GetVertexBuffer() const {
        return m_vertexBuffer.Get();
    }

    ID3D11Buffer *
    GetIndexBuffer() const {
        return m_indexBuffer.Get();
    }

    uint32_t
    GetNumIndices() const {
        return m_indices.size();
    }

    ID3D11DepthStencilState *
    GetDepthStencilState() const {
        return m_depthStencilState.Get();
    }

    ID3D11RasterizerState *
    GetRasterizerState() const {
        return m_rasterizerState.Get();
    }

private:
    void CreateSampler(ID3D11Device *device);
    void CreateDepthStencilState(ID3D11Device *device);
    void CreateRasterizerState(ID3D11Device *device);

    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D11ShaderResourceView> m_cubeMap;
    ComPtr<ID3D11SamplerState> m_sampler;

    std::vector<Math::Vec3D> m_vertices;
    std::vector<uint32_t> m_indices;
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;
};

}
}
