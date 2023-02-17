#pragma once
#include <vector>

#include "Animation.h"
#include "Mesh.h"
#include "NEngine/Renderer/MeshPrimitive.h"

namespace NEngine {
namespace Renderer {

class Mesh
{
public:
    Mesh() = default;
    Mesh(Helpers::DeviceResources &deviceResources,
         std::vector<MeshPrimitive> meshes,
         std::vector<Animation> animations);

    const std::vector<MeshPrimitive> &GetMeshPrimitives() const;
    const std::vector<Animation> &GetAnimations() const;
    std::vector<Animation> &GetAnimations();

private:
    std::vector<MeshPrimitive> mMeshPrimitives;
    std::vector<Animation> m_animations;

    friend class FileWriter;
};
}  // namespace Renderer
}  // namespace NEngine
