#pragma once

#include <memory>
#include <vector>

#include "NEngine/Renderer/Mesh.h"

namespace NEngine::ECS::Components {
struct RenderComponent
{
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> Mesh;
};
}  // namespace NEngine::ECS::Components