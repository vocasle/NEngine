#include "NEngine/Helpers/GLTFLoader.h"

#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/MeshPrimitive.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;
using namespace NEngine::Math;
using namespace NEngine::Renderer;

void
GLTFLoader::ProcessNode(
    const tinygltf::Node &node,
    const tinygltf::Model &model,
    std::vector<std::unique_ptr<NEngine::Renderer::MeshPrimitive>>
        &outMeshPrimitives)
{
    if (node.mesh >= 0) {
        auto meshPrimitive =
            ProcessMeshPrimitive(model.meshes[node.mesh], model);
        outMeshPrimitives.push_back(std::move(meshPrimitive));
    }

    for (const auto childIdx : node.children) {
        ProcessNode(model.nodes[childIdx], model, outMeshPrimitives);
    }
}

std::vector<unsigned int>
GLTFLoader::ExtractMeshIndices(const tinygltf::Accessor &indexAccessor,
                               const tinygltf::Model &model)
{
    std::vector<unsigned int> indices;
    const auto &bufferView = model.bufferViews[indexAccessor.bufferView];
    const auto &buffer = model.buffers[bufferView.buffer];

    const size_t byteStride = indexAccessor.ByteStride(bufferView);
    indices.reserve(indexAccessor.count);

    for (size_t i = bufferView.byteOffset; i < buffer.data.size();
         i += byteStride) {
        if (indexAccessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            indices.push_back(
                *reinterpret_cast<const unsigned short *>(&buffer.data.at(i)));
        }
        else if (indexAccessor.componentType ==
                 TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            indices.push_back(
                *reinterpret_cast<const unsigned int *>(&buffer.data.at(i)));
        }
        else {
            UtilsDebugPrint("ERROR: Unexpected byte length of index type.\n");
        }
    }

    return indices;
}

template <typename Vec>
Vec
ReadNextVec(const std::vector<unsigned char> &data, size_t idx)
{
    Vec v = *reinterpret_cast<const Vec *>(data.data() + idx);
    return v;
}

std::unique_ptr<NEngine::Renderer::MeshPrimitive>
GLTFLoader::ProcessMeshPrimitive(const tinygltf::Mesh &mesh,
                                 const tinygltf::Model &model)
{
    std::vector<unsigned int> indices;
    std::vector<Math::Vec3D> positions;
    std::vector<Math::Vec3D> normals;
    std::vector<Math::Vec4D> tangents;
    std::vector<Math::Vec2D> texCoords;

    assert(mesh.primitives.size() == 1 &&
           "Mesh contains more than one primitive!");

    for (const auto &primitive : mesh.primitives) {
        const auto &indexAccessor = model.accessors[primitive.indices];

        indices = ExtractMeshIndices(indexAccessor, model);

        for (const auto &[name, accessorIdx] : primitive.attributes) {
            const auto &accessor = model.accessors[accessorIdx];
            const auto &bufferView = model.bufferViews[accessor.bufferView];
            const auto &buffer = model.buffers[bufferView.buffer];
            const size_t byteStride = accessor.ByteStride(bufferView);
            if (name == "POSITION") {
                assert(sizeof(Vec3D) == byteStride);
                assert(bufferView.byteLength >= sizeof(Vec3D) * accessor.count);
                positions.reserve(accessor.count);

                for (size_t i = bufferView.byteOffset;
                     i < bufferView.byteLength;
                     i += byteStride) {
                    positions.push_back(ReadNextVec<Vec3D>(buffer.data, i));
                }
            }
            else if (name == "NORMAL") {
                assert(sizeof(Vec3D) == byteStride);
                assert(bufferView.byteLength >= sizeof(Vec3D) * accessor.count);
                normals.reserve(accessor.count);

                for (size_t i = bufferView.byteOffset;
                     i < bufferView.byteLength;
                     i += byteStride) {
                    normals.push_back(ReadNextVec<Vec3D>(buffer.data, i));
                }
            }
            else if (name == "TANGENT") {
                assert(sizeof(Vec4D) == byteStride);
                assert(bufferView.byteLength >= sizeof(Vec4D) * accessor.count);
                tangents.reserve(accessor.count);

                for (size_t i = bufferView.byteOffset;
                     i < bufferView.byteLength;
                     i += byteStride) {
                    tangents.push_back(ReadNextVec<Vec4D>(buffer.data, i));
                }
            }
            else if (name == "TEXCOORD_0") {
                assert(sizeof(Vec2D) == byteStride);
                assert(bufferView.byteLength >= sizeof(Vec2D) * accessor.count);
                texCoords.reserve(accessor.count);

                for (size_t i = bufferView.byteOffset;
                     i < bufferView.byteLength;
                     i += byteStride) {
                    texCoords.push_back(ReadNextVec<Vec2D>(buffer.data, i));
                }
            }
            else if (name == "COLOR_0") {
                UtilsDebugPrint("WARN: COLOR_0 is not supported yet\n");
            }
            else if (name == "JOINTS_0") {
                UtilsDebugPrint("WARN: JOINTS_0 is not supported yet\n");
            }
            else if ("WEIGHTS_0") {
                UtilsDebugPrint("WARN: WEIGHTS_0 is not supported yet\n");
            }
        }

        // TODO: Extract textures from gLTF file
        // const auto &material = model.materials[primitive.material];
        // const auto baseColorTextureIdx =
        // material.pbrMetallicRoughness.baseColorTexture.index; const auto
        // metallicRoughnessTextureIdx =
        // material.pbrMetallicRoughness.metallicRoughnessTexture.index; const
        // auto &tex = model.textures[metallicRoughnessTextureIdx]; const auto
        // &image = model.images[tex.source];
    }

    std::vector<Renderer::VertexPositionNormalTangent> vertices;

    if (!normals.empty()) {
        assert(normals.size() > indices.size() - 1);
    }

    if (!tangents.empty()) {
        assert(tangents.size() > indices.size() - 1);
    }

    for (const unsigned int idx : indices) {
        const Vec4D normal =
            normals.empty() ? Vec4D{0, 0, 0, 0} : Vec4D{normals[idx], 0};
        const Vec4D tangent =
            tangents.empty() ? Vec4D{0, 0, 0, 0} : tangents[idx];

        vertices.push_back({{positions[idx], 1}, normal, tangent});
    }

    assert(!vertices.empty() && "Vertices is empty! gLTF import failed!");
    assert(!indices.empty() && "Indices is empty! gLTF import failed!");

    return std::make_unique<Renderer::MeshPrimitive>(
        m_deviceResources, vertices, indices);
}

GLTFLoader::GLTFLoader(DeviceResources &deviceResources)
    : m_deviceResources(deviceResources)
{
}

std::unique_ptr<NEngine::Renderer::Mesh>
GLTFLoader::Load(const std::string &path)
{
    tinygltf::Model model;

    std::string err;
    std::string warn;

    tinygltf::TinyGLTF loader;

    const bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

    if (!err.empty()) {
        UtilsDebugPrint("ERROR: Failed to load model: %s\n", err.c_str());
        return nullptr;
    }
    if (!warn.empty()) {
        UtilsDebugPrint("WARN: %s\n", warn.c_str());
        return nullptr;
    }
    if (!ret) {
        UtilsDebugPrint("ERROR: Failed to parse gLTF\n");
        return nullptr;
    }

    const auto &scene = model.scenes[model.defaultScene];

    std::vector<std::unique_ptr<Renderer::MeshPrimitive>> meshPrimitives;

    for (const auto nodeIdx : scene.nodes) {
        ProcessNode(model.nodes[nodeIdx], model, meshPrimitives);
    }

    std::unique_ptr<Renderer::Mesh> modelTmp = std::make_unique<Renderer::Mesh>(
        m_deviceResources, std::move(meshPrimitives));

    return std::move(modelTmp);
}
