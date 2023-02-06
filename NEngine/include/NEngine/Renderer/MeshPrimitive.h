#pragma once

#include <memory>
#include <vector>

#include "IndexBuffer.h"
#include "InputLayout.h"
#include "NEngine/Helpers/DeviceResources.h"
#include "Texture.h"
#include "VertexBuffer.h"

namespace NEngine {
namespace Renderer {

enum class AlphaMode {
    Opaque,
    Mask,
    Blend
};

struct KHRMaterial
{
    Texture DiffuseTexture;
    Texture SpecularTexture;
    Texture SpecularGlossinessTexture;
    Math::Vec4D DiffuseFactor;
    Math::Vec3D SpecularFactor;
    float GlossinessFactor = 0;
};

struct Material
{
    Texture BaseColorTexture;
    // green is roughness, blue contains metalness
    Texture MetallicRoughnessTexture;
    Texture NormalTexture;
    Texture OcclusionTexture;
    Texture EmissiveTexture;
    KHRMaterial KHRMaterial;
    Math::Vec4D BaseColor;
    Math::Vec3D EmissiveFactor;
    float Metalness = 0;
    float Roughness = 0;
    float NormalScale = 0;
    float OcclusionStrength = 0;
    AlphaMode AlphaMode = AlphaMode::Blend;
};

class MeshPrimitive
{
public:
    MeshPrimitive(Helpers::DeviceResources &deviceResources,
                  std::vector<PosNormTangTex> vertices,
                  std::vector<unsigned int> indices);

    void SetMaterial(Material material);
    void Bind(Helpers::DeviceResources &deviceResources) const;
    size_t GetIndexNum() const;
    const Material &GetMaterial() const;

private:
    IndexBuffer mIndexBuffer;
    VertexBuffer<PosNormTangTex> mVertexBuffer;

    std::vector<PosNormTangTex> mVertices;
    std::vector<unsigned int> mIndices;

    Material mMaterial;

    friend class FileWriter;
};
}  // namespace Renderer
}  // namespace NEngine
