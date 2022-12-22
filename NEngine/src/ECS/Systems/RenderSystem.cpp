#include "NEngine/ECS/Systems/RenderSystem.h"

namespace NEngine::ECS::Systems {

using namespace NEngine::Renderer;

RenderSystem::RenderSystem(NEngine::Helpers::DeviceResources &deviceResources)
    : mDeviceResources(&deviceResources),
      mBasePass(std::make_unique<BasePass>(deviceResources))
{
}

auto
RenderSystem::Update(float dt) -> void
{
}

}  // namespace NEngine::ECS::Systems