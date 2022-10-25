#pragma once

#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine::Renderer {

enum class TextureBindTarget {
    ShaderResourceView,
    DepthStencilView,
    RenderTargetView,
};

class Texture : public Bindable
{
public:
    Texture(Helpers::DeviceResources &deviceResources,
            int bindSlot,
            TextureBindTarget bindTarget,
            const std::vector<uint8_t> &binaryBlob);
    Texture(Helpers::DeviceResources &deviceResources,
            int bindSlot,
            TextureBindTarget bindTarget,
            const std::string &imagePath);
};
}  // namespace NEngine::Renderer
