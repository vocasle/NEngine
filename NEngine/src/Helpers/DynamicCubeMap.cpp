#include "NEngine/Helpers/DynamicCubeMap.h"
#include "NEngine/Utils/Utils.h"

#include <cstdint>

namespace NEngine {
namespace Helpers {
using namespace Math;
using namespace Utils;
using namespace Microsoft::WRL;

DynamicCubeMap::DynamicCubeMap()
    : m_cubeMapViewport{} {
}

DynamicCubeMap::~DynamicCubeMap() {
    for (int i = 0; i < 6; ++i) {
        COM_FREE(m_dynamicCubeMapRTV[i]);
    }
}

void
DynamicCubeMap::Init(ID3D11Device *device) {
    CreateRenderTargets(device);
    CreateDepthBuffer(device);
    CreateViewport(device);
}

void
DynamicCubeMap::BuildCubeFaceCamera(const Vec3D &origin) {
    // Generate the cube map about the given position.
    Vec3D worldUp(0.0f, 1.0f, 0.0f);
    // Look along each coordinate axis.
    Vec3D targets[6] = {
        Vec3D(origin.X + 1.0f, origin.Y, origin.Z), // +X
        Vec3D(origin.X - 1.0f, origin.Y, origin.Z), // -X
        Vec3D(origin.X, origin.Y + 1.0f, origin.Z), // +Y
        Vec3D(origin.X, origin.Y - 1.0f, origin.Z), // -Y
        Vec3D(origin.X, origin.Y, origin.Z + 1.0f), // +Z
        Vec3D(origin.X, origin.Y, origin.Z - 1.0f)  // -Z
    };
    // Use world up vector (0,1,0) for all directions except +Y/-Y.
    // In these cases, we are looking down +Y or -Y, so we need a
    // different "up" vector.
    Vec3D ups[6] = {
        Vec3D(0.0f, 1.0f, 0.0f),  // +X
        Vec3D(0.0f, 1.0f, 0.0f),  // -X
        Vec3D(0.0f, 0.0f, -1.0f), // +Y
        Vec3D(0.0f, 0.0f, +1.0f), // -Y
        Vec3D(0.0f, 1.0f, 0.0f),  // +Z
        Vec3D(0.0f, 1.0f, 0.0f)   // -Z
    };
    for (int i = 0; i < 6; ++i) {
        m_cubeMapCamera[i].LookAt(origin, targets[i], ups[i]);
        m_cubeMapCamera[i].SetZFar(1000.0f);
        m_cubeMapCamera[i].SetZNear(0.1f);
        m_cubeMapCamera[i].SetFov(90.0f);
        m_cubeMapCamera[i].SetViewDimensions(CUBEMAP_SIZE,
                                             CUBEMAP_SIZE);
        // m_cubeMapCamera[i].SetLens(0.5f * XM_PI, 1.0f, 0.1f, 1000.0f);
        // m_cubeMapCamera[i].UpdateViewMatrix();
    }
}

void
DynamicCubeMap::CreateRenderTargets(ID3D11Device *device) {
    //
    // Cubemap is a special texture array with 6 elements. We
    // bind this as a render target to draw to the cube faces,
    // and also as a shader resource, so we can use it in a pixel shader.
    //
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = CUBEMAP_SIZE;
    texDesc.Height = CUBEMAP_SIZE;
    texDesc.MipLevels = 0;
    texDesc.ArraySize = 6;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET |
                        D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS |
                        D3D11_RESOURCE_MISC_TEXTURECUBE;
    ComPtr<ID3D11Texture2D> cubeTex;
    HR(device->CreateTexture2D(&texDesc, 0, cubeTex.GetAddressOf()));

    //
    // Create a render target view to each cube map face
    // (i.e., each element in the texture array).
    //
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.MipSlice = 0;
    // Only create a view to one array element.
    rtvDesc.Texture2DArray.ArraySize = 1;
    for (int i = 0; i < 6; ++i) {
        // Create a render target view to the ith element.
        rtvDesc.Texture2DArray.FirstArraySlice = i;
        HR(device->CreateRenderTargetView(cubeTex.Get(), &rtvDesc,
            &m_dynamicCubeMapRTV[i]));
    }

    //
    // Create a shader resource view to the cube map.
    //
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = -1;
    HR(device->CreateShaderResourceView(
        cubeTex.Get(), &srvDesc,
        m_dynamicCubeMapSRV.ReleaseAndGetAddressOf()));
}

void
DynamicCubeMap::CreateDepthBuffer(ID3D11Device *device) {
    D3D11_TEXTURE2D_DESC depthTexDesc = {};
    depthTexDesc.Width = CUBEMAP_SIZE;
    depthTexDesc.Height = CUBEMAP_SIZE;
    depthTexDesc.MipLevels = 1;
    depthTexDesc.ArraySize = 1;
    depthTexDesc.SampleDesc.Count = 1;
    depthTexDesc.SampleDesc.Quality = 0;
    depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
    depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthTexDesc.CPUAccessFlags = 0;
    depthTexDesc.MiscFlags = 0;
    ComPtr<ID3D11Texture2D> depthTex;
    HR(device->CreateTexture2D(&depthTexDesc, 0, depthTex.GetAddressOf()));
    // Create the depth stencil view for the entire buffer.
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Format = depthTexDesc.Format;
    dsvDesc.Flags = 0;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    HR(device->CreateDepthStencilView(
        depthTex.Get(), &dsvDesc,
        m_dynamicCubeMapDSV.ReleaseAndGetAddressOf()));
}

void
DynamicCubeMap::CreateViewport(ID3D11Device *device) {
    m_cubeMapViewport.TopLeftX = 0.0f;
    m_cubeMapViewport.TopLeftY = 0.0f;
    m_cubeMapViewport.Width = static_cast<float>(CUBEMAP_SIZE);
    m_cubeMapViewport.Height = static_cast<float>(CUBEMAP_SIZE);
    m_cubeMapViewport.MinDepth = 0.0f;
    m_cubeMapViewport.MaxDepth = 1.0f;
}

}
}
