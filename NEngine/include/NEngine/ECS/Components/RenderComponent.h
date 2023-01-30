#pragma once

#include <memory>
#include <vector>

#include "NEngine/Renderer/Mesh.h"

namespace NEngine::ECS::Components {
struct RenderComponent
{
    std::vector<NEngine::Renderer::Mesh> Mesh;
};
}  // namespace NEngine::ECS::Components