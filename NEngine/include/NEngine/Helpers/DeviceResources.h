#pragma once

#include <cstdint>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>
#include <wrl/client.h>

namespace NEngine {
namespace Helpers {
#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

class DeviceResources {
public:
    DeviceResources();
    ~DeviceResources();
    DeviceResources(const DeviceResources &rhs) = delete;
    DeviceResources(const DeviceResources &&rhs) = delete;
    DeviceResources &operator=(const DeviceResources &rhs) = delete;
    DeviceResources &operator=(const DeviceResources &&rhs) = delete;

    void SetWindow(HWND hWnd, uint32_t width, uint32_t height);
    void CreateDeviceResources();
    void CreateWindowSizeDependentResources();
    void ReportLiveObjects();

    ID3D11Device *
    GetDevice() const {
        return m_Device.Get();
    }

    ID3D11DeviceContext *
    GetDeviceContext() const {
        return m_Context.Get();
    }

    ID3D11RenderTargetView *
    GetRenderTargetView() const {
        return m_RenderTargetView.Get();
    }

    ID3D11DepthStencilView *
    GetDepthStencilView() const {
        return m_DepthStencilView.Get();
    }

    uint32_t
    GetBackBufferWidth() const {
        return m_OutputSize.right;
    }

    uint32_t
    GetBackBufferHeight() const {
        return m_OutputSize.bottom;
    }

    ID3D11RasterizerState *
    GetRasterizerState() const {
        return m_RasterizerState.Get();
    }

    const D3D11_VIEWPORT &
    GetViewport() const {
        return m_ScreenViewport;
    }

    IDXGISwapChain *
    GetSwapChain() const {
        return m_SwapChain.Get();
    }

    HWND
    GetWindow() const {
        return m_hWnd;
    }

    RECT
    GetOutputSize() const {
        return m_OutputSize;
    }

    void
    PIXBeginEvent(_In_z_ const wchar_t *name) {
        m_d3dAnnotation->BeginEvent(name);
    }

    void
    PIXEndEvent() {
        m_d3dAnnotation->EndEvent();
    }

    bool WindowSizeChanged(int width, int height);

private:
    void CreateFactory();
    void CreateRasterizerState();

    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain;
    Microsoft::WRL::ComPtr<IDXGIFactory2> m_Factory;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RenderTarget;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencil;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> m_d3dAnnotation;
    uint32_t m_BackbufferWidth;
    uint32_t m_BackbufferHeight;
    D3D11_VIEWPORT m_ScreenViewport;
    HWND m_hWnd;
    RECT m_OutputSize;
};

}
}
