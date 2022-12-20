#include "NEngine/ECS/Systems/MoveSystem.h"

namespace NEngine::ECS::Systems {

auto
MoveSystem::Update(float dt) -> void
{
    for (auto &pc : *mComponents) {
        pc.Component.Position.x += pc.Component.Velocity.x;
        pc.Component.Position.y += pc.Component.Velocity.y;
        pc.Component.Position.z += pc.Component.Velocity.z;
    }
}

}  // namespace NEngine::ECS::Systems