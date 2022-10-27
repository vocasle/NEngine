#pragma once

#include <memory>
#include <vector>

#include "Bindable.h"
#include "Drawable.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "NEngine/Helpers/DeviceResources.h"

namespace NEngine {
namespace Renderer {
class MeshPrimitive
{
public:
    MeshPrimitive(Helpers::DeviceResources &deviceResources,
                  std::vector<VertexPositionNormalTangent> vertices,
                  std::vector<unsigned int> indices);

    void SetBaseColorFactor(const Math::Vec4D &baseColorFactor);
    void SetMetallicFactor(float metallicFactor);

private:
    std::vector<std::unique_ptr<Bindable>> mBinds;
    IndexBuffer *mIndexBuffer;

    std::vector<VertexPositionNormalTangent> mVertices;
    std::vector<unsigned int> mIndices;

    Math::Vec4D mBaseColorFactor;
    float mMetallicFactor;
};
}  // namespace Renderer
}  // namespace NEngine
