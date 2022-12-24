#include "NEngine/ECS/Systems/InputSystem.h"

#include "NEngine/ECS/Components/InputComponent.h"
#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/Input/Keyboard.h"

namespace NEngine::ECS::Systems {

using namespace NEngine::Input;
using namespace NEngine::ECS::Components;

InputSystem::InputSystem(DefaultEntityManager &entityManager)
    : mEntityManager(&entityManager)
{
}

auto
InputSystem::Update(float dt) -> void
{
    const auto delta = dt * 0.004f;

    for (auto entity : mEntities) {
        auto &pc = *mEntityManager->GetComponent<PositionComponent>(entity);

        auto v = Velocity();
        if (Keyboard::Get().IsKeyDown('W')) {
            v.z = delta;
        }

        if (Keyboard::Get().IsKeyDown('S')) {
            v.z = -delta;
        }

        if (Keyboard::Get().IsKeyDown('A')) {
            v.x = delta;
        }

        if (Keyboard::Get().IsKeyDown('D')) {
            v.x = -delta;
        }
        pc.Velocity = v;
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
}  // namespace NEngine::ECS::Systems