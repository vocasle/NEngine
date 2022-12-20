#pragma once

#include <vector>

#include "NEngine/ECS/Components/PositionComponent.h"
#include "System.h"

namespace NEngine::ECS::Systems {
class MoveSystem : public System
{
public:
    MoveSystem(std::vector<Components::PositionComponent> &positions)
        : mComponents(&positions)
    {
    }

    virtual auto Update(float dt) -> void override;

private:
    std::vector<NEngine::ECS::Components::PositionComponent> *mComponents;
};
}  // namespace NEngine::ECS::Systems