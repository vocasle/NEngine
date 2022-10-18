#include "NEngine/Renderer/Model.h"

#include "NEngine/Helpers/GLTFLoader.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

void
NEngine::Renderer::Model::Draw(Helpers::DeviceResources &deviceResources)
{
    for (const auto &mesh : mMeshes) {
        mesh->Draw(deviceResources);
    }
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
