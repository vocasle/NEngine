#pragma once

#define _CRTDBG_MAP_ALLOC
#include <wrl/client.h>

#include <memory>
#include <utility>
#include <vector>

#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Utils/Timer.h"
#include "NEngine/Renderer/BasePass.h"
#include "NEngine/Game.h"
#include "NEngine/Engine.h"

#include "NEngine/ECS/EntityManager.h"
#include "NEngine/ECS/Components/PositionComponent.h"

class MyGame : public NEngine::Game {
public:
    MyGame();

    virtual auto Update(float dt) -> void override;
    virtual auto InitWithEngine(NEngine::Engine &engine) -> void override;

private:
    void Clear();
    void Update();
    void Render();

    void UpdateImgui();

    NEngine::Helpers::Camera m_camera;

    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> m_meshes;
    std::unique_ptr<NEngine::Renderer::BasePass> m_basePass;

    NEngine::Engine *mEngine;

    NEngine::ECS::EntityManager<NEngine::ECS::Components::PositionComponent>
        mEntityManager;
};
