#pragma once

#include <vector>

#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/EntityManager.h"

namespace NEngine::ECS::Systems {

class InputSystem
{
public:
    explicit InputSystem(DefaultEntityManager &entityManager);
    auto Update(float dt) -> void;
    auto RegisterEntity(Entity entity) -> void;
    auto UnregisterEntity(Entity entity) -> void;
    ~InputSystem();

private:
    Math::Vec2D mPrevMousePos;
    Math::Vec2D mMousePos;
    struct Angles
    {
        float Yaw;
        float Pitch;
    } mAngles;
    bool mProcessInput;
    std::vector<Entity> mEntities;
    DefaultEntityManager *mEntityManager;
    Input::Mouse::MouseEventListener mMouseListener;
};
}  // namespace NEngine::ECS::Systems