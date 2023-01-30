//
// Created by vocasle on 10/19/2022.
//

#ifndef NENGINE_PIXELSHADER_H
#define NENGINE_PIXELSHADER_H

#include <d3d11.h>
#include <wrl/client.h>

#include <string>

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine::Renderer {
class PixelShader : public Bindable
{
public:
    PixelShader(Helpers::DeviceResources &deviceResources,
                const std::string &path);
    void Bind(Helpers::DeviceResources &deviceResources) const override;
    void Unbind(Helpers::DeviceResources &deviceResources) const override;

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
};
}  // namespace NEngine::Renderer

#endif  // NENGINE_PIXELSHADER_H
