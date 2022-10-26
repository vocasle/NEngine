#pragma once
#include <vector>

#include "Drawable.h"
#include "Mesh.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/Mesh.h"

namespace NEngine {
namespace Renderer {
class Model
{
public:
    Model(Helpers::DeviceResources &deviceResources,
          std::vector<std::unique_ptr<Renderer::Mesh>> &meshes);

    const std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> &GetMeshes()
        const;

    struct Vertex
    {
        Math::Vec4D Position;
        Math::Vec4D Normal;
        Math::Vec4D Tangent;
    };

private:
    Helpers::DeviceResources &mDeviceResources;
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<std::string> mTextures;
    std::vector<std::unique_ptr<NEngine::Renderer::Mesh>> mMeshes;
};
}  // namespace Renderer
}  // namespace NEngine
