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
class Mesh
{
public:
    Mesh(Helpers::DeviceResources &deviceResources,
         const std::vector<VertexPositionNormalTangent> &vertices,
         const std::vector<unsigned int> &indices);

private:
    std::vector<std::unique_ptr<Bindable>> mBinds;
    IndexBuffer *mIndexBuffer;

    std::vector<VertexPositionNormalTangent> mVertices;
    std::vector<unsigned int> mIndices;
};
}  // namespace Renderer
}  // namespace NEngine
