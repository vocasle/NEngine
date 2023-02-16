#pragma once
#include <string>

#include "NEngine/Renderer/Animation.h"
#include "NEngine/Renderer/Mesh.h"
#include "NEngine/Renderer/RenderModel.h"
#include "NEngine/Renderer/RenderNode.h"
#include "tinygltf/tiny_gltf.h"

namespace NEngine::Helpers {
class GLTFLoader
{
public:
    explicit GLTFLoader(DeviceResources &deviceResources);
    std::vector<NEngine::Renderer::Mesh> Load(const std::string &path);
    NEngine::Renderer::RenderModel load(const std::string &path);

private:
    void ProcessNode(const tinygltf::Node &node,
                     const tinygltf::Model &model,
                     std::vector<NEngine::Renderer::Mesh> &outMeshes);

    NEngine::Renderer::RenderNode process_node(const tinygltf::Node &node,
                                               const tinygltf::Model &model,
                                               size_t node_idx);

    NEngine::Renderer::Mesh ProcessMesh(const tinygltf::Node &node,
                                        const tinygltf::Mesh &mesh,
                                        const tinygltf::Model &model);

    NEngine::Renderer::MeshPrimitive ProcessMeshPrimitive(
        const tinygltf::Primitive &primitive, const tinygltf::Model &model);

    std::vector<unsigned int> ExtractMeshIndices(
        const tinygltf::Accessor &indexAccessor, const tinygltf::Model &model);

    NEngine::Renderer::Texture CreateTexture(
        const tinygltf::Model &model,
        size_t idx,
        NEngine::Renderer::TextureBindTarget bindTarget,
        unsigned int bindSlot);

    NEngine::Renderer::Animation parse_animation(
        const tinygltf::Model &model, const tinygltf::Animation &animation);

    std::vector<NEngine::Renderer::Animation> parse_animations(
        const tinygltf::Model &model);

    DeviceResources &m_deviceResources;
};
}  // namespace NEngine::Helpers
