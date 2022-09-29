#include "NEngine/Helpers/CubeMap.h"
#include "stb_image/stb_image.h"
#include "NEngine/Utils/Utils.h"
#include "NEngine/Helpers/Actor.h"

namespace NEngine {
namespace Helpers {
using namespace Microsoft::WRL;
using namespace Math;
using namespace Utils;
using namespace Helpers;

CubeMap::CubeMap() {
}

CubeMap::~CubeMap() {
}

void
CubeMap::LoadCubeMap(ID3D11Device *device,
                     const std::vector<const char *> &filepaths) {
    int width = 0;
    int height = 0;
    int channelsInFile = 0;
    const int desiredChannels = 4;
    uint8_t *bytes = stbi_load(filepaths[0],
                               &width,
                               &height,
                               &channelsInFile,
                               desiredChannels);
    stbi_image_free(bytes);

    //Description of each face
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 6;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.CPUAccessFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc = {};
    SMViewDesc.Format = texDesc.Format;
    SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
    SMViewDesc.TextureCube.MostDetailedMip = 0;

    D3D11_SUBRESOURCE_DATA pData[6];
    uint8_t *pBytes[6];

    for (uint32_t i = 0; i < filepaths.size(); ++i) {
        pBytes[i] = stbi_load(filepaths[i],
                              &width,
                              &height,
                              &channelsInFile,
                              desiredChannels);
        assert(pBytes[i] && "stbi_load failed");
        pData[i].pSysMem = pBytes[i];
        pData[i].SysMemPitch = width * desiredChannels;

        if (!pBytes[i]) {
            UtilsDebugPrint("ERROR: Failed to load texture from %s\n",
                            filepaths[i]);
            ExitProcess(EXIT_FAILURE);
        }
    }

    ComPtr<ID3D11Texture2D> texture;
    HR(device->CreateTexture2D(&texDesc, pData, texture.ReleaseAndGetAddressOf()
    ));
    HR(device->CreateShaderResourceView(texture.Get(), &SMViewDesc, m_cubeMap.
        ReleaseAndGetAddressOf()));

    for (uint32_t i = 0; i < 6; ++i) {
        stbi_image_free(pBytes[i]);
    }

    CreateSampler(device);
    CreateDepthStencilState(device);
    CreateRasterizerState(device);
}

void
CubeMap::CreateCube(const Actor &actor, ID3D11Device *device) {
    m_vertices.reserve(actor.GetVertices().size());
    for (auto it = std::begin(actor.GetVertices());
         it != std::end(actor.GetVertices()); ++it) {
        m_vertices.emplace_back(it->Position);
    }
    m_indices.insert(std::end(m_indices),
                     std::begin(actor.GetIndices()),
                     std::end(actor.GetIndices()));

    UtilsCreateVertexBuffer(device,
                            &m_vertices[0],
                            m_vertices.size(),
                            sizeof(Vec3D),
                            m_vertexBuffer.ReleaseAndGetAddressOf());
    UtilsCreateIndexBuffer(device,
                           &m_indices[0],
                           m_indices.size(),
                           m_indexBuffer.ReleaseAndGetAddressOf());
}

void
CubeMap::CreateSampler(ID3D11Device *device) {
    D3D11_SAMPLER_DESC desc = {};
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    HR(device->CreateSamplerState(&desc, m_sampler.ReleaseAndGetAddressOf()));
}

void
CubeMap::CreateDepthStencilState(ID3D11Device *device) {
    CD3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC
        {CD3D11_DEFAULT{}};
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    HR(device->CreateDepthStencilState(&desc, m_depthStencilState.
        ReleaseAndGetAddressOf()))
}

void
CubeMap::CreateRasterizerState(ID3D11Device *device) {
    CD3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC{CD3D11_DEFAULT{}};
    desc.CullMode = D3D11_CULL_NONE;

    HR(device->CreateRasterizerState(&desc, m_rasterizerState.
        ReleaseAndGetAddressOf()))
}

}
}
