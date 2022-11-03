#pragma once
#include <vector>

#include "Drawable.h"
#include "Mesh.h"
#include "NEngine/Helpers/Transform.h"
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

    void SetTransform(const Helpers::Transform &t);

    const Helpers::Transform &GetTransform() const;
    Helpers::Transform &GetTransform();

private:
    std::vector<std::unique_ptr<NEngine::Renderer::MeshPrimitive>>
        mMeshPrimitives;
    Helpers::Transform mTransform;
};
}  // namespace Renderer
}  // namespace NEngine
