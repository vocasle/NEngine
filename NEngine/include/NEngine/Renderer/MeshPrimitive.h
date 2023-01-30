#pragma once

#include <memory>
#include <vector>

#include "Bindable.h"
#include "Drawable.h"
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
    Math::Vec4D DiffuseFactor;
    std::unique_ptr<Texture> DiffuseTexture;
    Math::Vec3D SpecularFactor;
    std::unique_ptr<Texture> SpecularTexture;
    float GlossinessFactor = 0;
    std::unique_ptr<Texture> SpecularGlossinessTexture;
};

struct Material
{
    Material() = default;
    Material(const Material &rhs);
    Material(Material &&rhs) = default;
    Material &operator=(Material &&rhs) = default;
    Material &operator=(const Material &rhs);
    Math::Vec4D BaseColor;
    float Metalness = 0;
    float Roughness = 0;
    float NormalScale = 0;
    std::unique_ptr<Texture> BaseColorTexture;
    // green is roughness, blue contains metalness
    std::unique_ptr<Texture> MetallicRoughnessTexture;
    std::unique_ptr<Texture> NormalTexture;
    std::unique_ptr<Texture> OcclusionTexture;
    std::unique_ptr<Texture> EmissiveTexture;
    AlphaMode AlphaMode = AlphaMode::Blend;
    KHRMaterial KHRMaterial;
    float OcclusionStrength = 0;
    Math::Vec3D EmissiveFactor;
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
};
}  // namespace Renderer
}  // namespace NEngine
