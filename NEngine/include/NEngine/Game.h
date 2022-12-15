#pragma once

namespace NEngine {
class Game
{
public:
    virtual ~Game() = default;
    virtual auto Update(float dt) -> void = 0;
};
}  // namespace NEngine