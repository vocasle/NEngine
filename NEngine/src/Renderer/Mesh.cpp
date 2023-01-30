#include "NEngine/Renderer/Mesh.h"

#include <fstream>

#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;
using namespace NEngine::Renderer;

namespace NEngine::Renderer {
const std::vector<MeshPrimitive> &
Mesh::GetMeshPrimitives() const
{
    return mMeshPrimitives;
}

Mesh::Mesh(DeviceResources &deviceResources, std::vector<MeshPrimitive> meshes)
    : mMeshPrimitives(std::move(meshes))
{
}

const Transform &
Mesh::GetTransform() const
{
    return mTransform;
}

Transform &
Mesh::GetTransform()
{
    return mTransform;
}

bool
Mesh::Save() const
{
    // const auto file_name = UtilsFormatStr("%s.nem");
    const auto file_name = std::string("Boba.nem");
    std::ofstream out(file_name, std::ios_base::binary);
    if (!out) {
        UTILS_PRINTLN("ERR: Failed to create: %s", file_name.c_str());
        return false;
    }

    if (!out.write(reinterpret_cast<const char *>(this), sizeof(*this))) {
        UTILS_PRINTLN("ERR: Failed to write mesh to file");
        return false;
    }

    return true;
}

Mesh
Mesh::FromFile(const std::string &file_name,
               Helpers::DeviceResources &device_resources)
{
    std::ifstream in(file_name);
    if (!in) {
        UTILS_PRINTLN("ERR: Failed to open: %s", file_name.c_str());
    }
    auto mesh = Mesh(device_resources, {});
    if (!in.read(reinterpret_cast<char *>(&mesh), sizeof(mesh))) {
        UTILS_PRINTLN("ERR: Failed to read from: %s", file_name.c_str());
    }
    return mesh;
}

void
Mesh::SetTransform(const Helpers::Transform &t)
{
    mTransform = t;
}

Mesh::Mesh(const Mesh &rhs)
    : mTransform(rhs.mTransform),
      mMeshPrimitives(std::move(rhs.mMeshPrimitives))
{
}

}  // namespace NEngine::Renderer