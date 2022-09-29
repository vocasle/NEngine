#include "NEngine/Helpers/AssetManager.h"

#include <assimp/scene.h>
#include <d3d11.h>

#include "NEngine/Helpers/Texture.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {

using namespace Utils;

AssetManager::AssetManager(DeviceResources &deviceResources)
    : m_deviceResources(&deviceResources) {
}

Texture *
AssetManager::LoadTexture(const std::string &path) {
    if (Texture *tex = GetTexture(path))
        return tex;

    const std::string fullPath =
        path.find(':') == std::string::npos
            ? m_knownLocations[m_knownLocations.size() - 1] + '/' + path
            : path;

    m_textures.insert(
        std::make_pair(path,
                       Texture(fullPath,
                               m_deviceResources->GetDevice(),
                               m_deviceResources->GetDeviceContext())));
    return &m_textures.at(path);
}

Texture *
AssetManager::GetTexture(const std::string &path) {
    auto it = m_textures.find(path);
    if (it != std::end(m_textures))
        return &it->second;
    return nullptr;
}

std::vector<Mesh>
AssetManager::LoadModel(const std::string &path) {
    AddPathToKnownLocations(path);
    std::vector<Mesh> meshes = m_modelLoader.Load(path);

    for (Mesh &mesh : meshes) {
        mesh.CreateDeviceDependentResources(m_deviceResources->GetDevice());

        for (const TextureInfo &ti : mesh.GetTextures()) {
            if (ti.StorageType == TextureStorageType::Detached) {
                LoadTexture(ti.Path);
            } else if (ti.StorageType == TextureStorageType::Embedded) {
                LoadEmbeddedTexture(ti);
            }
        }
    }
    m_modelLoader.FreeScene();
    return meshes;
}

Texture *
AssetManager::LoadEmbeddedTexture(const TextureInfo &textureInfo) {
    auto tex = reinterpret_cast<const aiTexture *>(textureInfo.TexturePtr);
    if (tex->mHeight != 0) {
        m_textures.insert(
            std::make_pair(textureInfo.Path,
                           Texture(DXGI_FORMAT_B8G8R8A8_UNORM,
                                   static_cast<int>(tex->mWidth),
                                   static_cast<int>(tex->mHeight),
                                   tex->pcData,
                                   tex->mWidth * 4,
                                   tex->mWidth * tex->mHeight * 4,
                                   m_deviceResources->GetDevice())));

        return &m_textures.at(textureInfo.Path);
    }

    //    // mHeight is 0, so try to load a compressed texture of mWidth bytes
    //    const size_t size = embeddedTexture->mWidth;
    //
    //    hr = CreateWICTextureFromMemory(
    //        dev_,
    //        devcon_,
    //        reinterpret_cast<const unsigned char *>(embeddedTexture->pcData),
    //        size,
    //        nullptr,
    //        &texture);
    //    if (FAILED(hr)) 		MessageBox(hwnd_, "Texture couldn't be
    //    created
    // from memory!", "Error!", MB_ICONERROR | MB_OK);
    UtilsDebugPrint(
        "ERROR: Failed to load embedded texture %s because height is "
        "0\n",
        textureInfo.Path.c_str());
    return nullptr;
}

void
AssetManager::AddPathToKnownLocations(const std::string &path) {
    const auto dir = path.substr(0, path.find_last_of('/'));
    m_knownLocations.push_back(dir);
}

}
}
