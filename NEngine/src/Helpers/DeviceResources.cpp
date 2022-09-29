#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Utils/Utils.h"

#include <algorithm>
#include <combaseapi.h>
#include <d3d11_4.h>

namespace NEngine {
namespace Helpers {

using namespace Utils;

DeviceResources::DeviceResources()
    : m_BackbufferWidth{0},
      m_BackbufferHeight{0},
      m_ScreenViewport{},
      m_hWnd{nullptr},
      m_OutputSize{} {
    m_BackbufferHeight = DEFAULT_WIN_HEIGHT;
    m_BackbufferWidth = DEFAULT_WIN_WIDTH;
}

DeviceResources::~DeviceResources() {
}

void
DeviceResources::SetWindow(HWND hWnd, uint32_t width, uint32_t height) {
    m_hWnd = hWnd;
    m_OutputSize.right = width;
    m_OutputSize.bottom = height;
}

void
DeviceResources::CreateFactory() {
    UINT dxgiFactoryFlags = 0;
#if _DEBUG
    {
        Microsoft::WRL::ComPtr<IDXGIInfoQueue> infoQueue;
        if (SUCCEEDED(DXGIGetDebugInterface1(
            0, IID_PPV_ARGS(
                infoQueue.ReleaseAndGetAddressOf())))) {
            dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

            infoQueue->SetBreakOnSeverity(
                DXGI_DEBUG_ALL,
                DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,
                TRUE);
            infoQueue->SetBreakOnSeverity(
                DXGI_DEBUG_ALL,
                DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,
                TRUE);
        }
    }
#endif
    if (FAILED(CreateDXGIFactory2(
        dxgiFactoryFlags,
        IID_PPV_ARGS(m_Factory.ReleaseAndGetAddressOf())))) {
        OutputDebugStringA("ERROR: Failed to create DXGI factory\n");
        ExitProcess(EXIT_FAILURE);
    }
}

void
DeviceResources::CreateRasterizerState() {
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.DepthBias = 100000;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 1.0f;

    if (FAILED(m_Device->CreateRasterizerState(
        &rasterizerDesc,
        m_RasterizerState.ReleaseAndGetAddressOf()))) {
        OutputDebugStringA(
            "ERROR: Failed to create rasterizer state\n");
        ExitProcess(EXIT_FAILURE);
    }
}

void
DeviceResources::CreateDeviceResources() {
    UINT createDeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if _DEBUG
    createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    CreateFactory();

    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1,
    };

    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;

    if (FAILED(m_Factory->EnumAdapters(0,
        adapter.ReleaseAndGetAddressOf()))) {
        OutputDebugStringA("ERROR: Failed to enumerate adapter\n");
        ExitProcess(EXIT_FAILURE);
    }

    if (FAILED(D3D11CreateDevice(
        adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL,
        createDeviceFlag, featureLevels,
        sizeof(featureLevels) / sizeof(featureLevels[0]),
        D3D11_SDK_VERSION, m_Device.ReleaseAndGetAddressOf(), NULL,
        m_Context.ReleaseAndGetAddressOf()))) {
        OutputDebugStringA("ERROR: Failed to create device\n");
        ExitProcess(EXIT_FAILURE);
    }
}

void
DeviceResources::CreateWindowSizeDependentResources() {
    if (!m_hWnd) {
        UtilsDebugPrint(
            "ERROR: hWnd is not set! Windows size dependent "
            "resources will not be created!\n");
        return;
    }

    ID3D11DeviceContext *ctx = m_Context.Get();

    ID3D11RenderTargetView *nullViews[] = {NULL};
    ctx->OMSetRenderTargets(_countof(nullViews), nullViews, NULL);
    ctx->Flush();

    m_RenderTargetView.Reset();
    m_DepthStencilView.Reset();
    m_RenderTarget.Reset();
    m_DepthStencil.Reset();

    const UINT backBufferWidth =
        std::max(m_OutputSize.right - m_OutputSize.left, 1l);
    const UINT backBufferHeight =
        std::max(m_OutputSize.bottom - m_OutputSize.top, 1l);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

    if (m_SwapChain) {
        if (FAILED(m_SwapChain->ResizeBuffers(2, backBufferWidth,
            backBufferHeight,
            backBufferFormat, 0))) {
            OutputDebugStringA("ERROR: Failed to resize buffers\n");
            ExitProcess(EXIT_FAILURE);
        }
    } else {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = 0;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        if (FAILED(m_Factory->CreateSwapChainForHwnd(
            m_Device.Get(), m_hWnd, &swapChainDesc,
            &fsSwapChainDesc, NULL,
            m_SwapChain.ReleaseAndGetAddressOf()))) {
            OutputDebugStringA(
                "ERROR: Failed to create swap chain\n");
            ExitProcess(EXIT_FAILURE);
        }

        // This class does not support exclusive full-screen mode and prevents
        // DXGI from responding to the ALT+ENTER shortcut
        if (FAILED(m_Factory->MakeWindowAssociation(
            m_hWnd, DXGI_MWA_NO_ALT_ENTER))) {
            OutputDebugStringA(
                "ERROR: Failed to make window association\n");
            ExitProcess(EXIT_FAILURE);
        }
    }

    if (FAILED(m_SwapChain->GetBuffer(
        0,
        IID_PPV_ARGS(m_RenderTarget.ReleaseAndGetAddressOf())))) {
        OutputDebugStringA("ERROR: Failed to get render target\n");
        ExitProcess(EXIT_FAILURE);
    }

    {
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
        renderTargetViewDesc.ViewDimension =
            D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        if (FAILED(m_Device->CreateRenderTargetView(
            m_RenderTarget.Get(), &renderTargetViewDesc,
            m_RenderTargetView.ReleaseAndGetAddressOf()))) {
            OutputDebugStringA(
                "ERROR: Failed to create render target view\n");
            ExitProcess(EXIT_FAILURE);
        }
    }

    {
        D3D11_TEXTURE2D_DESC depthStencilDesc = {};
        depthStencilDesc.Width = backBufferWidth;
        depthStencilDesc.Height = backBufferHeight;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;

        if (FAILED(m_Device->CreateTexture2D(
            &depthStencilDesc, NULL,
            m_DepthStencil.ReleaseAndGetAddressOf()))) {
            OutputDebugStringA(
                "ERROR: Failed to create depth stencil texture\n");
            ExitProcess(EXIT_FAILURE);
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        depthStencilViewDesc.ViewDimension =
            D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Format = depthStencilDesc.Format;

        if (FAILED(m_Device->CreateDepthStencilView(
            m_DepthStencil.Get(), &depthStencilViewDesc,
            m_DepthStencilView.ReleaseAndGetAddressOf()))) {
            OutputDebugStringA(
                "ERROR: Failed to create depth stencil view\n");
            ExitProcess(EXIT_FAILURE);
        }
    }

    m_ScreenViewport.TopLeftX = m_ScreenViewport.TopLeftY = 0.0f;
    m_ScreenViewport.Width = (float)backBufferWidth;
    m_ScreenViewport.Height = (float)backBufferHeight;
    m_ScreenViewport.MinDepth = 0;
    m_ScreenViewport.MaxDepth = 1;

    CreateRasterizerState();

    HR(m_Context.As(&m_d3dAnnotation));
}

void
DeviceResources::ReportLiveObjects(void) {
#ifdef _DEBUG
    {
        Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
        if (SUCCEEDED(DXGIGetDebugInterface1(
            0, IID_PPV_ARGS(
                dxgiDebug.ReleaseAndGetAddressOf())))) {
            dxgiDebug->ReportLiveObjects(
                DXGI_DEBUG_ALL,
                static_cast<DXGI_DEBUG_RLO_FLAGS>(
                    DXGI_DEBUG_RLO_DETAIL |
                    DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }
#endif
}

bool
DeviceResources::WindowSizeChanged(int width, int height) {
    RECT newRc;
    newRc.left = newRc.top = 0;
    newRc.right = width;
    newRc.bottom = height;
    if (newRc == m_OutputSize || !m_hWnd) {
        return false;
    }

    m_OutputSize = newRc;
    CreateWindowSizeDependentResources();
    return true;
}

}
}
