#include "Sandbox/Game.h"

#include <d3dcompiler.h>

#include "NEngine/ECS/Systems/InputSystem.h"
#include "NEngine/ECS/Systems/MoveSystem.h"
#include "NEngine/ECS/Systems/RenderSystem.h"
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
using namespace NEngine::ECS;
using namespace NEngine::ECS::Components;
using namespace NEngine::ECS::Systems;
using namespace NEngine;

#if WITH_IMGUI

#define IG_COMPONENT_NAME(componentType)                    \
    if (go.ComponentMask & ComponentType_##componentType) { \
        ImGui::Text("\t" #componentType);                   \
    }

void
MyGame::UpdateImgui()
{
    if (ImGui::Button("Recompile all shaders")) {
        NEngine::Helpers::ShaderManager::RecompileShaders(
            mEngine->GetDeviceResources());
        mSystemManager.GetSystem<RenderSystem>().ReloadShaders();
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
            auto &renderComp = *mEntityManager.GetComponent<RenderComponent>(
                mScene.FindEntityByName("Player")->ID);
            renderComp.Mesh = mEngine->LoadMesh(UtilsWstrToStr(szPath));
        }
    }

    if (ImGui::Begin("Scene")) {
        if (ImGui::CollapsingHeader("Graph")) {
            mScene.Visit(
                [](const GameObject &go) -> void
                {
                    ImGui::Text("%s", go.Name.c_str());
                    IG_COMPONENT_NAME(POSITION)
                    IG_COMPONENT_NAME(INPUT)
                    IG_COMPONENT_NAME(RENDER)
                });
        }
    }
    ImGui::End();
}
#endif

MyGame::MyGame()
    : m_camera({0, 0, -5}),
      mEngine(nullptr)
{
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
    }

    mSystemManager.Update(dt);

    Render();
}

auto
MyGame::InitWithEngine(NEngine::Engine &engine) -> void
{
    mEngine = &engine;

    mEntityManager.RegisterOnComponentAddCallback([this](Entity entity) -> void
                                                  { OnComponentAdd(entity); });
    mEntityManager.RegisterOnComponentRemoveCallback(
        [this](Entity entity) -> void { OnComponentRemove(entity); });

    auto winSize = mEngine->GetWindowSize();

    Mouse::Get().SetWindowDimensions(winSize.X, winSize.Y);

    m_camera.SetZFar(10000);
    m_camera.SetZNear(0.1f);
    m_camera.SetViewDimensions(winSize.X, winSize.Y);

    mSystemManager.SetSystem(std::make_unique<MoveSystem>(mEntityManager));
    mSystemManager.SetSystem(std::make_unique<RenderSystem>(
        mEngine->GetDeviceResources(), mEntityManager, m_camera));
    mSystemManager.SetSystem(std::make_unique<InputSystem>(mEntityManager));

    auto helmet = mEntityManager.CreateEntity();
    auto &pos = mEntityManager.CreateComponent<PositionComponent>(helmet);
    UtilsDebugPrint("Helmet default position: %f %f %f\n",
                    pos.Position.x,
                    pos.Position.y,
                    pos.Position.z);

    auto &renderComp = mEntityManager.CreateComponent<RenderComponent>(helmet);
    renderComp.Mesh = mEngine->LoadMesh("D:\\Assets\\cube.glb");
    auto &ic = mEntityManager.CreateComponent<InputComponent>(helmet);

    mScene.AddToScene({helmet, "Player", mEntityManager.GetBitmask(helmet)});

    auto plane = mEntityManager.CreateEntity();
    auto &planeMesh = mEntityManager.CreateComponent<RenderComponent>(plane);
    planeMesh.Mesh = mEngine->LoadMesh(
        UtilsFormatStr("%s/%s", NENGINE_RES_DIR, "\\gLTF\\plane.glb"));
    auto &planePos = mEntityManager.CreateComponent<PositionComponent>(plane);
    mScene.AddToScene({plane, "Ground", mEntityManager.GetBitmask(plane)});
}

auto
MyGame::OnComponentAdd(NEngine::ECS::Entity entity) -> void
{
    mSystemManager.RegisterEntity(entity);
}

auto
MyGame::OnComponentRemove(NEngine::ECS::Entity entity) -> void
{
    mSystemManager.UnregisterEntity(entity);
}
