#pragma once
#include <memory>
#include <vector>

#include "Mesh.h"
#include "NEngine/Helpers/Transform.h"

namespace NEngine::Renderer {
struct RenderNode
{
    std::vector<RenderNode> children;
    Helpers::Transform transform;
    std::string name;
    Mesh mesh;
    size_t id = 0;
};
}  // namespace NEngine::Renderer
