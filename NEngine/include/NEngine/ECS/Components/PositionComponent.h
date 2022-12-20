#pragma once

namespace NEngine::ECS::Components {

struct Position
{
    Position(float x, float y, float z)
        : x(x),
          y(y),
          z(z)
    {
    }
    Position()
        : Position(0, 0, 0)
    {
    }
    float x;
    float y;
    float z;
};

struct Velocity
{
    Velocity(float x, float y, float z)
        : x(x),
          y(y),
          z(z)
    {
    }
    Velocity()
        : Velocity(0, 0, 0)
    {
    }
    float x;
    float y;
    float z;
};

struct PositionComponent
{
    struct Velocity Velocity;
    struct Position Position;
};

}  // namespace NEngine::ECS::Components
