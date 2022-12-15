#pragma once

#include <cstdint>

namespace NEngine::ECS {

enum class ComponentType : uint8_t {
    Mesh,
    Position
};

enum class SystemType : uint8_t {
    Render
};

typedef long Entity;

}  // namespace NEngine::ECS