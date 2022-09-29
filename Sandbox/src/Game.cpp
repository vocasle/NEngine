#include "Sandbox/Game.h"

#include <d3dcompiler.h>

#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Helpers/Texture.h"
#include "NEngine/Utils/Utils.h"

#if WITH_IMGUI
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui.h>
#endif

#include <stdexcept>

using namespace Microsoft::WRL;
using namespace NEngine::Helpers;
using namespace NEngine::Utils;
using namespace NEngine::Math;
using namespace NEngine::Input;

struct SponzaSettings {
    ParticleSystemOptions firePSOpts;
};

namespace {
SponzaSettings gSettings;
};

static void
InitSponzaSettings() {
    ParticleSystemOptions &fire1 = gSettings.firePSOpts;
    fire1.isEnabled = true;
    fire1.maxParticles = 100;
    fire1.initVelRandFact = 2;
    fire1.burst = 5;
    fire1.accel = {0, 1.5f, 0};
    fire1.origin = {112, 12, 45};
    fire1.particleSize = {12, 12};
}

void
Game::CreateRasterizerState() {
    throw std::runtime_error("Not implemented");
}

#if WITH_IMGUI
void
Game::UpdateImgui() {
    if (ImGui::Button("Recompile all shaders")) {
        m_shaderManager.Recompile(m_deviceResources->GetDevice());
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

    if (ImGui::CollapsingHeader("Lights setting")) {
        if (ImGui::CollapsingHeader("Directional light settings")) {
            ImGui::InputFloat4(
                "Position##PointLight",
                reinterpret_cast<float *>(
                    m_perSceneCB->GetValue<Vec4D>("dirLight.Position")));
        }

        if (ImGui::CollapsingHeader("Point light settings")) {
            static int e = 0;
            ImGui::RadioButton("First", &e, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Second", &e, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Third", &e, 2);
            ImGui::SameLine();
            ImGui::RadioButton("Fourth", &e, 3);
            //            ImGui::ColorPicker4(
            //                "Ambient",
            //                reinterpret_cast<float *>(
            //                    m_perSceneCB->GetValue<Vec4D>("pointLights[0].Ambient")));
            //
            //            ImGui::ColorPicker4(
            //                "Diffuse",
            //                reinterpret_cast<float *>(
            //                    m_perSceneCB->GetValue<Vec4D>("pointLights[0].Diffuse")));
            //
            //            ImGui::ColorPicker4(
            //                "Specular",
            //                reinterpret_cast<float *>(
            //                    m_perSceneCB->GetValue<Vec4D>("pointLights[0].Specular")));

            ImGui::InputFloat4(
                UtilsFormatStr("Position##PointLight_No_%s", e).c_str(),
                reinterpret_cast<float *>(m_perSceneCB->GetValue<Vec4D>(
                    UtilsFormatStr("pointLights[%d].Position", e))));
        }

        if (ImGui::CollapsingHeader("Spot lights settings")) {
            static float spot = 10;
            static float range = 10;
            ImGui::InputFloat("Range##Spot1", &range);
            ImGui::InputFloat("Spot##Spot1", &spot);
            if (ImGui::Button("Apply##Spot1")) {
                Vec4D pos =
                    *m_perSceneCB->GetValue<Vec4D>("spotLights[0].Position");
                pos.W = range;
                m_perSceneCB->SetValue("spotLights[0].Position", pos);

                Vec4D dir =
                    *m_perSceneCB->GetValue<Vec4D>("spotLights[0].Direction");
                dir.W = spot;
                m_perSceneCB->SetValue("spotLights[0].Direction", dir);
            }
        }
    }

    if (ImGui::CollapsingHeader("Fire")) {
        ImGui::Checkbox("Enable fire", &m_firePS.GetOptions().isEnabled);
        ImGui::InputFloat("Lifespan##FirePS", &m_firePS.GetOptions().lifespan);
        ImGui::ColorEdit4(
            "Color##FirePS",
            reinterpret_cast<float *>(&gSettings.firePSOpts.color));

        ImGui::InputInt("Fire num particles",
                        &m_firePS.GetOptions().maxParticles);
        ImGui::InputFloat("Fire accel random factor",
                          &m_firePS.GetOptions().accelRandFact);
        ImGui::InputFloat("Fire init vel random factor",
                          &m_firePS.GetOptions().initVelRandFact);
        ImGui::InputInt("Fire burst (n per 100 ms)",
                        &m_firePS.GetOptions().burst);
        ImGui::InputFloat3(
            "Fire init vel",
            reinterpret_cast<float *>(&m_firePS.GetOptions().initVel));
        ImGui::InputFloat3(
            "Fire accel",
            reinterpret_cast<float *>(&m_firePS.GetOptions().accel));
        ImGui::InputFloat3(
            "Fire pos",
            reinterpret_cast<float *>(&m_firePS.GetOptions().origin));
        ImGui::InputFloat2(
            "Fire size",
            reinterpret_cast<float *>(&m_firePS.GetOptions().particleSize));

        if (ImGui::Button("Update")) {
            m_firePS.Reset();
        }
    }
}
#endif

void
Game::CreateDefaultSampler() {
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
    samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if (FAILED(m_deviceResources->GetDevice()->CreateSamplerState(
        &samplerDesc, m_defaultSampler.ReleaseAndGetAddressOf()))) {
        UtilsDebugPrint("ERROR: Failed to create default sampler state\n");
        ExitProcess(EXIT_FAILURE);
    }
}

Game::Game()
    : m_camera({0, 10, -5}),
      m_firePS("fire", {0, 0, 0}, {0, 10, 0}, {0, 0, 0}, m_camera) {
    m_deviceResources = std::make_unique<DeviceResources>();
}

Game::~Game() {
#if WITH_IMGUI
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}

void
Game::Clear() {
    ID3D11DeviceContext *ctx = m_deviceResources->GetDeviceContext();
    ID3D11RenderTargetView *rtv = m_deviceResources->GetRenderTargetView();
    ID3D11DepthStencilView *dsv = m_deviceResources->GetDepthStencilView();

    static const float CLEAR_COLOR[4] = {
        0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

    ctx->Flush();

    ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    ctx->RSSetViewports(1, &m_deviceResources->GetViewport());
}

void
Game::Update() {
    m_camera.ProcessKeyboard(m_timer.DeltaMillis);
    m_camera.ProcessMouse(m_timer.DeltaMillis);

    m_perFrameCB->SetValue("view", m_camera.GetViewMat());
    m_perFrameCB->SetValue("proj", m_camera.GetProjMat());
    m_perFrameCB->SetValue("cameraPosW", Vec4D(m_camera.GetPos(), 0));

    m_perSceneCB->SetValue("spotLights[0].Position",
                           Vec4D(m_camera.GetPos(), 10));
    m_perSceneCB->SetValue("spotLights[0].Direction",
                           Vec4D(m_camera.GetAt(), 10));

    static float elapsedTime = 0.0f;
    const auto deltaSeconds = static_cast<float>(m_timer.DeltaMillis / 1000.0);
    elapsedTime += deltaSeconds;

    if (m_firePS.GetOptions().isEnabled) {
        m_firePS.Tick(deltaSeconds);
        m_firePS.UpdateVertexBuffer(m_deviceResources->GetDeviceContext());
    }

    if (elapsedTime >= 1.0f) {
        const std::string title = UtilsFormatStr(
            "Sandbox -- FPS: %d, frame: %f s",
            static_cast<int>(elapsedTime / deltaSeconds),
            deltaSeconds);
        SetWindowText(
            m_deviceResources->GetWindow(),
            UtilsStrToWstr(title).c_str()
            );
        elapsedTime = 0.0f;
    }
}

void
Game::Render() {
#if WITH_IMGUI
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif

#if WITH_IMGUI
    UpdateImgui();
#endif

    static constexpr float CLEAR_COLOR[] = {0.2f, 0.2f, 0.2f, 1};
    m_renderer.SetRenderTargets(m_deviceResources->GetRenderTargetView(),
                                m_deviceResources->GetDepthStencilView());
    m_renderer.SetViewport(m_deviceResources->GetViewport());
    m_renderer.Clear(CLEAR_COLOR);
    m_renderer.SetBlendState(nullptr);
    m_renderer.SetDepthStencilState(nullptr);
    m_renderer.SetPrimitiveTopology(R_DEFAULT_PRIMTIVE_TOPOLOGY);
    m_renderer.SetRasterizerState(m_rasterizerState.Get());
    m_renderer.SetSamplerState(m_defaultSampler.Get(), 0);

    Mat4X4 shadowView = {};
    Mat4X4 shadowProj = {};
    BuildShadowTransform(shadowView, shadowProj);
    m_deviceResources->PIXBeginEvent(L"Shadow pass");
    {
        m_shadowMap.Bind(m_deviceResources->GetDeviceContext());

        m_renderer.BindPixelShader(nullptr);
        m_renderer.BindVertexShader(
            m_shaderManager.GetVertexShader("ShadowVS"));
        m_renderer.BindConstantBuffer(
            BindTargets::VertexShader,
            m_perObjectCB->Get(),
            0);
        m_renderer.BindConstantBuffer(
            BindTargets::VertexShader,
            m_perFrameCB->Get(),
            1);
        m_renderer.SetInputLayout(m_shaderManager.GetInputLayout());
        m_renderer.SetSamplerState(m_shadowMap.GetShadowSampler(), 1);
        m_perFrameCB->SetValue("proj", shadowProj);
        m_perFrameCB->SetValue("view", shadowView);
        m_perFrameCB->UpdateConstantBuffer();
        m_renderer.SetRasterizerState(m_shadowMap.GetRasterizerState());
        DrawActors();
        m_renderer.SetRasterizerState(nullptr);
        m_shadowMap.Unbind(m_deviceResources->GetDeviceContext());
        m_perFrameCB->SetValue("proj", m_camera.GetProjMat());
        m_perFrameCB->SetValue("view", m_camera.GetViewMat());
        m_perFrameCB->UpdateConstantBuffer();
    }
    m_deviceResources->PIXEndEvent();

    m_deviceResources->PIXBeginEvent(L"Dynamic cube map pass");
    {
        m_renderer.SetDepthStencilState(nullptr);
        m_renderer.SetViewport(m_dynamicCubeMap.GetViewport());
        //m_actors[1].SetVisible(false);
        m_renderer.BindVertexShader(m_shaderManager.GetVertexShader("ColorVS"));
        m_renderer.BindPixelShader(m_shaderManager.GetPixelShader("PhongPS"));
        m_perPassCB->SetValue("calcReflection", 0);
        m_perPassCB->UpdateConstantBuffer();
        m_renderer.BindConstantBuffer(
            BindTargets::PixelShader,
            m_perPassCB->Get(),
            3);

        for (int i = 0; i < 6; ++i) {
            // Bind cube map face as render target.
            m_renderer.SetRenderTargets(m_dynamicCubeMap.GetRTV(i),
                                        m_dynamicCubeMap.GetDSV());
            // Clear cube map face and depth buffer.
            m_renderer.Clear(CLEAR_COLOR);
            m_perFrameCB->SetValue(
                "cameraPosW",
                Vec4D(m_dynamicCubeMap.GetCamera(i).GetPos(), 0));
            m_perFrameCB->SetValue("view",
                                   m_dynamicCubeMap.GetCamera(i).GetViewMat());
            m_perFrameCB->SetValue("proj",
                                   m_dynamicCubeMap.GetCamera(i).GetProjMat());
            m_perFrameCB->UpdateConstantBuffer();
            DrawActors();
            m_renderer.SetDepthStencilState(nullptr);
        }

        // reset viewport
        m_renderer.SetViewport(m_deviceResources->GetViewport());
        m_renderer.SetRenderTargets(m_deviceResources->GetRenderTargetView(),
                                    m_deviceResources->GetDepthStencilView());
        m_renderer.Clear(CLEAR_COLOR);
        //m_actors[1].SetVisible(true);
        m_renderer.SetRasterizerState(nullptr);
        m_renderer.SetDepthStencilState(nullptr);
        m_perFrameCB->SetValue("proj", m_camera.GetProjMat());
        m_perFrameCB->SetValue("view", m_camera.GetViewMat());
        m_perFrameCB->SetValue("cameraPosW", Vec4D(m_camera.GetPos(), 0));
        m_perFrameCB->UpdateConstantBuffer();
    }
    m_deviceResources->PIXEndEvent();

    m_deviceResources->PIXBeginEvent(L"Color pass");
    // reset view proj matrix back to camera
    {
        m_perPassCB->SetValue("calcReflection", 1);
        m_perPassCB->UpdateConstantBuffer();
        m_renderer.BindConstantBuffer(
            BindTargets::PixelShader,
            m_perPassCB->Get(),
            3);

        m_renderer.SetSamplerState(m_defaultSampler.Get(), 0);
        m_renderer.SetInputLayout(m_shaderManager.GetInputLayout());

        m_perFrameCB->UpdateConstantBuffer();
        m_perSceneCB->UpdateConstantBuffer();
        m_perObjectCB->UpdateConstantBuffer();

        m_renderer.BindConstantBuffer(
            BindTargets::VertexShader,
            m_perObjectCB->Get(),
            0);
        m_renderer.BindConstantBuffer(
            BindTargets::VertexShader,
            m_perFrameCB->Get(),
            1);
        m_renderer.BindConstantBuffer(
            BindTargets::VertexShader,
            m_perSceneCB->Get(),
            2);
        m_renderer.BindConstantBuffer(
            BindTargets::PixelShader,
            m_perObjectCB->Get(),
            0);
        m_renderer.BindConstantBuffer(
            BindTargets::PixelShader,
            m_perFrameCB->Get(),
            1);
        m_renderer.BindConstantBuffer(
            BindTargets::PixelShader,
            m_perSceneCB->Get(),
            2);
        m_renderer.BindShaderResource(
            BindTargets::PixelShader,
            m_shadowMap.GetDepthMapSRV(),
            4);
        m_renderer.SetSamplerState(m_shadowMap.GetShadowSampler(), 1);
        m_renderer.BindShaderResource(
            BindTargets::PixelShader,
            m_dynamicCubeMap.GetSRV(),
            6);
        DrawActors();
        m_renderer.SetSamplerState(nullptr, 1);
        m_renderer.BindShaderResource(BindTargets::PixelShader, nullptr, 6);
    }
    m_deviceResources->PIXEndEvent();

    if (m_firePS.GetOptions().isEnabled) {
        m_deviceResources->PIXBeginEvent(L"Draw fire");
        m_renderer.SetBlendState(m_firePS.GetBlendState());
        m_renderer.SetDepthStencilState(m_firePS.GetDepthStencilState());
        m_renderer.BindVertexShader(
            m_shaderManager.GetVertexShader("ParticleVS"));
        m_renderer.SetVertexBuffer(
            m_firePS.GetVertexBuffer(),
            m_shaderManager.GetStrides(),
            0);
        m_renderer.SetIndexBuffer(m_firePS.GetIndexBuffer(), 0);
        m_renderer.BindPixelShader(
            m_shaderManager.GetPixelShader("ParticlePS"));
        m_renderer.SetInputLayout(m_shaderManager.GetInputLayout());
        m_renderer.BindConstantBuffer(
            BindTargets::VertexShader,
            m_perFrameCB->Get(),
            0);
        m_renderer.BindShaderResource(
            BindTargets::PixelShader,
            m_firePS.GetSRV(),
            0);
        m_renderer.SetSamplerState(m_firePS.GetSamplerState(), 0);
        const Vec4D prevCamVal = *m_perFrameCB->GetValue<Vec4D>("cameraPosW");
        m_perFrameCB->SetValue("cameraPosW", gSettings.firePSOpts.color);
        m_perFrameCB->UpdateConstantBuffer();
        m_renderer.BindConstantBuffer(
            BindTargets::PixelShader,
            m_perFrameCB->Get(),
            0);
        m_renderer.DrawIndexed(m_firePS.GetNumIndices(), 0, 0);
        m_perFrameCB->SetValue("cameraPosW", prevCamVal);
        m_deviceResources->PIXEndEvent();
    }

#if WITH_IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    m_renderer.Present();
}

void
Game::Tick() {
    TimerTick(&m_timer);
    Update();
    Render();
}

void
Game::Initialize(HWND hWnd, uint32_t width, uint32_t height) {
    using namespace Microsoft::WRL;
#ifdef MATH_TEST
    MathTest();
#endif
    m_deviceResources->SetWindow(hWnd, width, height);
    m_deviceResources->CreateDeviceResources();
    m_deviceResources->CreateWindowSizeDependentResources();
    InitSponzaSettings();
    TimerInitialize(&m_timer);
    Mouse::Get().SetWindowDimensions(m_deviceResources->GetBackBufferWidth(),
                                     m_deviceResources->GetBackBufferHeight());
    ID3D11Device *device = m_deviceResources->GetDevice();

    m_shaderManager.Initialize(
        device,
        SHADER_BINARY_ROOT,
        UtilsFormatStr("%s/shader", SHADER_SRC_ROOT));

    CreateDefaultSampler();
    // CreateRasterizerState();

    m_camera.SetZFar(10000);
    m_camera.SetZNear(10);

    m_renderer.SetDeviceResources(m_deviceResources.get());
    m_assetManager = std::make_unique<AssetManager>(*m_deviceResources);
    //m_actors.emplace_back(m_assetManager->LoadModel(
    //    UtilsFormatStr("%s/Sponza.gltf", SPONZA_ROOT)));
    //m_actors.emplace_back(m_assetManager->LoadModel(
    //    UtilsFormatStr("%s/Suzanne/glTF/Suzanne.gltf", ASSETS_ROOT)));
    //m_actors[0].SetWorld(MathMat4X4ScaleFromVec3D({0.1f, 0.1f, 0.1f}));
    //m_actors[1].SetRoughness(0.1f);
    //m_actors[1].SetWorld(MathMat4X4ScaleFromVec3D({10, 10, 10}) *
    //                     MathMat4X4TranslateFromVec3D({0, 20, 0}));

    //m_firePS.Init(m_deviceResources->GetDevice(),
    //              m_deviceResources->GetDeviceContext(),
    //              UtilsFormatStr("%s/textures/particlePack_1.1/flame_05.png",
    //                             ASSETS_ROOT));
    //m_firePS.SetOptions(gSettings.firePSOpts);

    const int shadowMapSize = 2048;
    m_shadowMap.InitResources(
        m_deviceResources->GetDevice(),
        shadowMapSize,
        shadowMapSize);

    m_dynamicCubeMap.Init(m_deviceResources->GetDevice());
    m_dynamicCubeMap.BuildCubeFaceCamera({0, 0, 0});

    {
        DynamicConstBufferDesc perObjectDesc;
        perObjectDesc.AddNode(Node("worldInvTranspose", NodeType::Float4X4));
        perObjectDesc.AddNode(Node("world", NodeType::Float4X4));
        perObjectDesc.AddNode(Node("shadowTransform", NodeType::Float4X4));
        Node material("material", NodeType::Struct);
        material.AddChild("roughness", NodeType::Float);
        perObjectDesc.AddNode(material);

        m_perObjectCB = std::make_unique<DynamicConstBuffer>(
            perObjectDesc,
            *m_deviceResources);

        DynamicConstBufferDesc perSceneDesc;
        Node dirLight = Node("dirLight", NodeType::Struct);
        dirLight.AddChild("Ambient", NodeType::Float4);
        dirLight.AddChild("Diffuse", NodeType::Float4);
        dirLight.AddChild("Specular", NodeType::Float4);
        dirLight.AddChild("Position", NodeType::Float4);

        Node pointLight = Node("pointLight", NodeType::Struct);
        pointLight.AddChild("Ambient", NodeType::Float4);
        pointLight.AddChild("Diffuse", NodeType::Float4);
        pointLight.AddChild("Specular", NodeType::Float4);
        pointLight.AddChild("Position", NodeType::Float4);
        Node pointLightsArr = Node("pointLights", NodeType::Array);
        pointLightsArr.AddChildN(pointLight, 6);

        Node spotLight = Node("spotLight", NodeType::Struct);
        spotLight.AddChild("Ambient", NodeType::Float4);
        spotLight.AddChild("Diffuse", NodeType::Float4);
        spotLight.AddChild("Specular", NodeType::Float4);
        spotLight.AddChild("Position", NodeType::Float4);
        spotLight.AddChild("Direction", NodeType::Float4);
        Node spotLightsArr = Node("spotLights", NodeType::Array);
        spotLightsArr.AddChildN(spotLight, 2);

        perSceneDesc.AddNode(dirLight);
        perSceneDesc.AddNode(pointLightsArr);
        perSceneDesc.AddNode(spotLightsArr);

        m_perSceneCB = std::make_unique<DynamicConstBuffer>(perSceneDesc,
            *m_deviceResources);

        DynamicConstBufferDesc perFrameDesc;
        perFrameDesc.AddNode(Node("view", NodeType::Float4X4));
        perFrameDesc.AddNode(Node("proj", NodeType::Float4X4));
        perFrameDesc.AddNode(Node("cameraPosW", NodeType::Float4));

        m_perFrameCB = std::make_unique<DynamicConstBuffer>(perFrameDesc,
            *m_deviceResources);

        m_perSceneCB->SetValue("dirLight.Diffuse",
                               Vec4D(1.0f, 1.0f, 1.0f, 1.0f));
        m_perSceneCB->SetValue("dirLight.Position", Vec4D(10, 150, 10, 150));

        DynamicConstBufferDesc perPassDesc;
        perPassDesc.AddNode(Node("calcReflection", NodeType::Float));

        m_perPassCB = std::make_unique<DynamicConstBuffer>(perPassDesc,
            *m_deviceResources);

        m_perSceneCB->SetValue("pointLights[0].Position",
                               Vec4D(100, 10, 0, 100));
        m_perSceneCB->SetValue("pointLights[1].Position",
                               Vec4D(-100, 10, 0, 100));
        m_perSceneCB->SetValue("pointLights[2].Position",
                               Vec4D(0, 10, 50, 100));
        m_perSceneCB->SetValue("pointLights[3].Position",
                               Vec4D(0, 10, 7 - 50, 100));
        m_perSceneCB->SetValue("pointLights[4].Position",
                               Vec4D(40, 50, 50, 100));
        m_perSceneCB->SetValue("pointLights[5].Position",
                               Vec4D(-40, 50, -50, 100));
    }

#if WITH_IMGUI
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    //const std::string droidSansTtf = UtilsFormatStr(
    //    "%s/../imgui-1.87/misc/fonts/DroidSans.ttf",
    //    SOURCE_ROOT);
    //io.Fonts->AddFontFromFileTTF(droidSansTtf.c_str(), 16.0f);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_deviceResources->GetDevice(),
                        m_deviceResources->GetDeviceContext());
#endif
}

void
Game::GetDefaultSize(uint32_t *width, uint32_t *height) {
    *width = DEFAULT_WIN_WIDTH;
    *height = DEFAULT_WIN_HEIGHT;
}

void
Game::OnWindowSizeChanged(int width, int height) {
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void
Game::CreateWindowSizeDependentResources() {
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

void
Game::DrawMeshes(const std::vector<Mesh> &meshes) {
    for (const Mesh &mesh : meshes) {
        for (const TextureInfo &ti : mesh.GetTextures()) {
            if (ti.Type == TextureType::Diffuse) {
                if (Texture *tex = m_assetManager->GetTexture(ti.Path)) {
                    m_renderer.BindShaderResource(
                        BindTargets::PixelShader,
                        tex->GetSRV(),
                        0);
                }
            }
            if (ti.Type == TextureType::Normal) {
                if (Texture *tex = m_assetManager->GetTexture(ti.Path)) {
                    m_renderer.BindShaderResource(
                        BindTargets::PixelShader,
                        tex->GetSRV(),
                        3);
                }
            }
            if (ti.Type == TextureType::Specular ||
                ti.Type == TextureType::Metalness) {
                if (Texture *tex = m_assetManager->GetTexture(ti.Path)) {
                    m_renderer.BindShaderResource(
                        BindTargets::PixelShader,
                        tex->GetSRV(),
                        1);
                }
            }
            if (ti.Type == TextureType::Shininess ||
                ti.Type == TextureType::DiffuseRoughness) {
                if (Texture *tex = m_assetManager->GetTexture(ti.Path)) {
                    m_renderer.BindShaderResource(
                        BindTargets::PixelShader,
                        tex->GetSRV(),
                        2);
                }
            }
        }
        m_renderer.SetVertexBuffer(
            mesh.GetVertexBuffer(),
            mesh.GetVertexSize(),
            0);
        m_renderer.SetIndexBuffer(mesh.GetIndexBuffer(), 0);
        m_renderer.DrawIndexed(mesh.GetIndexCount(), 0, 0);
    }
}

void
Game::BuildShadowTransform(Mat4X4 &view, Mat4X4 &proj) {
    // Only the first "main" light casts a shadow.
    const Vec4D dirLightPos =
        *m_perSceneCB->GetValue<Vec4D>("dirLight.Position");
    const Vec3D lightPos = {dirLightPos.X, dirLightPos.Y, dirLightPos.Z};
    const Vec3D targetPos = {0.0f, 0.0f, 0.0f};
    const Vec3D worldUp = {0.0f, 1.0f, 0.0f};
    const float radius = MathVec3DLength(lightPos);

    const Vec3D right = (targetPos - lightPos).Cross(worldUp);
    const Vec3D up = right.Cross(targetPos - lightPos);

    view = MathMat4X4ViewAt(&lightPos, &targetPos, &up);
    proj = MathMat4X4OrthographicOffCenter(-radius,
                                           radius,
                                           -radius,
                                           radius,
                                           m_camera.GetZNear(),
                                           m_camera.GetZFar());
}

void
Game::DrawActors() {
    Mat4X4 shadowView = {};
    Mat4X4 shadowProj = {};
    BuildShadowTransform(shadowView, shadowProj);
    for (const Actor &actor : m_actors) {
        if (actor.IsVisible()) {
            m_perObjectCB->SetValue("material.roughness", actor.GetRoughness());
            m_perObjectCB->SetValue("world", actor.GetWorld());
            m_perObjectCB->SetValue("worldInvTranspose", actor.GetWorld());
            m_perObjectCB->SetValue("shadowTransform",
                                    actor.GetWorld() * shadowView * shadowProj);
            m_perObjectCB->UpdateConstantBuffer();
            DrawMeshes(actor.GetMeshes());
        }
    }
}
