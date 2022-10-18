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

void
NEngine::Renderer::Model::AddMesh(std::unique_ptr<Mesh> mesh)
{
    mMeshes.push_back(std::move(mesh));
}
