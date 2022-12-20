#include "NEngine/ECS/Systems/MoveSystem.h"

namespace NEngine::ECS::Systems {

auto
MoveSystem::Update(float dt) -> void
{
    for (auto &pc : *mComponents) {
        pc.Position.x += pc.Velocity.x;
        pc.Position.y += pc.Velocity.y;
        pc.Position.z += pc.Velocity.z;
    }
}

}  // namespace NEngine::ECS::Systems