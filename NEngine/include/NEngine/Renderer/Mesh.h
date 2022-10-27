#pragma once
#include <vector>

#include "Drawable.h"
#include "Mesh.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Renderer/MeshPrimitive.h"

namespace NEngine {
namespace Renderer {
class Mesh
{
public:
    Mesh(Helpers::DeviceResources &deviceResources,
         std::vector<std::unique_ptr<Renderer::MeshPrimitive>> meshes);

    const std::vector<std::unique_ptr<NEngine::Renderer::MeshPrimitive>>
        &GetMeshPrimitives() const;

private:
    std::vector<std::unique_ptr<NEngine::Renderer::MeshPrimitive>>
        mMeshPrimitives;
};
}  // namespace Renderer
}  // namespace NEngine
