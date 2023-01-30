#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine::Renderer {

enum class FillMode {
    Solid = D3D11_FILL_SOLID,
    WireFrame = D3D11_FILL_WIREFRAME
};

enum class CullMode {
    Back = D3D11_CULL_BACK,
    Front = D3D11_CULL_FRONT,
    None = D3D11_CULL_NONE
};

struct RasterizerDescription
{
    RasterizerDescription();
    FillMode FillMode;
    CullMode CullMode;
    bool FrontCounterClockwise;
    int DepthBias;
    float DepthBiasClamp;
    float SlopeScaledDepthBias;
    bool DepthClipEnable;
    bool ScissorEnable;
    bool MultisampleEnable;
    bool AntialiasedLineEnable;
};

class RasterizerState : public Bindable
{
public:
    RasterizerState(Helpers::DeviceResources &deviceResources,
                    const RasterizerDescription &desc);
    virtual void Bind(Helpers::DeviceResources &deviceResources) const override;
    virtual void Unbind(
        Helpers::DeviceResources &deviceResources) const override;

    void SetFillMode(FillMode fillMode);
    void SetCullMode(CullMode cullMode);
    void SetFrontCounterClockwise(bool isFrontCounterClockwise);
    void SetDepthBias(int depthBias);
    void SetDepthBiasClamp(float depthBiasClamp);
    void SetSlopeScaledDepthBias(float slopeScaledDepthBias);
    void SetDepthClipEnable(bool isDepthClipEnabled);
    void SetScissorEnable(bool isScissorEnable);
    void SetMultisampleEnable(bool isMultisampleEnabled);
    void SetAntialisedLineEnable(bool isAntialisedLineEnabled);

private:
    void UpdateRasterizerState();

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
    Helpers::DeviceResources &mDeviceResources;
    RasterizerDescription mRasterizerDescription;
};
}  // namespace NEngine::Renderer