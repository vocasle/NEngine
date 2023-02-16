#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DeviceResources.h"
#include "NEngine/Renderer/Mesh.h"
#include "NEngine/Renderer/RenderModel.h"

namespace NEngine::Helpers {
class ModelImporter
{
public:
    explicit ModelImporter(Helpers::DeviceResources &deviceResources);
    std::vector<NEngine::Renderer::Mesh> Load(const std::string &str) const;
    NEngine::Renderer::RenderModel load(const std::string &path) const;

private:
    Helpers::DeviceResources *mDeviceResources;
};
}  // namespace NEngine::Helpers