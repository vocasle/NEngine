#pragma once

#include <vector>

#include "ECS/Components/PositionComponent.h"

namespace NEngine::ECS::Systems {
class MoveSystem
{
public:
    MoveSystem(const std::vector<Components::PositionComponent> &positions)
        : mComponents(&positions)
    {
    }

private:
    const std::vector<NEngine::ECS::Components::PositionComponent> *mComponents;
};
}  // namespace NEngine::ECS::Systems