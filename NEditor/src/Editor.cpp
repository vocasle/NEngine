#include "NEditor/Editor.h"

#include "DirectXMath.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/ModelImporter.h"
#include "NEngine/Renderer/Texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace NEditor {

using namespace NEngine::Helpers;
using namespace NEngine::Renderer;
using namespace DirectX;

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
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
    static bool *p_open;
    // If you strip some features of, this demo is pretty much equivalent to
    // calling DockSpaceOverViewport()!
    // In most cases you should be able to just call DockSpaceOverViewport() and
    // ignore all the code below! In this specific demo, we are not using
    // DockSpaceOverViewport() because:
    // - we allow the host window to be floating/moveable instead of filling the
    // viewport (when opt_fullscreen == false)
    // - we allow the host window to have padding (when opt_padding == true)
    // - we have a local menu bar in the host window (vs. you could use
    // BeginMainMenuBar() + DockSpaceOverViewport() in your code!) TL;DR; this
    // demo is more complicated than what you would normally use. If we removed
    // all the options we are showcasing, this demo would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;
    }
    else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }
    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin() to
    // not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    // Important: note that we proceed even if Begin() returns false (aka window
    // is collapsed). This is because we want to keep our DockSpace() active. If
    // a DockSpace() is inactive, all active windows docked into it will lose
    // their parent and become undocked. We cannot preserve the docking
    // relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in
    // limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();
    if (opt_fullscreen)
        ImGui::PopStyleVar(2);
    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else {
        // ShowDockingDisabledMessage();
    }
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            // Disabling fullscreen would allow the window to be moved to the
            // front of other windows, which we can't undo at the moment without
            // finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();
            if (ImGui::MenuItem(
                    "Flag: NoSplit",
                    "",
                    (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            }
            if (ImGui::MenuItem(
                    "Flag: NoResize",
                    "",
                    (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem(
                    "Flag: NoDockingInCentralNode",
                    "",
                    (dockspace_flags &
                     ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar",
                                "",
                                (dockspace_flags &
                                 ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode",
                                "",
                                (dockspace_flags &
                                 ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                                opt_fullscreen)) {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
        // HelpMarker(
        //     "When docking is enabled, you can ALWAYS dock MOST window into "
        //     "another! Try it now!"
        //     "\n"
        //     "- Drag from window title bar or their tab to dock/undock."
        //     "\n"
        //     "- Drag from window menu button (upper-left button) to undock an
        //     " "entire node (all windows)."
        //     "\n"
        //     "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift ==
        //     " "false, default)"
        //     "\n"
        //     "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift ==
        //     " "true)"
        //     "\n"
        //     "This demo app has nothing to do with enabling docking!"
        //     "\n\n"
        //     "This demo app only demonstrate the use of ImGui::DockSpace() "
        //     "which allows you to manually create a docking node _within_ "
        //     "another window."
        //     "\n\n"
        //     "Read comments in ShowExampleAppDockSpace() for more details.");
        ImGui::EndMenuBar();

        ImGui::Begin("Viewport");
        if (!OnViewportSizeChanged()) {
            ImGui::Image((void *)mRenderTarget->GetShaderResourceView(),
                         ImVec2(mViewport.Width, mViewport.Height));
        }

        // {
        //     ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        //     ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        //     // vMin.x += ImGui::GetWindowPos().x;
        //     // vMin.y += ImGui::GetWindowPos().y;
        //     // vMax.x += ImGui::GetWindowPos().x;
        //     // vMax.y += ImGui::GetWindowPos().y;

        //     // mViewport.Width = vMax.x - vMin.x;
        //     // mViewport.Height = vMax.y - vMin.y;

        //     // ImGui::GetForegroundDrawList()->AddRect(
        //     //     vMin, vMax, IM_COL32(255, 255, 0, 255));
        // }
        ImGui::End();
    }
    ImGui::End();

    // ImGui::SetNextWindowSize(ImVec2(mViewport.Width, mViewport.Height));
    // ImGui::SetNextWindowPos(
    //     ImVec2(mViewport.Width - mViewport.Width / 2.0f, 0));
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

bool
Editor::OnViewportSizeChanged()
{
    auto min = ImGui::GetWindowContentRegionMin();
    auto max = ImGui::GetWindowContentRegionMax();
    auto viewportSize = XMFLOAT2(max.x - min.x, max.y - min.y);
    const auto oldViewportSize = XMFLOAT2(mViewport.Width, mViewport.Height);
    const auto epsilon = 0.01f;
    if (!XMVector2NearEqual(XMLoadFloat2(&viewportSize),
                            XMLoadFloat2(&oldViewportSize),
                            XMLoadFloat(&epsilon))) {
        UpdateViewportSize(viewportSize);
        return true;
    }
    return false;
}

void
Editor::UpdateViewportSize(DirectX::XMFLOAT2 viewportSize)
{
    mViewport.Width = viewportSize.x;
    mViewport.Height = viewportSize.y;

    mCamera.SetViewDimensions(viewportSize.x, viewportSize.y);
    {
        const float aspectRatio = static_cast<float>(viewportSize.y) /
                                  static_cast<float>(viewportSize.x);
        float fovAngleY = 45.0f;

        // portrait or snapped view.
        if (aspectRatio < 1.0f) {
            fovAngleY *= 2.0f;
        }

        mCamera.SetFov(fovAngleY);
    }

    mRenderTarget->Resize(mDeviceResources, viewportSize);
    mDepthTarget->Resize(mDeviceResources, viewportSize);
}
}  // namespace NEditor