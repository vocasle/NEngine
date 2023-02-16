#include "NEngine/Helpers/ModelImporter.h"

#include "NEngine/Helpers/GLTFLoader.h"
#include "NEngine/Renderer/Texture.h"
#include "NEngine/Utils/Utils.h"
#include "stb_image/stb_image.h"

namespace NEngine::Helpers {

using namespace Utils;

std::vector<NEngine::Renderer::Mesh>
ModelImporter::Load(const std::string &str) const
{
    using namespace Utils;

    if (str.find("gltf") == std::string::npos &&
        str.find("glb") == std::string::npos) {
        UtilsDebugPrint(
            "ERROR: Unsupported model format (%s). Enable assimp via "
            "NENGINE_USE_ASSIMP CMake option to support more types\n",
            str.c_str());
        return {};
    }
    return GLTFLoader(*mDeviceResources).Load(str);
}

Renderer::RenderModel
ModelImporter::load(const std::string &path) const
{
    return GLTFLoader(*mDeviceResources).load(path);
}

ModelImporter::ModelImporter(Helpers::DeviceResources &deviceResources)
    : mDeviceResources(&deviceResources)
{
}

}  // namespace NEngine::Helpers