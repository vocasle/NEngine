#pragma once
#include <string>
#include <span>

#include "tinygltf/tiny_gltf.h"

#include "NEngine/Renderer/Model.h"

namespace NEngine {
namespace Helpers {
class GLTFLoader
{
public:
    explicit GLTFLoader(DeviceResources &deviceResources);
    std::unique_ptr<NEngine::Renderer::Model> Load(const std::string &path);

private:
    void
    ProcessNode(const tinygltf::Node &node,
                const tinygltf::Model &model);

    void ProcessMesh(const tinygltf::Mesh &mesh,
                     const tinygltf::Model &model);

    std::vector<unsigned int> ExtractMeshIndices(
        const tinygltf::Accessor &indexAccessor,
        const tinygltf::Model &model);


    tinygltf::TinyGLTF m_loader;
    std::unique_ptr<NEngine::Renderer::Model> m_model;
    DeviceResources &m_deviceResources;
};
}
}
