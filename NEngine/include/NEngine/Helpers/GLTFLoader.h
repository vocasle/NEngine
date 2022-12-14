#pragma once
#include <span>
#include <string>

#include "NEngine/Renderer/Mesh.h"
#include "tinygltf/tiny_gltf.h"

namespace NEngine::Helpers {
class GLTFLoader
{
public:
    explicit GLTFLoader(DeviceResources &deviceResources);
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> Load(
        const std::string &path);

private:
    void ProcessNode(
        const tinygltf::Node &node,
        const tinygltf::Model &model,
        std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &outMeshes);

    std::unique_ptr<NEngine::Renderer::Mesh> ProcessMesh(
        const tinygltf::Mesh &mesh, const tinygltf::Model &model);

    std::unique_ptr<NEngine::Renderer::MeshPrimitive> ProcessMeshPrimitive(
        const tinygltf::Primitive &primitive, const tinygltf::Model &model);

    std::vector<unsigned int> ExtractMeshIndices(
        const tinygltf::Accessor &indexAccessor, const tinygltf::Model &model);

    std::unique_ptr<NEngine::Renderer::Texture> CreateTexture(
        const tinygltf::Model &model,
        size_t idx,
        NEngine::Renderer::TextureBindTarget bindTarget,
        unsigned int bindSlot);

    DeviceResources &m_deviceResources;
};
}  // namespace NEngine::Helpers
