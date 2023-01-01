#include "NEngine/Helpers/GLTFLoader.h"

#include <stdlib.h>
#include <winscard.h>

#include <algorithm>
#include <memory>

#include "NEngine/Helpers/DeviceResources.h"
#include "NEngine/Helpers/Renderer.h"
#include "NEngine/Helpers/Transform.h"
#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/MeshPrimitive.h"
#include "NEngine/Renderer/Sampler.h"
#include "NEngine/Renderer/Texture.h"
#include "NEngine/Utils/Utils.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "mikktspace/mikktspace.h"
#include "tinygltf/tiny_gltf.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;
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
            const auto rot = glm::toMat4(glm::quat(node.rotation[0],
                                                   node.rotation[1],
                                                   node.rotation[2],
                                                   node.rotation[3]));

            t.SetRotation(rot);
        }
        if (!node.translation.empty()) {
            const auto offset = glm::vec3(
                node.translation[0], node.translation[1], node.translation[2]);
            t.SetTranslation(glm::translate(glm::mat4x4(1), offset));
        }
        if (!node.scale.empty()) {
            const auto scale =
                glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
            t.SetScale(glm::scale({}, scale));
        }
    }

    return t;
}

void
GLTFLoader::ProcessNode(
    const tinygltf::Node &node,
    const tinygltf::Model &model,
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &outMeshes)
{
    if (node.mesh >= 0) {
        const auto transform = GetNodeTransform(node);
        auto mesh = ProcessMesh(model.meshes[node.mesh], model);
        mesh->SetTransform(transform);
        outMeshes.push_back(std::move(mesh));
    }

    for (const auto childIdx : node.children) {
        ProcessNode(model.nodes[childIdx], model, outMeshes);
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
        // const auto &sampler = model.samplers[texInfo.sampler];
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

std::unique_ptr<NEngine::Renderer::Mesh>
GLTFLoader::ProcessMesh(const tinygltf::Mesh &mesh,
                        const tinygltf::Model &model)
{
    std::vector<std::unique_ptr<MeshPrimitive>> meshPrimitives;
    meshPrimitives.reserve(mesh.primitives.size());
    for (const auto &primitive : mesh.primitives) {
        meshPrimitives.push_back(ProcessMeshPrimitive(primitive, model));
    }

    return std::make_unique<Mesh>(m_deviceResources, std::move(meshPrimitives));
}

static std::vector<glm::vec4>
GenerateTangents(const std::vector<unsigned int> &indices,
                 const std::vector<glm::vec3> &normals,
                 const std::vector<glm::vec2> &texCoords,
                 const std::vector<glm::vec3> &positions)
{
    struct Mesh
    {
        Mesh(const std::vector<unsigned int> &indices,
             const std::vector<glm::vec3> &normals,
             const std::vector<glm::vec2> &texCoords,
             const std::vector<glm::vec3> &positions)
            : indices(indices),
              normals(normals),
              texCoords(texCoords),
              positions(positions),
              tangents(positions.size(), glm::vec4())
        {
        }
        const std::vector<unsigned int> &indices;
        const std::vector<glm::vec3> &normals;
        const std::vector<glm::vec2> &texCoords;
        const std::vector<glm::vec3> &positions;
        std::vector<glm::vec4> tangents;
    };

    Mesh mesh(indices, normals, texCoords, positions);

    SMikkTSpaceContext ctx;
    SMikkTSpaceInterface spaceInterface;
    ctx.m_pInterface = &spaceInterface;
    ctx.m_pUserData = &mesh;

    // auto SetTSpaceBasicCB = [](const SMikkTSpaceContext *pContext,
    //                            const float fvTangent[],
    //                            const float fSign,
    //                            const int iFace,
    //                            const int iVert)
    // {
    //     const auto mesh = reinterpret_cast<Mesh *>(pContext->m_pUserData);
    //     const auto idx = mesh->indices[iFace * 3 + iVert];
    //     mesh->tangents[idx] =
    //         Vec4D(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
    // };

    auto GetNumFacesCB = [](const SMikkTSpaceContext *pContext)
    {
        const auto mesh = reinterpret_cast<Mesh *>(pContext->m_pUserData);
        assert(mesh->indices.size() % 3 == 0);
        return static_cast<int>(mesh->indices.size() / 3);
    };

    auto GetNumVerticesOfFaceCB = [](const SMikkTSpaceContext *pContext,
                                     const int iFace) { return 3; };

    auto GetPositionCB = [](const SMikkTSpaceContext *pContext,
                            float fvPosOut[],
                            const int iFace,
                            const int iVert)
    {
        const auto mesh = reinterpret_cast<Mesh *>(pContext->m_pUserData);
        const auto idx = mesh->indices[iFace * 3 + iVert];
        const auto pos = mesh->positions[idx];
        fvPosOut[0] = pos.x;
        fvPosOut[1] = pos.y;
        fvPosOut[2] = pos.z;
    };

    auto GetNormalCB = [](const SMikkTSpaceContext *pContext,
                          float fvNormOut[],
                          const int iFace,
                          const int iVert)
    {
        const auto mesh = reinterpret_cast<Mesh *>(pContext->m_pUserData);
        const auto idx = mesh->indices[iFace * 3 + iVert];
        const auto norm = mesh->normals[idx];
        fvNormOut[0] = norm.x;
        fvNormOut[1] = norm.y;
        fvNormOut[2] = norm.z;
    };

    auto GetTexCoordCB = [](const SMikkTSpaceContext *pContext,
                            float fvTexcOut[],
                            const int iFace,
                            const int iVert)
    {
        const auto mesh = reinterpret_cast<Mesh *>(pContext->m_pUserData);
        const auto idx = mesh->indices[iFace * 3 + iVert];
        const auto texCoord = mesh->texCoords[idx];
        fvTexcOut[0] = texCoord.x;
        fvTexcOut[1] = texCoord.y;
    };

    auto SetTSpaceCB = [](const SMikkTSpaceContext *pContext,
                          const float fvTangent[],
                          const float fvBiTangent[],
                          const float fMagS,
                          const float fMagT,
                          const tbool bIsOrientationPreserving,
                          const int iFace,
                          const int iVert)
    {
        const auto mesh = reinterpret_cast<Mesh *>(pContext->m_pUserData);
        const auto idx = mesh->indices[iFace * 3 + iVert];
        mesh->tangents[idx] = glm::vec4(fvTangent[0],
                                        fvTangent[1],
                                        fvTangent[2],
                                        bIsOrientationPreserving ? 1 : -1);
    };

    spaceInterface.m_getNumFaces = GetNumFacesCB;
    spaceInterface.m_getNumVerticesOfFace = GetNumVerticesOfFaceCB;
    spaceInterface.m_getPosition = GetPositionCB;
    spaceInterface.m_getNormal = GetNormalCB;
    spaceInterface.m_getTexCoord = GetTexCoordCB;
    spaceInterface.m_setTSpace = SetTSpaceCB;
    spaceInterface.m_setTSpaceBasic = nullptr;

    genTangSpaceDefault(&ctx);
    return mesh.tangents;
}

std::vector<float>
GetPropAsVec(const tinygltf::Value &ext, const std::string &propName)
{
    std::vector<float> numbers;
    if (ext.Has(propName)) {
        auto prop = ext.Get(propName);
        for (size_t i = 0; i < prop.ArrayLen(); ++i) {
            auto val = prop.Get(i);
            const float num = val.IsNumber()
                                  ? static_cast<float>(val.Get<double>())
                                  : static_cast<float>(val.Get<int>());
            numbers.push_back(num);
        }
    }
    return numbers;
}

std::unique_ptr<NEngine::Renderer::MeshPrimitive>
GLTFLoader::ProcessMeshPrimitive(const tinygltf::Primitive &primitive,
                                 const tinygltf::Model &model)
{
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec2> texCoords;
    Material tmpMaterial;
    KHRMaterial khrMaterial;
    std::unique_ptr<Texture> baseColorTex;
    std::unique_ptr<Texture> metallicRoughnessTex;
    std::unique_ptr<Texture> normalTex;
    std::unique_ptr<Texture> occlusionTex;
    std::unique_ptr<Texture> emissiveTex;

    const auto &indexAccessor = model.accessors[primitive.indices];

    indices = ExtractMeshIndices(indexAccessor, model);

    for (const auto &[name, accessorIdx] : primitive.attributes) {
        const auto &accessor = model.accessors[accessorIdx];
        const auto &bufferView = model.bufferViews[accessor.bufferView];
        const auto &buffer = model.buffers[bufferView.buffer];
        const size_t byteStride = accessor.ByteStride(bufferView);
        const size_t byteOffset = accessor.byteOffset + bufferView.byteOffset;

        if (name == "POSITION") {
            positions =
                ReadData<glm::vec3>(buffer.data, accessor.count, byteOffset);
        }
        else if (name == "NORMAL") {
            normals =
                ReadData<glm::vec3>(buffer.data, accessor.count, byteOffset);
        }
        else if (name == "TANGENT") {
            tangents =
                ReadData<glm::vec4>(buffer.data, accessor.count, byteOffset);
        }
        else if (name == "TEXCOORD_0") {
            texCoords =
                ReadData<glm::vec2>(buffer.data, accessor.count, byteOffset);
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

    if (primitive.material >= 0) {
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
        tmpMaterial.NormalScale = material.normalTexture.scale;

        if (material.extensions.size() > 0) {
            for (auto &[extName, ext] : material.extensions) {
                if (extName == "KHR_materials_pbrSpecularGlossiness") {
                    if (ext.Has("diffuseFactor")) {
                        auto diffuseFactor = GetPropAsVec(ext, "diffuseFactor");
                        khrMaterial.DiffuseFactor = {diffuseFactor[0],
                                                     diffuseFactor[1],
                                                     diffuseFactor[2],
                                                     diffuseFactor[3]};
                    }

                    if (ext.Has("diffuseTexture")) {
                        auto idx = ext.Get("diffuseTexture").Get("index");
                        khrMaterial.DiffuseTexture =
                            CreateTexture(model,
                                          idx.Get<int>(),
                                          TextureBindTarget::ShaderResourceView,
                                          0);
                    }
                }
            }
        }

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
            tmpMaterial.OcclusionStrength = material.occlusionTexture.strength;
        }

        if (material.emissiveTexture.index >= 0) {
            emissiveTex = CreateTexture(model,
                                        material.emissiveTexture.index,
                                        TextureBindTarget::ShaderResourceView,
                                        4);
            tmpMaterial.EmissiveFactor = glm::vec3(material.emissiveFactor[0],
                                                   material.emissiveFactor[1],
                                                   material.emissiveFactor[2]);
        }
    }

    std::vector<PosNormTangTex> vertices;

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
        tangents = GenerateTangents(indices, normals, texCoords, positions);
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
        vertices.push_back(
            {positions[idx], normals[idx], tangents[idx], texCoords[idx]});
    }

    assert(!vertices.empty() && "Vertices is empty! gLTF import failed!");
    assert(!indices.empty() && "Indices is empty! gLTF import failed!");

    auto meshPrim =
        std::make_unique<MeshPrimitive>(m_deviceResources, vertices, indices);
    tmpMaterial.BaseColorTexture = std::move(baseColorTex);
    tmpMaterial.MetallicRoughnessTexture = std::move(metallicRoughnessTex);
    tmpMaterial.NormalTexture = std::move(normalTex);
    tmpMaterial.OcclusionTexture = std::move(occlusionTex);
    tmpMaterial.EmissiveTexture = std::move(emissiveTex);
    tmpMaterial.KHRMaterial = std::move(khrMaterial);
    meshPrim->SetMaterial(std::move(tmpMaterial));

    return std::move(meshPrim);
}

GLTFLoader::GLTFLoader(DeviceResources &deviceResources)
    : m_deviceResources(deviceResources)
{
}

std::vector<std::unique_ptr<NEngine::Renderer::Mesh>>
GLTFLoader::Load(const std::string &path)
{
    tinygltf::Model model;

    std::string err;
    std::string warn;

    tinygltf::TinyGLTF loader;

    bool isLoadSuccessful = false;
    if (path.find(".gltf") != std::string::npos) {
        isLoadSuccessful = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }
    else {
        isLoadSuccessful = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    }

    if (!err.empty()) {
        UtilsDebugPrint("ERROR: Failed to load model: %s\n", err.c_str());
        return {};
    }
    if (!warn.empty()) {
        UtilsDebugPrint("WARN: %s\n", warn.c_str());
        return {};
    }
    if (!isLoadSuccessful) {
        UtilsDebugPrint("ERROR: Failed to parse gLTF\n");
        return {};
    }

    const auto &scene = model.scenes[model.defaultScene];

    std::vector<std::unique_ptr<MeshPrimitive>> meshPrimitives;

    std::vector<std::unique_ptr<Mesh>> meshes;
    for (const auto nodeIdx : scene.nodes) {
        ProcessNode(model.nodes[nodeIdx], model, meshes);
    }

    return meshes;
}
