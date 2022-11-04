#include "NEngine/Helpers/GLTFLoader.h"

#include <algorithm>

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/Transform.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/MeshPrimitive.h"
#include "NEngine/Renderer/Sampler.h"
#include "NEngine/Renderer/Texture.h"
#include "NEngine/Utils/Utils.h"
#include "tinygltf/tiny_gltf.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;
using namespace NEngine::Math;
using namespace NEngine::Renderer;

Transform
GetNodeTransform(const tinygltf::Node &node)
{
    auto t = Transform();

    if (!node.matrix.empty()) {
        assert("Setting matrix transform from gLTF is not yet implemented");
    }
    else {
        if (!node.rotation.empty()) {
            const auto rot =
                MathQuaternionToRotationMat(Vec4D(node.rotation[0],
                                                  node.rotation[1],
                                                  node.rotation[2],
                                                  node.rotation[3]));

            t.SetRotation(rot);
        }
        if (!node.translation.empty()) {
            const Vec3D offset(
                node.translation[0], node.translation[1], node.translation[2]);
            t.SetTranslation(MathMat4X4TranslateFromVec3D(&offset));
        }
        if (!node.scale.empty()) {
            const Vec3D scale(node.scale[0], node.scale[1], node.scale[2]);
            t.SetScale(MathMat4X4ScaleFromVec3D(&scale));
        }
    }

    return t;
}

void
GLTFLoader::ProcessNode(
    const tinygltf::Node &node,
    const tinygltf::Model &model,
    std::vector<std::unique_ptr<NEngine::Renderer::MeshPrimitive>>
        &outMeshPrimitives,
    Helpers::Transform &outTransform)
{
    if (node.mesh >= 0) {
        outTransform = GetNodeTransform(node);
        auto meshPrimitive =
            ProcessMeshPrimitive(model.meshes[node.mesh], model);
        outMeshPrimitives.push_back(std::move(meshPrimitive));
    }

    for (const auto childIdx : node.children) {
        ProcessNode(
            model.nodes[childIdx], model, outMeshPrimitives, outTransform);
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

std::unique_ptr<Texture>
GLTFLoader::CreateTexture(const tinygltf::Model &model,
                          size_t idx,
                          TextureBindTarget bindTarget,
                          unsigned int bindSlot)
{
    const auto &texInfo = model.textures[idx];
    SamplerDescription samDesc =
        SamplerDescription::CreateGLTFDefaultSamplerDesc();
    samDesc.Filter = SamplingFilter::MIN_LINEAR_MAG_MIP_POINT;
    if (texInfo.sampler >= 0) {
        const auto &sampler = model.samplers[texInfo.sampler];
        // TODO: Setup sapDesc according to sampler from gLTF model
    }
    const auto &img = model.images[texInfo.source];
    if (!img.image.empty()) {
        const auto tmpImage =
            Image(img.image, img.width, img.height, img.component, img.bits);
        return std::make_unique<Texture>(m_deviceResources,
                                         bindSlot,
                                         TextureBindTarget::ShaderResourceView,
                                         tmpImage,
                                         img.name,
                                         samDesc);
    }
    return nullptr;
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
    std::unique_ptr<Texture> baseColorTex;
    std::unique_ptr<Texture> metallicRoughnessTex;
    std::unique_ptr<Texture> normalTex;
    std::unique_ptr<Texture> occlusionTex;
    std::unique_ptr<Texture> emissiveTex;

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
            const size_t byteOffset =
                accessor.byteOffset + bufferView.byteOffset;

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

        if (primitive.material < 0) {
            continue;
        }

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
            baseColorTex = CreateTexture(
                model,
                material.pbrMetallicRoughness.baseColorTexture.index,
                TextureBindTarget::ShaderResourceView,
                0);
        }

        if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
            metallicRoughnessTex = CreateTexture(
                model,
                material.pbrMetallicRoughness.metallicRoughnessTexture.index,
                TextureBindTarget::ShaderResourceView,
                1);
        }

        if (material.normalTexture.index >= 0) {
            normalTex = CreateTexture(model,
                                      material.normalTexture.index,
                                      TextureBindTarget::ShaderResourceView,
                                      2);
        }

        if (material.occlusionTexture.index >= 0) {
            occlusionTex = CreateTexture(model,
                                         material.occlusionTexture.index,
                                         TextureBindTarget::ShaderResourceView,
                                         3);
        }

        if (material.emissiveTexture.index >= 0) {
            emissiveTex = CreateTexture(model,
                                        material.emissiveTexture.index,
                                        TextureBindTarget::ShaderResourceView,
                                        4);
        }
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
        const Vec4D position = Vec4D(
            positions[idx].X, positions[idx].Y, positions[idx].Z, texCoord.X);
        vertices.push_back({position, normal, tangent});
    }

    assert(!vertices.empty() && "Vertices is empty! gLTF import failed!");
    assert(!indices.empty() && "Indices is empty! gLTF import failed!");

    auto meshPrim = std::make_unique<Renderer::MeshPrimitive>(
        m_deviceResources, vertices, indices);
    tmpMaterial.BaseColorTexture = std::move(baseColorTex);
    tmpMaterial.MetallicRoughnessTexture = std::move(metallicRoughnessTex);
    tmpMaterial.NormalTexture = std::move(normalTex);
    tmpMaterial.OcclusionTexture = std::move(occlusionTex);
    tmpMaterial.EmissiveTexture = std::move(emissiveTex);
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

    Transform transform;
    for (const auto nodeIdx : scene.nodes) {
        ProcessNode(model.nodes[nodeIdx], model, meshPrimitives, transform);
    }

    std::unique_ptr<Renderer::Mesh> modelTmp = std::make_unique<Renderer::Mesh>(
        m_deviceResources, std::move(meshPrimitives));
    modelTmp->SetTransform(transform);

    return std::move(modelTmp);
}
