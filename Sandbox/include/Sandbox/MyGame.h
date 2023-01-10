#pragma once

#define _CRTDBG_MAP_ALLOC
#include <wrl/client.h>

#include <memory>
#include <utility>
#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/ECS/Components/RenderComponent.h"
#include "NEngine/ECS/EntityManager.h"
#include "NEngine/ECS/SystemManager.h"
#include "NEngine/Engine.h"
#include "NEngine/Game.h"
#include "NEngine/Helpers/Camera.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Scene.h"

class MyGame : public NEngine::Game
{
public:
    MyGame();

    virtual auto Update(float dt) -> void override;
    virtual auto InitWithEngine(NEngine::Engine &engine) -> void override;

private:
    void Update();
    void Render();

    void UpdateImgui();

    auto OnComponentAdd(NEngine::ECS::Entity entity) -> void;
    auto OnComponentRemove(NEngine::ECS::Entity entity) -> void;

    auto CreatePlayer() -> void;

    NEngine::Engine *mEngine;

    NEngine::ECS::DefaultEntityManager mEntityManager;

    NEngine::ECS::DefaultSystemManager mSystemManager;
    NEngine::Scene mScene;
};
