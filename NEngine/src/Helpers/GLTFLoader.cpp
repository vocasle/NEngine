#include "NEngine/Helpers/GLTFLoader.h"

#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Utils;
using namespace NEngine::Helpers;
using namespace NEngine::Math;
using namespace NEngine::Renderer;

void
GLTFLoader::ProcessNode(
    const tinygltf::Node &node,
    const tinygltf::Model &model,
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &outMeshes)
{
    if (node.mesh >= 0) {
        auto mesh = ProcessMesh(model.meshes[node.mesh], model);
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

std::unique_ptr<NEngine::Renderer::Mesh>
GLTFLoader::ProcessMesh(const tinygltf::Mesh &mesh,
                        const tinygltf::Model &model)
{
    std::vector<unsigned int> indices;
    std::vector<Math::Vec3D> positions;
    std::vector<Math::Vec3D> normals;
    std::vector<Math::Vec4D> tangents;

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

            size_t count = accessor.count;
            const void *source = reinterpret_cast<const void *>(buffer.data.data() + bufferView.byteOffset);

            if (name == "POSITION") {
                assert(byteStride == sizeof(Math::Vec3D));
                size_t size = std::min(buffer.data.size() - bufferView.byteOffset, accessor.count * sizeof(Math::Vec3D));
                positions.resize(count);
                memcpy(&positions[0], source, size);
            }
            else if (name == "NORMAL") {
                assert(byteStride == sizeof(Math::Vec3D));
                size_t size = std::min(buffer.data.size() - bufferView.byteOffset, accessor.count * sizeof(Math::Vec3D));
                normals.resize(count);
                memcpy(&normals[0], source, size);
            }
            else if (name == "TANGENT") {
                assert(byteStride == sizeof(Math::Vec4D));
                size_t size = std::min(buffer.data.size() - bufferView.byteOffset, accessor.count * sizeof(Math::Vec4D));
                tangents.resize(count);
                memcpy(&tangents[0], source, size);
            }
            else if (name == "TEXCOORD_0") {
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
    }

    std::vector<Renderer::VertexPositionNormalTangent> vertices;

    for (const unsigned int idx : indices) {
        if (!normals.empty()) {
            assert(normals.size() > idx);
        }

        if (!tangents.empty()) {
            assert(tangents.size() > idx);
        }
        const Vec4D normal =
            normals.empty() ? Vec4D{0, 0, 0, 0} : Vec4D{normals[idx], 0};
        const Vec4D tangent =
            tangents.empty() ? Vec4D{0, 0, 0, 0} : tangents[idx];

        vertices.push_back({{positions[idx], 1}, normal, tangent});
    }

    assert(!vertices.empty() && "Vertices is empty! gLTF import failed!");
    assert(!indices.empty() && "Indices is empty! gLTF import failed!");

    return std::make_unique<Renderer::Mesh>(
        m_deviceResources, vertices, indices);
}

GLTFLoader::GLTFLoader(DeviceResources &deviceResources)
    : m_deviceResources(deviceResources)
{
}

std::unique_ptr<NEngine::Renderer::Model>
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

    std::vector<std::unique_ptr<Renderer::Mesh>> meshes;

    for (const auto nodeIdx : scene.nodes) {
        ProcessNode(model.nodes[nodeIdx], model, meshes);
    }

    std::unique_ptr<Renderer::Model> modelTmp =
        std::make_unique<Renderer::Model>(m_deviceResources, meshes);

    return std::move(modelTmp);
}
