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
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace NEngine {
namespace Renderer {

enum class AlphaMode {
    Opaque,
    Mask,
    Blend
};

struct KHRMaterial
{
    glm::vec4 DiffuseFactor;
    std::unique_ptr<Texture> DiffuseTexture;
    glm::vec3 SpecularFactor;
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
    glm::vec4 BaseColor;
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
    glm::vec3 EmissiveFactor;
};

class MeshPrimitive
{
public:
    MeshPrimitive(Helpers::DeviceResources &deviceResources,
                  std::vector<PosNormTangTex> vertices,
                  std::vector<unsigned int> indices);

    MeshPrimitive(const MeshPrimitive &rhs);

    void SetMaterial(Material material);
    void Bind(Helpers::DeviceResources &deviceResources) const;
    size_t GetIndexNum() const;
    const Material &GetMaterial() const;

private:
    std::unique_ptr<IndexBuffer> mIndexBuffer;
    std::unique_ptr<VertexBuffer<PosNormTangTex>> mVertexBuffer;

    std::vector<PosNormTangTex> mVertices;
    std::vector<unsigned int> mIndices;

    Material mMaterial;
};
}  // namespace Renderer
}  // namespace NEngine
