#pragma once

namespace NEngine::ECS::Systems {

class System
{
public:
    virtual ~System() = default;
    virtual auto Update(float dt) -> void = 0;
};
}  // namespace NEngine::ECS::Systems