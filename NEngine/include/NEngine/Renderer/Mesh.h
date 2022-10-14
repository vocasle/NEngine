#pragma once

#include <memory>
#include <vector>

#include "Drawable.h"
#include "Bindable.h"
#include "NEngine/Helpers/DeviceResources.h"


namespace NEngine {
namespace Renderer {
class Mesh : public Drawable
{
public:
    explicit Mesh(Helpers::DeviceResources& deviceResources);
    void Draw(Helpers::DeviceResources &deviceResources) override;

private:
    std::vector<std::unique_ptr<Bindable>> mBinds;


};
}
}
