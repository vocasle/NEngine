#pragma once
#include <vector>

#include "Drawable.h"
#include "Mesh.h"
#include "NEngine/Helpers/Transform.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Renderer/MeshPrimitive.h"

namespace NEngine {
namespace Renderer {
class Mesh
{
public:
    Mesh(Helpers::DeviceResources &deviceResources,
         std::vector<MeshPrimitive> meshes);
    Mesh(const Mesh &rhs);

    const std::vector<MeshPrimitive> &GetMeshPrimitives() const;

    void SetTransform(const Helpers::Transform &t);

    const Helpers::Transform &GetTransform() const;
    Helpers::Transform &GetTransform();

private:
    std::vector<MeshPrimitive> mMeshPrimitives;
    Helpers::Transform mTransform;
};
}  // namespace Renderer
}  // namespace NEngine
