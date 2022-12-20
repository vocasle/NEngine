#pragma once

#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"
#include "NEngine/ECS/Entity.h"
#include "NEngine/ECS/System.h"

namespace NEngine::ECS::Systems {
class MoveSystem : public NEngine::ECS::System
{
public:
    MoveSystem(
        std::vector<ComponentData<Components::PositionComponent>> &positions)
        : mComponents(&positions)
    {
    }

    virtual auto Update(float dt) -> void override;

private:
    std::vector<ComponentData<Components::PositionComponent>> *mComponents;
};
}  // namespace NEngine::ECS::Systems