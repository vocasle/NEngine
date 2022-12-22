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
    for (auto entity : mEntities) {
        auto &pc = *mEntityManager->GetComponent<PositionComponent>(entity);

        if (Keyboard::Get().IsKeyDown('W')) {
            pc.Velocity.z = 0.1f;
        }
        else {
            pc.Velocity.z = 0;
        }
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