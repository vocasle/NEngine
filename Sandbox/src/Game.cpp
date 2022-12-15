#include "Sandbox/Game.h"

#include <d3dcompiler.h>

#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Utils/Utils.h"

#if WITH_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#endif

#include <stdexcept>

#include "NEngine/Helpers/ModelImporter.h"
#include "NEngine/Helpers/ShaderManager.h"

using namespace Microsoft::WRL;
using namespace NEngine::Helpers;
using namespace NEngine::Utils;
using namespace NEngine::Math;
using namespace NEngine::Input;
using namespace NEngine::Renderer;

#if WITH_IMGUI
void
MyGame::UpdateImgui()
{
    if (ImGui::Button("Recompile all shaders")) {
        NEngine::Helpers::ShaderManager::RecompileShaders(*m_deviceResources);
        m_basePass->ReloadShaders();
    }

    if (ImGui::CollapsingHeader("Camera settings")) {
        static float zFar = m_camera.GetZFar();
        static float zNear = m_camera.GetZNear();

        ImGui::InputFloat("z far", &zFar);
        ImGui::InputFloat("z near", &zNear);
        if (ImGui::Button("Apply##Camera")) {
            m_camera.SetZFar(zFar);
            m_camera.SetZNear(zNear);
        }
    }
}
#endif

MyGame::MyGame()
    : m_camera({0, 0, 5})
{
    m_deviceResources = std::make_unique<DeviceResources>();
}

MyGame::~MyGame()
{
#if WITH_IMGUI
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}

void
MyGame::Clear()
{
    ID3D11DeviceContext *ctx = m_deviceResources->GetDeviceContext();
    ID3D11RenderTargetView *rtv = m_deviceResources->GetRenderTargetView();
    ID3D11DepthStencilView *dsv = m_deviceResources->GetDepthStencilView();

    static const float CLEAR_COLOR[4] = {
        0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

    ctx->Flush();

    ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    ctx->RSSetViewports(1, &m_deviceResources->GetViewport());
}

void
MyGame::Update()
{
    m_camera.ProcessKeyboard(m_timer.DeltaMillis);
    m_camera.ProcessMouse(m_timer.DeltaMillis);

    static float elapsedTime = 0.0f;
    const auto deltaSeconds = static_cast<float>(m_timer.DeltaMillis / 1000.0);
    elapsedTime += deltaSeconds;
    static float dirLightTime = 0;
    dirLightTime += deltaSeconds;

    if (elapsedTime >= 1.0f) {
        const std::string title =
            UtilsFormatStr("Sandbox -- FPS: %d, frame: %f s",
                           static_cast<int>(elapsedTime / deltaSeconds),
                           deltaSeconds);
        SetWindowText(m_deviceResources->GetWindow(),
                      UtilsStrToWstr(title).c_str());
        elapsedTime = 0.0f;
    }

    auto dirLight = m_basePass->GetBufferValue<DirectionalLight>(
        "dirLight", BufferType::PerScene);

    if (dirLight) {
        constexpr float radius = 100;
        dirLight->Direction = Vec4D(
            cos(dirLightTime) * radius, 0, sin(dirLightTime) * radius, radius);
    }
}

void
MyGame::Render()
{
#if WITH_IMGUI
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif

#if WITH_IMGUI
    UpdateImgui();
#endif

    m_basePass->Draw(*m_deviceResources, m_meshes);

#if WITH_IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    const HRESULT hr = m_deviceResources->GetSwapChain()->Present(1, 0);

    ID3D11DeviceContext1 *ctx = reinterpret_cast<ID3D11DeviceContext1 *>(
        m_deviceResources->GetDeviceContext());
    ctx->DiscardView((ID3D11View *)m_deviceResources->GetRenderTargetView());
    if (m_deviceResources->GetDepthStencilView()) {
        ctx->DiscardView(
            (ID3D11View *)m_deviceResources->GetDepthStencilView());
    }
}

void
MyGame::Update(float dt)
{
    TimerTick(&m_timer);
    Update();
    Render();
}

void
MyGame::Initialize(HWND hWnd, uint32_t width, uint32_t height)
{
    using namespace Microsoft::WRL;

    m_deviceResources->SetWindow(hWnd, width, height);
    m_deviceResources->CreateDeviceResources();
    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
    TimerInitialize(&m_timer);
    Mouse::Get().SetWindowDimensions(m_deviceResources->GetBackBufferWidth(),
                                     m_deviceResources->GetBackBufferHeight());
    
    m_basePass = std::make_unique<BasePass>(*m_deviceResources);
    m_basePass->SetCamera(m_camera);

    m_camera.SetZFar(10000);
    m_camera.SetZNear(0.1f);

#if WITH_IMGUI
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_deviceResources->GetDevice(),
                        m_deviceResources->GetDeviceContext());
#endif
}

void
MyGame::GetDefaultSize(uint32_t *width, uint32_t *height)
{
    *width = DEFAULT_WIN_WIDTH;
    *height = DEFAULT_WIN_HEIGHT;
}

void
MyGame::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void
MyGame::CreateWindowSizeDependentResources()
{
    const auto size = m_deviceResources->GetOutputSize();
    const float aspectRatio =
        static_cast<float>(size.right) / static_cast<float>(size.bottom);
    float fovAngleY = 45.0f;

    // portrait or snapped view.
    if (aspectRatio < 1.0f) {
        fovAngleY *= 2.0f;
    }

    m_camera.SetFov(fovAngleY);
    m_camera.SetViewDimensions(size.right, size.bottom);
}
