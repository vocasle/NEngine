#include "NEngine/Renderer/Model.h"

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

const std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &
NEngine::Renderer::Model::GetMeshes() const
{
    return mMeshes;
}

NEngine::Renderer::Model::Model(
    DeviceResources &deviceResources,
    std::vector<std::unique_ptr<Renderer::Mesh>> &meshes)
    : mDeviceResources(deviceResources)
{
    for (auto &mesh : meshes) {
        mMeshes.push_back(std::move(mesh));
    }
}
