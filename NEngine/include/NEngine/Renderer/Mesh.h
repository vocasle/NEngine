#pragma once

#include <memory>
#include <vector>

#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "InputLayout.h"


namespace NEngine {
namespace Renderer {
class Mesh : public Drawable
{
public:
    Mesh(Helpers::DeviceResources& deviceResources, const std::vector<VertexPositionNormalTangent> &vertices, const std::vector<unsigned int> &indices);
    void Draw(Helpers::DeviceResources &deviceResources) override;

private:
    std::vector<std::unique_ptr<Bindable>> mBinds;
    IndexBuffer *mIndexBuffer;

    std::vector<VertexPositionNormalTangent> mVertices;
    std::vector<unsigned int> mIndices;


};
}
}