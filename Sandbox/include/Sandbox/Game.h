#pragma once

#define _CRTDBG_MAP_ALLOC
#include <wrl/client.h>

#include <memory>
#include <utility>
#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/ECS/Components/RenderComponent.h"
#include "NEngine/ECS/EntityManager.h"
#include "NEngine/ECS/System.h"
#include "NEngine/Engine.h"
#include "NEngine/Game.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/LightHelper.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/BasePass.h"
#include "NEngine/Utils/Timer.h"

class MyGame : public NEngine::Game
{
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

    std::unique_ptr<NEngine::Renderer::BasePass> m_basePass;

    NEngine::Engine *mEngine;

    NEngine::ECS::EntityManager<NEngine::ECS::Components::PositionComponent,
                                NEngine::ECS::Components::RenderComponent>
        mEntityManager;

    std::vector<NEngine::ECS::Entity> mEntities;
    std::vector<std::unique_ptr<NEngine::ECS::System>> mSystems;
};
