#pragma once

#include <memory>
#include <vector>

#include "NEngine/Renderer/RenderModel.h"

namespace NEngine::ECS::Components {
struct RenderComponent
{
    NEngine::Renderer::RenderModel Model;
};
}  // namespace NEngine::ECS::Components