#pragma once

#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine {
namespace Renderer {
class Drawable
{
public:
    virtual void Draw(Helpers::DeviceResources &deviceResources) = 0;
    virtual ~Drawable() = default;
};
}
}
