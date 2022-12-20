#pragma once

#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"

namespace NEngine::ECS::Systems {
class MoveSystem
{
public:
    MoveSystem(std::vector<Components::PositionComponent> &positions)
        : mComponents(&positions)
    {
    }

    void Update(float dt);

private:
    std::vector<NEngine::ECS::Components::PositionComponent> *mComponents;
};
}  // namespace NEngine::ECS::Systems