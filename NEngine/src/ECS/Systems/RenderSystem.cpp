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

auto
RenderSystem::Clear() -> void
{
    ID3D11DeviceContext *ctx = mDeviceResources->GetDeviceContext();
    ID3D11RenderTargetView *rtv = mDeviceResources->GetRenderTargetView();
    ID3D11DepthStencilView *dsv = mDeviceResources->GetDepthStencilView();

    static const float CLEAR_COLOR[4] = {
        0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f};

    ctx->Flush();

    ctx->ClearRenderTargetView(rtv, CLEAR_COLOR);
    ctx->ClearDepthStencilView(
        dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    ctx->OMSetRenderTargets(1, &rtv, dsv);
    ctx->RSSetViewports(1, &mDeviceResources->GetViewport());
}

auto
RenderSystem::RegisterEntity(Entity entity) -> void
{
}

auto
RenderSystem::UnregisterEntity(Entity entity) -> void
{
}

}  // namespace NEngine::ECS::Systems