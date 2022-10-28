#include "Sandbox/Game.h"

#include <d3dcompiler.h>

#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Utils/Utils.h"

#if WITH_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#endif

#include <stdexcept>

#include "NEngine/Helpers/GLTFLoader.h"

using namespace Microsoft::WRL;
using namespace NEngine::Helpers;
using namespace NEngine::Utils;
using namespace NEngine::Math;
using namespace NEngine::Input;
using namespace NEngine::Renderer;

#if WITH_IMGUI
void
Game::UpdateImgui()
{
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
}
#endif

void
Game::CreateDefaultSampler()
{
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
      m_firePS("fire", {0, 0, 0}, {0, 10, 0}, {0, 0, 0}, m_camera)
{
    m_deviceResources = std::make_unique<DeviceResources>();
}

Game::~Game()
{
#if WITH_IMGUI
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}

void
Game::Clear()
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
Game::Update()
{
    m_camera.ProcessKeyboard(m_timer.DeltaMillis);
    m_camera.ProcessMouse(m_timer.DeltaMillis);

    m_perFrameCB->SetValue("view", m_camera.GetViewMat());
    auto projMat = m_camera.GetProjMat();
    m_perFrameCB->SetValue("proj", projMat);
    m_perFrameCB->SetValue("cameraPosW", Vec4D(m_camera.GetPos(), 0));

    m_perSceneCB->SetValue("spotLights[0].Position",
                           Vec4D(m_camera.GetPos(), 10));
    m_perSceneCB->SetValue("spotLights[0].Direction",
                           Vec4D(m_camera.GetAt(), 10));

    static float elapsedTime = 0.0f;
    const auto deltaSeconds = static_cast<float>(m_timer.DeltaMillis / 1000.0);
    elapsedTime += deltaSeconds;

    if (elapsedTime >= 1.0f) {
        const std::string title =
            UtilsFormatStr("Sandbox -- FPS: %d, frame: %f s",
                           static_cast<int>(elapsedTime / deltaSeconds),
                           deltaSeconds);
        SetWindowText(m_deviceResources->GetWindow(),
                      UtilsStrToWstr(title).c_str());
        elapsedTime = 0.0f;
    }
}

void
Game::Render()
{
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
    m_renderer.SetSamplerState(m_defaultSampler.Get(), 0);

    Mat4X4 shadowView = {};
    Mat4X4 shadowProj = {};
    BuildShadowTransform(shadowView, shadowProj);

    m_deviceResources->PIXBeginEvent(L"Color pass");
    {
        m_basePass->Draw(*m_deviceResources, m_meshes);
    }
    // reset view proj matrix back to camera
    {
        // m_perPassCB->SetValue("calcReflection", 1);
        // m_perPassCB->UpdateConstantBuffer();
        // m_renderer.BindConstantBuffer(
        //     BindTargets::PixelShader,
        //     m_perPassCB->Get(),
        //     3);

        // m_renderer.BindVertexShader(
        //     m_shaderManager.GetVertexShader("ColorVS.cso"));
        // m_renderer.BindPixelShader(
        //     m_shaderManager.GetPixelShader("PhongPS.cso"));
        // m_renderer.SetSamplerState(m_defaultSampler.Get(), 0);
        // m_renderer.SetInputLayout(m_shaderManager.GetInputLayout());
        // m_perObjectCB->SetValue("world", MathMat4X4Identity());
        // m_perObjectCB->SetValue("worldInvTranspose", MathMat4X4Identity());

        // m_perFrameCB->UpdateConstantBuffer();
        // m_perSceneCB->UpdateConstantBuffer();
        // m_perObjectCB->UpdateConstantBuffer();

        // m_renderer.BindConstantBuffer(
        //     BindTargets::VertexShader,
        //     m_perObjectCB->Get(),
        //     0);
        // m_renderer.BindConstantBuffer(
        //     BindTargets::VertexShader,
        //     m_perFrameCB->Get(),
        //     1);
        // m_renderer.BindConstantBuffer(
        //     BindTargets::VertexShader,
        //     m_perSceneCB->Get(),
        //     2);
        // m_renderer.BindConstantBuffer(
        //     BindTargets::PixelShader,
        //     m_perObjectCB->Get(),
        //     0);
        // m_renderer.BindConstantBuffer(
        //     BindTargets::PixelShader,
        //     m_perFrameCB->Get(),
        //     1);
        // m_renderer.BindConstantBuffer(
        //     BindTargets::PixelShader,
        //     m_perSceneCB->Get(),
        //     2);
        // m_renderer.BindShaderResource(
        //     BindTargets::PixelShader,
        //     m_shadowMap.GetDepthMapSRV(),
        //     4);
        // m_renderer.SetSamplerState(m_shadowMap.GetShadowSampler(), 1);
        // m_renderer.BindShaderResource(
        //     BindTargets::PixelShader,
        //     m_dynamicCubeMap.GetSRV(),
        //     6);

        // m_renderer.SetVertexBuffer(gVertexBuffer.Get(),
        //                            sizeof(VertexPositionNormalTangent),
        //                            0);
        // m_renderer.SetIndexBuffer(gIndexBuffer.Get(), 0);
        // m_renderer.DrawIndexed(gIndices.size(), 0, 0);

        // m_renderer.SetSamplerState(nullptr, 1);
        // m_renderer.BindShaderResource(BindTargets::PixelShader, nullptr, 6);
    }
    m_deviceResources->PIXEndEvent();

#if WITH_IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    m_renderer.Present();
}

void
Game::Tick()
{
    TimerTick(&m_timer);
    Update();
    Render();
}

void
Game::Initialize(HWND hWnd, uint32_t width, uint32_t height)
{
    using namespace Microsoft::WRL;
#ifdef MATH_TEST
    MathTest();
#endif
    m_deviceResources->SetWindow(hWnd, width, height);
    m_deviceResources->CreateDeviceResources();
    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
    TimerInitialize(&m_timer);
    Mouse::Get().SetWindowDimensions(m_deviceResources->GetBackBufferWidth(),
                                     m_deviceResources->GetBackBufferHeight());
    ID3D11Device *device = m_deviceResources->GetDevice();

    //    m_shaderManager.Initialize(
    //        device,
    //        SHADER_BINARY_ROOT,
    //        UtilsFormatStr("%s/shader", SHADER_SRC_ROOT));

    GLTFLoader loader(*m_deviceResources);
    m_meshes.push_back(std::move(
        loader.Load(R"(D:\Source\glTF-Sample-Models\2.0\Box\glTF\Box.gltf)")));

    m_basePass = std::make_unique<BasePass>(*m_deviceResources);
    m_basePass->SetCamera(m_camera);

    CreateDefaultSampler();
    // CreateRasterizerState();

    m_camera.SetZFar(10000);
    m_camera.SetZNear(10);

    m_renderer.SetDeviceResources(m_deviceResources.get());

    const int shadowMapSize = 2048;
    m_shadowMap.InitResources(
        m_deviceResources->GetDevice(), shadowMapSize, shadowMapSize);

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
            perObjectDesc, *m_deviceResources);

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
    // const std::string droidSansTtf = UtilsFormatStr(
    //     "%s/../imgui-1.87/misc/fonts/DroidSans.ttf",
    //     SOURCE_ROOT);
    // io.Fonts->AddFontFromFileTTF(droidSansTtf.c_str(), 16.0f);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_deviceResources->GetDevice(),
                        m_deviceResources->GetDeviceContext());
#endif
}

void
Game::GetDefaultSize(uint32_t *width, uint32_t *height)
{
    *width = DEFAULT_WIN_WIDTH;
    *height = DEFAULT_WIN_HEIGHT;
}

void
Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void
Game::CreateWindowSizeDependentResources()
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

void
Game::BuildShadowTransform(Mat4X4 &view, Mat4X4 &proj)
{
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