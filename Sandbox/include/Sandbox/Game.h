#pragma once

#define _CRTDBG_MAP_ALLOC
#include <wrl/client.h>

#include <memory>
#include <utility>
#include <vector>

#include "NEngine/Helpers/AssetManager.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/DynamicConstBuffer.h"
#include "NEngine/Helpers/DynamicCubeMap.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Helpers/ParticleSystem.h"
#include "NEngine/Helpers/Renderer.h"
#include "NEngine/Helpers/ShaderManager.h"
#include "NEngine/Helpers/ShadowMap.h"
#include "NEngine/Renderer/Model.h"
#include "NEngine/Utils/Timer.h"

class Game {
public:
    Game();
    ~Game();

    void Tick();
    void Initialize(HWND hWnd, uint32_t width, uint32_t height);
    void GetDefaultSize(uint32_t *width, uint32_t *height);
    void OnWindowSizeChanged(int width, int height);

private:
    void CreateDefaultSampler();
    void Clear();
    void Update();
    void Render();
    void CreateRasterizerState();
    void CreateWindowSizeDependentResources();
    void DrawMeshes(const std::vector<NEngine::Helpers::Mesh> &meshes);
    void DrawActors();
    void BuildShadowTransform(NEngine::Math::Mat4X4 &view,
                              NEngine::Math::Mat4X4 &proj);

#if WITH_IMGUI
    void UpdateImgui();
#endif

    std::unique_ptr<NEngine::Helpers::DeviceResources> m_deviceResources;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_defaultSampler;
    NEngine::Utils::Timer m_timer;
    NEngine::Helpers::Camera m_camera;
    NEngine::Helpers::Renderer m_renderer;
    NEngine::Helpers::ShaderManager m_shaderManager;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
    std::unique_ptr<NEngine::Helpers::DynamicConstBuffer> m_perFrameCB;
    std::unique_ptr<NEngine::Helpers::DynamicConstBuffer> m_perSceneCB;
    std::unique_ptr<NEngine::Helpers::DynamicConstBuffer> m_perObjectCB;
    std::unique_ptr<NEngine::Helpers::DynamicConstBuffer> m_perPassCB;
    std::unique_ptr<NEngine::Helpers::AssetManager> m_assetManager;
    NEngine::Helpers::ParticleSystem m_firePS;
    NEngine::Helpers::ShadowMap m_shadowMap;
    NEngine::Helpers::DynamicCubeMap m_dynamicCubeMap;

    std::unique_ptr<NEngine::Renderer::Model> m_model;
};
