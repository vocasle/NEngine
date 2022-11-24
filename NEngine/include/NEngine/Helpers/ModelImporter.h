#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DeviceResources.h"
#include "NEngine/Renderer/Mesh.h"

namespace NEngine::Helpers {
class ModelImporter
{
public:
    explicit ModelImporter(Helpers::DeviceResources &deviceResources);
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> Load(
        const std::string &str);

private:
    Helpers::DeviceResources *mDeviceResources;
};
}  // namespace NEngine::Helpers