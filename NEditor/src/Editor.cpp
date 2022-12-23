#include "NEditor/Editor.h"

#include <mciapi.h>

#include "DirectXMath.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/ModelImporter.h"
#include "NEngine/Helpers/ShaderManager.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Renderer/Texture.h"
#include "NEngine/Utils/Timer.h"
#include "NEngine/Utils/Utils.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

namespace NEditor {

using namespace NEngine::Helpers;
using namespace NEngine::Renderer;
using namespace DirectX;
using namespace NEngine::Input;

void
Editor::Tick()
{
    TimerTick(&mTimer);
    auto deltaMillis = mTimer.DeltaMillis;
    mCamera.Tick(deltaMillis);

    Clear();
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    mBasePass->SetPerSceneValue("dirLight", mDirLight);
    mBasePass->Draw(mDeviceResources, mMeshes);

    UpdateImgui();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    mDeviceResources.GetSwapChain()->Present(1, 0);
}

void
Editor::Initialize(HWND wnd, uint32_t width, uint32_t height)
{
    TimerInitialize(&mTimer);
    mDeviceResources.SetWindow(wnd, width, height);
    mDeviceResources.CreateDeviceResources();
    mDeviceResources.CreateWindowSizeDependentResources();
    mViewport.Width = width / 2.0f;
    mViewport.Height = height / 2.0f;
    mViewport.MinDepth = 0;
    mViewport.MaxDepth = 1;

    mRenderTarget =
        std::make_unique<Texture>(mDeviceResources,
                                  mViewport.Width,
                                  mViewport.Height,
                                  TextureBindTarget::RenderTargetView,
                                  "Viewport Render Target");

    mDepthTarget =
        std::make_unique<Texture>(mDeviceResources,
                                  mViewport.Width,
                                  mViewport.Height,
                                  TextureBindTarget::DepthStencilView,
                                  "Viewport Depth View");

    mBasePass = std::make_unique<BasePass>(mDeviceResources);
    mBasePass->SetCamera(mCamera);
    mCamera.SetFov(XMConvertToRadians(45));
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

    RegisterKeys();
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
Editor::ProcessViewportInput()
{
    if (!ImGui::IsWindowFocused()) {
        return;
    }
    ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
    ImVec2 screenPositionAbsolute = ImGui::GetItemRectMin();
    ImVec2 mousePositionRelative =
        ImVec2(mousePositionAbsolute.x - screenPositionAbsolute.x,
               mousePositionAbsolute.y - screenPositionAbsolute.y);

    static bool prevIsLeftDown = false;
    static bool prevIsRightDown = false;
    const bool isLeftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool isRightDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);

    const auto mousePosAbs =
        Vec2D(mousePositionAbsolute.x, mousePositionAbsolute.y);

    if (isLeftDown && !prevIsLeftDown) {
        Mouse::Get().OnMouseDown(mousePosAbs, Mouse::ButtonType::Left);
    }
    if (isRightDown && !prevIsRightDown) {
        Mouse::Get().OnMouseDown(mousePosAbs, Mouse::ButtonType::Right);
    }
    if (prevIsLeftDown && !isLeftDown) {
        Mouse::Get().OnMouseUp(mousePosAbs, Mouse::ButtonType::Left);
    }
    if (prevIsRightDown && !isRightDown) {
        Mouse::Get().OnMouseUp(mousePosAbs, Mouse::ButtonType::Right);
    }
    Mouse::Get().OnMouseMove(mousePosAbs);
    // UtilsDebugPrint("Mouse pos: %s\n", mousePosAbs.ToString().c_str());

    prevIsLeftDown = isLeftDown;
    prevIsRightDown = isRightDown;
}

void
Editor::UpdateImgui()
{
    static bool *p_open;
    static bool outputRenderTarget = true;
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
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    static bool isViewportSettingsVisible = false;
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
            if (ImGui::MenuItem("Show preview window")) {
                isViewportSettingsVisible = !isViewportSettingsVisible;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close", nullptr, false, p_open != nullptr)) {
                if (p_open) {
                    *p_open = false;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

        ImGui::Begin("Viewport");
        ProcessViewportInput();
        if (!OnViewportSizeChanged()) {
            auto srv = outputRenderTarget
                           ? mRenderTarget->GetShaderResourceView()
                           : mDepthTarget->GetShaderResourceView();
            ImGui::Image((void *)srv,
                         ImVec2(mViewport.Width, mViewport.Height));
        }
        ImGui::End();

        if (isViewportSettingsVisible) {
            ImGui::Begin("Viewport settings");
            if (ImGui::RadioButton("Render Target", outputRenderTarget)) {
                outputRenderTarget = true;
            }
            if (ImGui::RadioButton("Depth Target", !outputRenderTarget)) {
                outputRenderTarget = false;
            }

            if (ImGui::Button("Open model")) {
                OPENFILENAME ofn = {sizeof(ofn)};
                WCHAR szPath[MAX_PATH] = {};
                ofn.lpstrFilter =
                    L"GLB - gLTF 2.0 binary\0*.glb\0GLTF - gLTF 2.0\0*.gltf\0";
                ofn.lpstrFile = szPath;
                ofn.nMaxFile = ARRAYSIZE(szPath);
                ofn.hwndOwner = mDeviceResources.GetWindow();
                if (GetOpenFileName(&ofn)) {
                    LoadMesh(UtilsWstrToStr(szPath));
                }
            }

            static const char *options[] = {"DEBUG_NONE",
                                            "DEBUG_NDF",
                                            "DEBUG_GEOMETRY",
                                            "DEBUG_FRESNEL",
                                            "DEBUG_NORMAL",
                                            "DEBUG_ROUGHNESS",
                                            "DEBUG_METALLIC",
                                            "DEBUG_BASE_COLOR",
                                            "DEBUG_DIFFUSE_BRDF",
                                            "DEBUG_SPECULAR_BRDF"};
            static const char *current_item = options[0];
            static ShaderDefine define("", "");
            if (ImGui::Button("Recompile all shaders")) {
                ShaderManager::RecompileShaders(mDeviceResources, {define});
                mBasePass->ReloadShaders();
            }

            if (ImGui::BeginCombo("Debug layer", current_item)) {
                for (int n = 0; n < IM_ARRAYSIZE(options); n++) {
                    bool is_selected =
                        (current_item ==
                         options[n]);  // You can store your selection however
                                       // you want, outside or inside your
                                       // objects

                    if (ImGui::Selectable(options[n], is_selected)) {
                        current_item = options[n];
                        define.Name = current_item;
                        define.Value = "1";
                        ShaderManager::RecompileShaders(mDeviceResources,
                                                        {define});
                        mBasePass->ReloadShaders();
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();  // You may set the
                                                       // initial focus when
                                                       // opening the combo
                                                       // (scrolling + for
                                                       // keyboard navigation
                                                       // support)
                    }
                }

                ImGui::EndCombo();
            }
            ImGui::End();
        }

        ImGui::Begin("Properties");
        if (ImGui::CollapsingHeader("Camera")) {
            static float zNear = 1;
            static float zFar = 10;

            ImGui::InputFloat("Z near", &zNear);
            ImGui::InputFloat("Z far", &zFar);

            if (ImGui::Button("Apply##amera")) {
                mCamera.SetZFar(zFar);
                mCamera.SetZNear(zNear);
            }
        }

        if (ImGui::CollapsingHeader("Directional Light")) {
            ImGui::ColorPicker4("Diffuse",
                                reinterpret_cast<float *>(&mDirLight.Diffuse));
            ImGui::SliderFloat4("Direction",
                                reinterpret_cast<float *>(&mDirLight.Direction),
                                -1,
                                1);
        }
        ImGui::End();
    }
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

        mCamera.SetFov(XMConvertToRadians(fovAngleY));
    }

    mRenderTarget->Resize(mDeviceResources, viewportSize);
    mDepthTarget->Resize(mDeviceResources, viewportSize);
}

void
Editor::RegisterKeys()
{
    // Reset camera
    Keyboard::Get().RegisterKey('X');
}

void
Editor::LoadMesh(const std::string &path)
{
    ModelImporter importer(mDeviceResources);
    auto model = importer.Load(path);
    if (!model.empty()) {
        mMeshes.clear();
        std::move(
            std::begin(model), std::end(model), std::back_inserter(mMeshes));
    }
}
}  // namespace NEditor