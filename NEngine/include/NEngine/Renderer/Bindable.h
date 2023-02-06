#pragma once

#include <limits>

#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine {
namespace Renderer {
class Bindable
{
public:
    virtual void Bind(Helpers::DeviceResources &deviceResources) const = 0;
    virtual void Unbind(Helpers::DeviceResources &deviceResources) const = 0;
    virtual ~Bindable() = default;

    static constexpr unsigned int INVALID_BIND_SLOT =
        std::numeric_limits<unsigned int>::max();
};
}  // namespace Renderer
}  // namespace NEngine
