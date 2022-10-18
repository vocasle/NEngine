#pragma once
#include <string>
#include <span>

#include "tinygltf/tiny_gltf.h"

#include "NEngine/Renderer/Model.h"
#include "NEngine/Renderer/Mesh.h"

namespace NEngine::Helpers {
class GLTFLoader
{
public:
    explicit GLTFLoader(DeviceResources &deviceResources);
    std::unique_ptr<NEngine::Renderer::Model> Load(const std::string &path);

private:
    void
    ProcessNode(const tinygltf::Node &node,
                const tinygltf::Model &model,
                std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &outMeshes);

    std::unique_ptr<NEngine::Renderer::Mesh> ProcessMesh(const tinygltf::Mesh &mesh,
                     const tinygltf::Model &model);

    std::vector<unsigned int> ExtractMeshIndices(
        const tinygltf::Accessor &indexAccessor,
        const tinygltf::Model &model);

    DeviceResources &m_deviceResources;
};
}
