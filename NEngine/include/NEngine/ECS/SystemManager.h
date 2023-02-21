#pragma once

#include "Systems/AnimationSystem.h"
#include "Systems/AudioSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MoveSystem.h"
#include "Systems/RenderSystem.h"

namespace NEngine::ECS {

template <typename... System>
class SystemManager
{
public:
    template <typename System>
    using SystemPtr = std::unique_ptr<System>;

    using tuple_t = std::tuple<SystemPtr<System>...>;

    template <typename System>
    [[nodiscard]] auto
    GetSystem() -> System &
    {
        return *std::get<SystemPtr<System>>(mSystems);
    }

    template <typename System>
    [[nodiscard]] auto
    SetSystem(SystemPtr<System> system) -> void
    {
        std::get<SystemPtr<System>>(mSystems) = std::move(system);
    }

    // Taken from https://stackoverflow.com/a/54641400/3846281
    constexpr auto
    Update(float dt) -> void
    {
        std::apply([&](const auto &...e) { (e->Update(dt), ...); }, mSystems);
    }

    auto
    RegisterEntity(Entity entity) -> void
    {
        std::apply([&](const auto &...e) { (e->RegisterEntity(entity), ...); },
                   mSystems);
    }

    auto
    UnregisterEntity(Entity entity) -> void
    {
        std::apply([&](const auto &...e)
                   { (e->UnregisterEntity(entity), ...); },
                   mSystems);
    }

private:
    tuple_t mSystems;
};

using DefaultSystemManager = SystemManager<Systems::RenderSystem,
                                           Systems::MoveSystem,
                                           Systems::InputSystem,
                                           Systems::CollisionSystem,
                                           Systems::AudioSystem,
                                           Systems::AnimationSystem>;

}  // namespace NEngine::ECS