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
        NEngine::Helpers::ShaderManager::RecompileShaders(
            mEngine->GetDeviceResources());
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

    if (ImGui::Button("Open model")) {
        OPENFILENAME ofn = {sizeof(ofn)};
        WCHAR szPath[MAX_PATH] = {};
        ofn.lpstrFilter =
            L"GLB - gLTF 2.0 binary\0*.glb\0GLTF - gLTF 2.0\0*.gltf\0";
        ofn.lpstrFile = szPath;
        ofn.nMaxFile = ARRAYSIZE(szPath);
        ofn.hwndOwner = mEngine->GetDeviceResources().GetWindow();
        if (GetOpenFileName(&ofn)) {
            auto mesh = mEngine->LoadMesh(UtilsWstrToStr(szPath));
            std::move(
                std::begin(mesh), std::end(mesh), std::back_inserter(m_meshes));
        }
    }
}
#endif

MyGame::MyGame()
    : m_camera({0, 0, 5}),
      mEngine(nullptr)
{
}

void
MyGame::Clear()
{
    ID3D11DeviceContext *ctx = mEngine->GetDeviceResources().GetDeviceContext();
    ID3D11RenderTargetView *rtv =
        mEngine->GetDeviceResources().GetRenderTargetView();
    ID3D11DepthStencilView *dsv =
        mEngine->GetDeviceResources().GetDepthStencilView();

    static const float CLEAR_COLOR[4] = {
        0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

    ctx->Flush();

    ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    ctx->RSSetViewports(1, &mEngine->GetDeviceResources().GetViewport());
}

void
MyGame::Render()
{
#if WITH_IMGUI
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif

    Clear();

#if WITH_IMGUI
    UpdateImgui();
#endif

    m_basePass->Draw(mEngine->GetDeviceResources(), m_meshes);

#if WITH_IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    const HRESULT hr =
        mEngine->GetDeviceResources().GetSwapChain()->Present(1, 0);

    ID3D11DeviceContext1 *ctx = reinterpret_cast<ID3D11DeviceContext1 *>(
        mEngine->GetDeviceResources().GetDeviceContext());
    ctx->DiscardView(
        (ID3D11View *)mEngine->GetDeviceResources().GetRenderTargetView());
    if (mEngine->GetDeviceResources().GetDepthStencilView()) {
        ctx->DiscardView(
            (ID3D11View *)mEngine->GetDeviceResources().GetDepthStencilView());
    }
}

void
MyGame::Update(float dt)
{
    {
        m_camera.ProcessKeyboard(dt);
        m_camera.ProcessMouse(dt);

        static float elapsedTime = 0.0f;
        const auto deltaSeconds = static_cast<float>(dt / 1000.0);
        elapsedTime += deltaSeconds;
        static float dirLightTime = 0;
        dirLightTime += deltaSeconds;

        if (elapsedTime >= 1.0f) {
            const std::string title =
                UtilsFormatStr("Sandbox -- FPS: %d, frame: %f s",
                               static_cast<int>(elapsedTime / deltaSeconds),
                               deltaSeconds);
            SetWindowText(mEngine->GetDeviceResources().GetWindow(),
                          UtilsStrToWstr(title).c_str());
            elapsedTime = 0.0f;
        }

        auto dirLight = m_basePass->GetBufferValue<DirectionalLight>(
            "dirLight", BufferType::PerScene);

        if (dirLight) {
            constexpr float radius = 100;
            dirLight->Direction = Vec4D(cos(dirLightTime) * radius,
                                        0,
                                        sin(dirLightTime) * radius,
                                        radius);
        }
    }

    Render();
}

auto
MyGame::InitWithEngine(NEngine::Engine &engine) -> void
{
    mEngine = &engine;

    auto winSize = mEngine->GetWindowSize();

    Mouse::Get().SetWindowDimensions(winSize.X, winSize.Y);

    m_basePass = std::make_unique<BasePass>(mEngine->GetDeviceResources());
    m_basePass->SetCamera(m_camera);

    m_camera.SetZFar(10000);
    m_camera.SetZNear(0.1f);
}
