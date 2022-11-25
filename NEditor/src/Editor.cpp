#include "NEditor/Editor.h"

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/ModelImporter.h"
#include "NEngine/Renderer/Texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace NEditor {

using namespace NEngine::Helpers;
using namespace NEngine::Renderer;

void
Editor::Tick()
{
    Clear();
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    mBasePass->Draw(mDeviceResources, mMeshes);

    UpdateImgui();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    mDeviceResources.GetSwapChain()->Present(1, 0);

    ImGui::EndFrame();
}

void
Editor::Initialize(HWND wnd, uint32_t width, uint32_t height)
{
    mDeviceResources.SetWindow(wnd, width, height);
    mDeviceResources.CreateDeviceResources();
    mDeviceResources.CreateWindowSizeDependentResources();
    mViewport.Width = width / 2.0f;
    mViewport.Height = height / 2.0f;

    mRenderTarget =
        std::make_unique<Texture>(mDeviceResources,
                                  0,
                                  TextureBindTarget::RenderTargetView,
                                  mViewport.Width,
                                  mViewport.Height,
                                  "Viewport Render Target");

    mDepthTarget =
        std::make_unique<Texture>(mDeviceResources,
                                  0,
                                  TextureBindTarget::DepthStencilView,
                                  mViewport.Width,
                                  mViewport.Height,
                                  "Viewport Depth View");

    mBasePass = std::make_unique<BasePass>(mDeviceResources);
    mBasePass->SetCamera(mCamera);
    mBasePass->SetRenderTarget(*mRenderTarget);
    mBasePass->SetDepthTarget(*mDepthTarget);
    mBasePass->SetViewport(mViewport);
    mCamera.SetViewDimensions(mViewport.Width, mViewport.Height);

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(wnd);
    ImGui_ImplDX11_Init(mDeviceResources.GetDevice(),
                        mDeviceResources.GetDeviceContext());

    {
        ModelImporter importer(mDeviceResources);

        const auto helmetPath =
            R"(D:\Source\glTF-Sample-Models\2.0\DamagedHelmet\glTF-Binary\DamagedHelmet.glb)";

        auto helmet = importer.Load(helmetPath);
        std::move(
            std::begin(helmet), std::end(helmet), std::back_inserter(mMeshes));
    }
}

void
Editor::GetDefaultSize(uint32_t *width, uint32_t *height)
{
    *width = mWinSize.x;
    *height = mWinSize.y;
}
void
Editor::OnWindowSizeChanged(int width, int height)
{
    mWinSize.x = width;
    mWinSize.y = height;

    mDeviceResources.WindowSizeChanged(width, height);
}

void
Editor::UpdateImgui()
{
    ImGui::SetNextWindowSize(ImVec2(mViewport.Width, mViewport.Height));
    ImGui::SetNextWindowPos(
        ImVec2(mViewport.Width - mViewport.Width / 2.0f, 0));
    ImGui::Begin("Viewport");
    ImGui::Image((void *)mRenderTarget->GetShaderResourceView(),
                 ImVec2(mViewport.Width, mViewport.Height));
    ImGui::End();
}

Editor::~Editor()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void
Editor::Clear()
{
    ID3D11DeviceContext *ctx = mDeviceResources.GetDeviceContext();
    ID3D11RenderTargetView *rtv = mDeviceResources.GetRenderTargetView();
    ID3D11DepthStencilView *dsv = mDeviceResources.GetDepthStencilView();

    static const float CLEAR_COLOR[4] = {
        0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

    ctx->Flush();

    ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    ctx->RSSetViewports(1, &mDeviceResources.GetViewport());
}
}  // namespace NEditor