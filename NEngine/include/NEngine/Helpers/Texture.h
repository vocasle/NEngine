#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <string>

namespace NEngine {
namespace Helpers {
class Texture {
public:
    Texture(DXGI_FORMAT format, int width, int height, ID3D11Device *device);
    Texture(DXGI_FORMAT format,
            int width,
            int height,
            void *data,
            size_t sysMemPitch,
            size_t sysMemSlicePitch,
            ID3D11Device *device);
    Texture(const std::string &filepath,
            ID3D11Device *device,
            ID3D11DeviceContext *context);
    Texture(const Texture &rhs);
    Texture &operator=(const Texture &rhs);
    void GenerateMips(ID3D11DeviceContext *context);

    [[nodiscard]] ID3D11ShaderResourceView *
    GetSRV() const {
        return mSRV.Get();
    }

    [[nodiscard]] ID3D11RenderTargetView *
    GetRTV() const {
        return mRTV.Get();
    }

    [[nodiscard]] ID3D11DepthStencilView *
    GetDSV() const {
        return mDSV.Get();
    }

    [[nodiscard]] ID3D11ShaderResourceView *
    GetDepthSRV() const {
        return mDepthSRV.Get();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthSRV;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthTexture;

    int mWidth;
    int mHeight;
    DXGI_FORMAT mFormat;
};

}
}
