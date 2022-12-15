#pragma once

namespace NEngine {
class Engine;
class Game
{
public:
    virtual ~Game() = default;
    virtual auto Update(float dt) -> void = 0;
    virtual auto InitWithEngine(NEngine::Engine &engine) -> void = 0;
};
}  // namespace NEngine