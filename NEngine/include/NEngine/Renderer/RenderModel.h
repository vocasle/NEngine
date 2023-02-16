#pragma once
#include "Animation.h"
#include "RenderNode.h"

namespace NEngine::Renderer {
struct RenderModel
{
    std::vector<RenderNode> nodes;
    std::vector<Animation> animations;
};
}  // namespace NEngine::Renderer
