#include "NEngine/Renderer/RasterizerState.h"

#include <combaseapi.h>
#include <d3d11.h>

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

namespace NEngine::Renderer {

RasterizerDescription::RasterizerDescription()
    : FillMode(FillMode::Solid),
      CullMode(CullMode::Back),
      FrontCounterClockwise(false),
      DepthBias(0),
      DepthBiasClamp(0),
      SlopeScaledDepthBias(0),
      DepthClipEnable(true),
      ScissorEnable(false),
      MultisampleEnable(false),
      AntialiasedLineEnable(false)
{
}

RasterizerState::RasterizerState(Helpers::DeviceResources &deviceResources,
                                 const RasterizerDescription &desc)
    : mDeviceResources(deviceResources),
      mRasterizerDescription(desc)
{
    UpdateRasterizerState();
}

void
RasterizerState::Bind(Helpers::DeviceResources &deviceResources)
{
    deviceResources.GetDeviceContext()->RSSetState(mRasterizerState.Get());
}

void
RasterizerState::Unbind(Helpers::DeviceResources &deviceResources)
{
}

void
RasterizerState::UpdateRasterizerState()
{
    CD3D11_RASTERIZER_DESC d3dDesc(
        static_cast<D3D11_FILL_MODE>(mRasterizerDescription.FillMode),
        static_cast<D3D11_CULL_MODE>(mRasterizerDescription.CullMode),
        mRasterizerDescription.FrontCounterClockwise,
        mRasterizerDescription.DepthBias,
        mRasterizerDescription.DepthBiasClamp,
        mRasterizerDescription.SlopeScaledDepthBias,
        mRasterizerDescription.DepthClipEnable,
        mRasterizerDescription.ScissorEnable,
        mRasterizerDescription.MultisampleEnable,
        mRasterizerDescription.AntialiasedLineEnable);

    HR(mDeviceResources.GetDevice()->CreateRasterizerState(
        &d3dDesc, mRasterizerState.ReleaseAndGetAddressOf()))
}

void
RasterizerState::SetFillMode(FillMode fillMode)
{
    mRasterizerDescription.FillMode = fillMode;
    UpdateRasterizerState();
}
void
RasterizerState::SetCullMode(CullMode cullMode)
{
    mRasterizerDescription.CullMode = cullMode;
    UpdateRasterizerState();
}
void
RasterizerState::SetFrontCounterClockwise(bool isFrontCounterClockwise)
{
    mRasterizerDescription.FrontCounterClockwise = isFrontCounterClockwise;
    UpdateRasterizerState();
}
void
RasterizerState::SetDepthBias(int depthBias)
{
    mRasterizerDescription.DepthBias = depthBias;
    UpdateRasterizerState();
}
void
RasterizerState::SetDepthBiasClamp(float depthBiasClamp)
{
    mRasterizerDescription.DepthBiasClamp = depthBiasClamp;
    UpdateRasterizerState();
}
void
RasterizerState::SetSlopeScaledDepthBias(float slopeScaledDepthBias)
{
    mRasterizerDescription.SlopeScaledDepthBias = slopeScaledDepthBias;
    UpdateRasterizerState();
}
void
RasterizerState::SetDepthClipEnable(bool isDepthClipEnabled)
{
    mRasterizerDescription.DepthClipEnable = isDepthClipEnabled;
    UpdateRasterizerState();
}
void
RasterizerState::SetScissorEnable(bool isScissorEnable)
{
    mRasterizerDescription.ScissorEnable = isScissorEnable;
    UpdateRasterizerState();
}
void
RasterizerState::SetMultisampleEnable(bool isMultisampleEnabled)
{
    mRasterizerDescription.MultisampleEnable = isMultisampleEnabled;
    UpdateRasterizerState();
}
void
RasterizerState::SetAntialisedLineEnable(bool isAntialisedLineEnabled)
{
    mRasterizerDescription.AntialiasedLineEnable = isAntialisedLineEnabled;
    UpdateRasterizerState();
}
}  // namespace NEngine::Renderer