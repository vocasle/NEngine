#include "NEngine/Helpers/GLTFLoader.h"

#include <algorithm>

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
    const size_t byteOffset = indexAccessor.byteOffset + bufferView.byteOffset;
    indices.reserve(indexAccessor.count);

    for (size_t i = 0; i < indexAccessor.count; ++i) {
        if (indexAccessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            indices.push_back(*reinterpret_cast<const unsigned short *>(
                &buffer.data.at(i * byteStride + byteOffset)));
        }
        else if (indexAccessor.componentType ==
                 TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            indices.push_back(*reinterpret_cast<const unsigned int *>(
                &buffer.data.at(i * byteStride + byteOffset)));
        }
        else {
            UtilsDebugPrint("ERROR: Unexpected byte length of index type.\n");
        }
    }

    return indices;
}

template <typename T>
std::vector<T>
ReadData(const std::vector<unsigned char> &data, size_t count, size_t offset)
{
    std::vector<T> vec;
    vec.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        vec.push_back(*(reinterpret_cast<const T *>(&data[0] + offset) + i));
    }
    return vec;
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
    Material tmpMaterial;

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
            const size_t byteOffset = accessor.byteOffset;

            assert(bufferView.byteLength - byteOffset >=
                   byteStride * accessor.count);

            if (name == "POSITION") {
                assert(sizeof(Vec3D) == byteStride);
                positions =
                    ReadData<Vec3D>(buffer.data, accessor.count, byteOffset);
            }
            else if (name == "NORMAL") {
                assert(sizeof(Vec3D) == byteStride);
                normals =
                    ReadData<Vec3D>(buffer.data, accessor.count, byteOffset);
            }
            else if (name == "TANGENT") {
                assert(sizeof(Vec4D) == byteStride);
                tangents =
                    ReadData<Vec4D>(buffer.data, accessor.count, byteOffset);
            }
            else if (name == "TEXCOORD_0") {
                assert(sizeof(Vec2D) == byteStride);
                texCoords =
                    ReadData<Vec2D>(buffer.data, accessor.count, byteOffset);
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

        assert(primitive.material >= 0 &&
               "ERROR: gLTF model does not contain material!");
        const auto &material = model.materials[primitive.material];

        tmpMaterial.BaseColor = {
            static_cast<float>(
                material.pbrMetallicRoughness.baseColorFactor[0]),
            static_cast<float>(
                material.pbrMetallicRoughness.baseColorFactor[1]),
            static_cast<float>(
                material.pbrMetallicRoughness.baseColorFactor[2]),
            static_cast<float>(
                material.pbrMetallicRoughness.baseColorFactor[3])};

        tmpMaterial.Roughness = material.pbrMetallicRoughness.roughnessFactor;
        tmpMaterial.Metalness = material.pbrMetallicRoughness.metallicFactor;
        if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
            UtilsDebugPrint(
                "WARN: Extract baseColorTexture is not yet implemented");
        }
        if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
            UtilsDebugPrint(
                "WARN: Extract metallicRoughnessTexture is not yet "
                "implemented");
        }

        // const auto baseColorTextureIdx =
        // material.pbrMetallicRoughness.baseColorTexture.index; const auto
        // metallicRoughnessTextureIdx =
        // material.pbrMetallicRoughness.metallicRoughnessTexture.index; const
        // auto &tex = model.textures[metallicRoughnessTextureIdx]; const auto
        // &image = model.images[tex.source];
    }

    std::vector<Renderer::VertexPositionNormalTangent> vertices;

    const auto max = std::max_element(std::begin(indices), std::end(indices));

    if (!normals.empty()) {
        assert(normals.size() > *max);
    }
    else {
        normals.resize(positions.size(), {0, 0, 0});
    }

    if (!tangents.empty()) {
        assert(tangents.size() > *max);
    }
    else {
        tangents.resize(positions.size(), {0, 0, 0, 0});
    }

    if (!texCoords.empty()) {
        assert(texCoords.size() > *max);
    }
    else {
        texCoords.resize(positions.size(), {0, 0});
    }

    assert(positions.size() == normals.size() &&
           positions.size() == tangents.size() &&
           positions.size() == texCoords.size());

    vertices.reserve(positions.size());
    for (size_t idx = 0; idx < positions.size(); ++idx) {
        const Vec2D &texCoord = texCoords[idx];
        const Vec4D normal = Vec4D(normals[idx], texCoord.Y);
        const Vec4D &tangent = tangents[idx];
        const Vec4D position = Vec4D(positions[idx], texCoord.X);
        vertices.push_back({position, normal, tangent});
    }

    assert(!vertices.empty() && "Vertices is empty! gLTF import failed!");
    assert(!indices.empty() && "Indices is empty! gLTF import failed!");

    auto meshPrim = std::make_unique<Renderer::MeshPrimitive>(
        m_deviceResources, vertices, indices);
    meshPrim->SetMaterial(std::move(tmpMaterial));
    return std::move(meshPrim);
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
