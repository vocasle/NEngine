#pragma once

#include <memory>
#include <vector>

#include "Bindable.h"
#include "Drawable.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "Texture.h"

namespace NEngine {
namespace Renderer {

enum class AlphaMode {
    Opaque,
    Mask,
    Blend
};

struct Material
{
    Math::Vec4D BaseColor;
    float Metalness;
    float Roughness;
    std::unique_ptr<Texture> BaseColorTexture;
    // green is roughness, blue contains metalness
    std::unique_ptr<Texture> MetallicRoughnessTexture;
    std::unique_ptr<Texture> NormalTexture;
    std::unique_ptr<Texture> OcclusionTexture;
    std::unique_ptr<Texture> EmissiveTexture;
    AlphaMode AlphaMode;
};

class MeshPrimitive
{
public:
    MeshPrimitive(Helpers::DeviceResources &deviceResources,
                  std::vector<VertexPositionNormalTangent> vertices,
                  std::vector<unsigned int> indices);

    void SetMaterial(Material material);
    void Bind(Helpers::DeviceResources &deviceResources) const;
    size_t GetIndexNum() const;
    const Material &GetMaterial() const;

private:
    std::vector<std::unique_ptr<Bindable>> mBinds;
    IndexBuffer *mIndexBuffer;

    std::vector<VertexPositionNormalTangent> mVertices;
    std::vector<unsigned int> mIndices;

    Material mMaterial;
};
}  // namespace Renderer
}  // namespace NEngine
