#pragma once
#include <vector>

#include "Drawable.h"
#include "Mesh.h"

#include "NEngine/Math/Math.h"

namespace NEngine {
namespace Renderer {
class Model : public Drawable
{
public:
    void Draw(Helpers::DeviceResources &deviceResources) override;
    void AddMesh(std::unique_ptr<Mesh> mesh);

    struct Vertex
    {
        Math::Vec4D Position;
        Math::Vec4D Normal;
        Math::Vec4D Tangent;
    };

private:
    Model() = default;

    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<std::string> mTextures;
    std::vector<std::unique_ptr<Mesh>> mMeshes;
};
}
}
