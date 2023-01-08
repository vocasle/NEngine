#include "NEngine/ECS/Systems/InputSystem.h"

#include "NEngine/ECS/Components/InputComponent.h"
#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/Engine.h"
#include "NEngine/Input/Keyboard.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/Vec2D.h"

namespace NEngine::ECS::Systems {

using namespace NEngine::Input;
using namespace NEngine::ECS::Components;
using namespace NEngine::Math;
using namespace NEngine;

InputSystem::InputSystem(DefaultEntityManager &entityManager)
    : mEntityManager(&entityManager),
      mMouseListener(),
      mProcessInput(false)
{
    const auto winSize = Engine::GEngine->GetWindowSize();
    mPrevMousePos.X = winSize.X / 2;
    mPrevMousePos.Y = winSize.Y / 2;

    mMouseListener.MouseMoveCallback =
        [this](const Math::Vec2D &position) -> void { mMousePos = position; };
    mMouseListener.MouseDownCallback = [this](const Math::Vec2D &position,
                                              Mouse::ButtonType btnType) -> void
    {
        if (btnType == Mouse::ButtonType::Right) {
            mProcessInput = true;
        }
    };

    mMouseListener.MouseUpCallback = [this](const Math::Vec2D &position,
                                            Mouse::ButtonType btnType) -> void
    {
        if (btnType == Mouse::ButtonType::Right) {
            mProcessInput = false;
        }
    };

    Mouse::Get().SetMouseEventListener(mMouseListener);
}

auto
InputSystem::Update(float dt) -> void
{
    const auto delta = dt * 0.004f;

    if (mProcessInput && mPrevMousePos != mMousePos) {
        auto xoffset = mMousePos.X - mPrevMousePos.X;
        auto yoffset = mPrevMousePos.Y - mMousePos.Y;
        xoffset *= dt * 0.02f;
        yoffset *= dt * 0.02f;

        mAngles.Pitch += yoffset;
        mAngles.Yaw += xoffset;
        UTILS_PRINTLN("prev_pos=%s, pos=%s",
                      mPrevMousePos.ToString().c_str(),
                      mMousePos.ToString().c_str());
        UTILS_PRINTLN("xoffset=%f", xoffset);
        UTILS_PRINTLN("yaw=%f", mAngles.Yaw);

        if (mAngles.Pitch > 89.0f)
            mAngles.Pitch = 89.0f;
        if (mAngles.Pitch < -89.0f)
            mAngles.Pitch = -89.0f;

        mPrevMousePos = mMousePos;
    }

    auto v = Vec3D();
    if (Keyboard::Get().IsKeyDown('W')) {
        v.Z += delta;
    }

    if (Keyboard::Get().IsKeyDown('S')) {
        v.Z += -delta;
    }

    if (Keyboard::Get().IsKeyDown('A')) {
        v.X += delta;
    }

    if (Keyboard::Get().IsKeyDown('D')) {
        v.X += -delta;
    }

    for (auto entity : mEntities) {
        auto &pc = *mEntityManager->GetComponent<PositionComponent>(entity);

        pc.Velocity = v;
        pc.Pitch = mAngles.Pitch;
        pc.Yaw = mAngles.Yaw;
    }
}

auto
InputSystem::RegisterEntity(Entity entity) -> void
{
    if (mEntityManager->HasComponent<PositionComponent>(entity) &&
        mEntityManager->HasComponent<InputComponent>(entity)) {
        mEntities.push_back(entity);
    }
}

auto
InputSystem::UnregisterEntity(Entity entity) -> void
{
    if (mEntityManager->HasComponent<PositionComponent>(entity) &&
        mEntityManager->HasComponent<InputComponent>(entity)) {
        auto it = std::find(std::begin(mEntities), std::end(mEntities), entity);
        if (it != std::end(mEntities)) {
            mEntities.erase(it);
        }
    }
}
InputSystem::~InputSystem()
{
    Mouse::Get().RemoveMouseEventListener(mMouseListener);
}

}  // namespace NEngine::ECS::Systems