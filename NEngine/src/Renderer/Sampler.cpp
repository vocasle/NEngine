#include "NEngine/Renderer/Sampler.h"

#include <d3d11.h>

#include <limits>

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;

namespace NEngine::Renderer {
void
Sampler::Bind(Helpers::DeviceResources &deviceResources)
{
    deviceResources.GetDeviceContext()->VSSetSamplers(
        mBindSlot, 1, mSamplerState.GetAddressOf());
    deviceResources.GetDeviceContext()->PSSetSamplers(
        mBindSlot, 1, mSamplerState.GetAddressOf());
}
void
Sampler::Unbind(Helpers::DeviceResources &deviceResources)
{
}

Sampler::Sampler(Helpers::DeviceResources &deviceResources,
                 const SamplerDescription &desc,
                 unsigned int bindSlot)
    : mDescription(desc),
      mBindSlot(bindSlot)
{
    const CD3D11_SAMPLER_DESC descTmp(
        static_cast<D3D11_FILTER>(mDescription.Filter),
        static_cast<D3D11_TEXTURE_ADDRESS_MODE>(mDescription.AddressU),
        static_cast<D3D11_TEXTURE_ADDRESS_MODE>(mDescription.AddressV),
        static_cast<D3D11_TEXTURE_ADDRESS_MODE>(mDescription.AddressW),
        mDescription.MipLODBias,
        mDescription.MaxAnisotropy,
        static_cast<D3D11_COMPARISON_FUNC>(mDescription.ComparisonFunc),
        reinterpret_cast<float *>(&mDescription.BorderColor),
        mDescription.MinLOD,
        mDescription.MaxLOD);

    HR(deviceResources.GetDevice()->CreateSamplerState(
        &descTmp, mSamplerState.ReleaseAndGetAddressOf()))
}

SamplerDescription::SamplerDescription()
    : Filter(SamplingFilter::MIN_MAG_MIP_LINEAR),
      AddressU(TextureAddressMode::Clamp),
      AddressV(TextureAddressMode::Clamp),
      AddressW(TextureAddressMode::Clamp),
      MipLODBias(0),
      MaxAnisotropy(1),
      ComparisonFunc(ComparisonFunc::NEVER),
      BorderColor(1, 1, 1, 1),
      MinLOD(-std::numeric_limits<float>::max()),
      MaxLOD(-std::numeric_limits<float>::max())
{
}
}  // namespace NEngine::Renderer