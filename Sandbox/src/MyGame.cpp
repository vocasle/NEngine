#include "Sandbox/MyGame.h"

#include <d3dcompiler.h>

#include "NEngine/ECS/Systems/InputSystem.h"
#include "NEngine/ECS/Systems/MoveSystem.h"
#include "NEngine/ECS/Systems/RenderSystem.h"
#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"
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

void
PrintComponents(const GameObject &go)
{
    if ((go.ComponentMask & ComponentType_CAMERA) == ComponentType_CAMERA) {
        ImGui::Text("\tCAMERA");
    }
    if ((go.ComponentMask & ComponentType_POSITION) == ComponentType_POSITION) {
        ImGui::Text("\tPOSITION");
    }
    if ((go.ComponentMask & ComponentType_RENDER) == ComponentType_RENDER) {
        ImGui::Text("\tRENDER");
    }
    if ((go.ComponentMask & ComponentType_INPUT) == ComponentType_INPUT) {
        ImGui::Text("\tINPUT");
    }
    if ((go.ComponentMask & ComponentType_COLLISION) ==
        ComponentType_COLLISION) {
        ImGui::Text("\tCOLLISION");
    }
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
        static float zFar = 100;
        static float zNear = 0.1;

        ImGui::InputFloat("z far", &zFar);
        ImGui::InputFloat("z near", &zNear);

        if (ImGui::Button("Apply##Camera")) {
            const auto player = mScene.FindEntityByName("Player");

            if (player) {
                auto &cam =
                    *mEntityManager.GetComponent<CameraComponent>(player->ID);
                auto &pos =
                    *mEntityManager.GetComponent<PositionComponent>(player->ID);
                cam.Camera.SetZFar(zFar);
                cam.Camera.SetZNear(zNear);
            }
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

    if (ImGui::CollapsingHeader("Player World")) {
        const auto &pc = *mEntityManager.GetComponent<PositionComponent>(
            mScene.FindEntityByName("Player")->ID);
        auto translate = MathMat4X4TranslateFromVec3D(&pc.Position);
        auto axis = Vec3D(0, 1, 0);
        auto rotate = XM::RotateAxis(ToRadians(pc.Yaw), axis);
        ImGui::Text("%s", (translate * rotate).ToString().c_str());
    }

    if (ImGui::Begin("Scene")) {
        if (ImGui::CollapsingHeader("Graph")) {
            mScene.Visit(
                [](const GameObject &go) -> void
                {
                    ImGui::Text("%s", go.Name.c_str());
                    PrintComponents(go);
                });
        }
    }
    ImGui::End();
}
#endif

MyGame::MyGame()
    : mEngine(nullptr)
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

    CreateSystems();

    CreatePlayer();

    auto plane = mEntityManager.CreateEntity();
    auto &planeMesh = mEntityManager.CreateComponent<RenderComponent>(plane);
    planeMesh.Mesh = mEngine->LoadMesh(
        UtilsFormatStr("%s/%s", GAME_RES_DIR, "\\gLTF\\plane.glb"));
    auto &planePos = mEntityManager.CreateComponent<PositionComponent>(plane);
    planePos.Movable = false;
    mScene.AddToScene({plane, "Ground", mEntityManager.GetBitmask(plane)});

    auto obj = mEntityManager.CreateEntity();
    auto &objMesh = mEntityManager.CreateComponent<RenderComponent>(obj);
    objMesh.Mesh = mEngine->LoadMesh(
        UtilsFormatStr("%s/%s", GAME_RES_DIR, "\\gLTF\\cube.glb"));
    auto &objPos = mEntityManager.CreateComponent<PositionComponent>(obj);
    objPos.Position = {5, 2, 5};
    objPos.Movable = false;

    auto &ac = mEntityManager.CreateComponent<AudioComponent>(obj);
    ac.IsPlaying = false;
    ac.Path = UtilsFormatStr("%s/audio/GenericMale_VoicePack/Frustraion_3.wav",
                             GAME_RES_DIR);

    auto &dbgCubeCol = mEntityManager.CreateComponent<CollisionComponent>(obj);
    dbgCubeCol.BoxMin = vec3(-0.5f, -0.5f, -0.5f);
    dbgCubeCol.BoxMax = vec3(0.5f, 0.5f, 0.5f);
    dbgCubeCol.OnCollision = [&ac](Entity lhs, Entity rhs)
    { ac.IsPlaying = true; };
    mScene.AddToScene({obj, "DebugCube", mEntityManager.GetBitmask(obj)});
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

auto
MyGame::CreatePlayer() -> void
{
    auto player = mEntityManager.CreateEntity();
    auto &pos = mEntityManager.CreateComponent<PositionComponent>(player);
    pos.Position.Y = 2;

    auto &renderComp = mEntityManager.CreateComponent<RenderComponent>(player);
    renderComp.Mesh = mEngine->LoadMesh(
        UtilsFormatStr("%s/%s", GAME_RES_DIR, "\\gLTF\\player.glb"));
    auto &ic = mEntityManager.CreateComponent<InputComponent>(player);

    auto &camComp = mEntityManager.CreateComponent<CameraComponent>(player);
    camComp.Camera.SetZFar(10000);
    camComp.Camera.SetZNear(0.1f);
    camComp.Camera.LookAt({0, 10, -20}, {0, 0, -1}, {0, 1, 0});
    const auto winSize = mEngine->GetWindowSize();
    camComp.Camera.SetViewDimensions(winSize.X, winSize.Y);
    pos.Movable = true;
    auto &cc = mEntityManager.CreateComponent<CollisionComponent>(player);
    cc.BoxMin = vec3(-1, -1, -1);
    cc.BoxMax = vec3(1, 1, 1);
    cc.OnCollision = [this](Entity e1, Entity e2)
    {
        auto &go1 = *mScene.FindEntityById(e1);
        auto &go2 = *mScene.FindEntityById(e2);
        UTILS_PRINTLN(
            "%s collided with %s", go1.Name.c_str(), go2.Name.c_str());
    };

    mScene.AddToScene({player, "Player", mEntityManager.GetBitmask(player)});
}

auto
MyGame::CreateSystems() -> void
{
    mSystemManager.SetSystem(std::make_unique<MoveSystem>(mEntityManager));
    mSystemManager.SetSystem(std::make_unique<RenderSystem>(
        mEngine->GetDeviceResources(), mEntityManager));
    mSystemManager.SetSystem(std::make_unique<InputSystem>(mEntityManager));
    mSystemManager.SetSystem(std::make_unique<CollisionSystem>(mEntityManager));
    mSystemManager.SetSystem(std::make_unique<AudioSystem>(mEntityManager));
}
