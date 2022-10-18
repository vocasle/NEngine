#include "NEngine/Helpers/GLTFLoader.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Utils/Utils.h"


using namespace NEngine::Utils;
using namespace NEngine::Helpers;


void
GLTFLoader::ProcessNode(const tinygltf::Node &node,
                        const tinygltf::Model &model)
{
    if (node.mesh >= 0)
        ProcessMesh(model.meshes[node.mesh], model);

    for (const auto childIdx : node.children) {
        ProcessNode(model.nodes[childIdx], model);
    }
}

std::vector<unsigned int>
GLTFLoader::ExtractMeshIndices(const tinygltf::Accessor &indexAccessor,
                               const tinygltf::Model &model)
{
    std::vector<unsigned int> indices;
    const auto &bufferView = model.bufferViews[indexAccessor.
        bufferView];
    const auto &buffer = model.buffers[bufferView.buffer];

    const size_t byteStride = indexAccessor.ByteStride(bufferView);
    indices.reserve(indexAccessor.count);

    for (size_t i = bufferView.byteOffset; i < buffer.data.size();
         i += byteStride) {
        if (indexAccessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            indices.push_back(
                *reinterpret_cast<const unsigned short *>(&buffer.data.at(
                    i)));
        }
        else if (indexAccessor.componentType ==
                 TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            indices.push_back(
                *reinterpret_cast<const unsigned int *>(&buffer.data.at(
                    i)));
        }
        else {
            UtilsDebugPrint("ERROR: Unexpected byte length of index type.\n");
        }
    }

    return indices;
}

void
GLTFLoader::ProcessMesh(const tinygltf::Mesh &mesh,
                        const tinygltf::Model &model)
{
    std::vector<unsigned int> indices;
    std::vector<Math::Vec3D> positions;
    std::vector<Math::Vec3D> normals;
    std::vector<Math::Vec4D> tangents;

    assert(
        mesh.primitives.size() == 1 &&
        "Mesh contains more than one primitive!");

    for (const auto &primitive : mesh.primitives) {
        const auto &indexAccessor = model.accessors[primitive.indices];

        indices =
            ExtractMeshIndices(indexAccessor, model);

        for (const auto &[name, accessorIdx] : primitive.attributes) {
            const auto &accessor = model.accessors[accessorIdx];
            const auto &bufferView = model.bufferViews[accessor.bufferView];
            const auto &buffer = model.buffers[bufferView.buffer];
            const size_t byteStride = accessor.ByteStride(bufferView);
            if (name == "POSITION") {
                positions.resize(accessor.count);
                memcpy(&positions[0],
                       buffer.data.data() + bufferView.byteOffset,
                       buffer.data.size() - bufferView.byteOffset);
            }
            else if (name == "NORMAL") {
                normals.resize(accessor.count);
                memcpy(&normals[0],
                       buffer.data.data() + bufferView.byteOffset,
                       buffer.data.size() - bufferView.byteOffset);
            }
            else if (name == "TANGENT") {
                tangents.resize(accessor.count);
                memcpy(&tangents[0],
                       buffer.data.data() + bufferView.byteOffset,
                       buffer.data.size() - bufferView.byteOffset);
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

    if (normals.empty()) {
        normals.resize(indices.size(), {0, 0, 0});
    }
    if (tangents.empty()) {
        tangents.resize(indices.size(), {0, 0, 0, 0});
    }

    std::vector<Renderer::VertexPositionNormalTangent> vertices;
    vertices.reserve(indices.size());
    for (const unsigned int idx : indices) {
        vertices.push_back({
            {positions[idx], 1},
            {normals[idx], 0},
            tangents[idx]
        });
    }

    assert(!vertices.empty() && "Vertices is empty! gLTF import failed!");
    assert(!indices.empty() && "Indices is empty! gLTF import failed!");

    m_model->AddMesh(std::make_unique<Renderer::Mesh>(m_deviceResources, std::move(vertices), std::move(indices)));
}

GLTFLoader::GLTFLoader(DeviceResources &deviceResources): m_deviceResources(deviceResources)
{
}

std::unique_ptr<NEngine::Renderer::Model>
GLTFLoader::Load(const std::string &path)
{
    tinygltf::Model model;

    std::string err;
    std::string warn;

    const bool ret = m_loader.LoadASCIIFromFile(
        &model,
        &err,
        &warn,
        path);

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

    for (const auto nodeIdx : scene.nodes) {
        ProcessNode(model.nodes[nodeIdx], model);
    }

    return std::move(m_model);
}
