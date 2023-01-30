#include "NEngine/Renderer/MeshPrimitive.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/VertexBuffer.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

NEngine::Renderer::MeshPrimitive::MeshPrimitive(
    Helpers::DeviceResources &deviceResources,
    std::vector<PosNormTangTex> vertices,
    std::vector<unsigned int> indices)
    : mIndexBuffer(deviceResources, indices),
      mVertexBuffer(deviceResources, vertices),
      mVertices(std::move(vertices)),
      mIndices(std::move(indices))

{
}

void
NEngine::Renderer::MeshPrimitive::SetMaterial(Material material)
{
    mMaterial = std::move(material);
}

void
NEngine::Renderer::MeshPrimitive::Bind(
    Helpers::DeviceResources &deviceResources) const
{
    mVertexBuffer.Bind(deviceResources);
    mIndexBuffer.Bind(deviceResources);

    if (mMaterial.BaseColorTexture)
        mMaterial.BaseColorTexture->Bind(deviceResources);
    if (mMaterial.MetallicRoughnessTexture)
        mMaterial.MetallicRoughnessTexture->Bind(deviceResources);
    if (mMaterial.EmissiveTexture)
        mMaterial.EmissiveTexture->Bind(deviceResources);
    if (mMaterial.NormalTexture)
        mMaterial.NormalTexture->Bind(deviceResources);
    if (mMaterial.OcclusionTexture)
        mMaterial.OcclusionTexture->Bind(deviceResources);
    if (mMaterial.KHRMaterial.DiffuseTexture)
        mMaterial.KHRMaterial.DiffuseTexture->Bind(deviceResources);
}
size_t
NEngine::Renderer::MeshPrimitive::GetIndexNum() const
{
    return mIndices.size();
}

const NEngine::Renderer::Material &
NEngine::Renderer::MeshPrimitive::GetMaterial() const
{
    return mMaterial;
}

NEngine::Renderer::Material::Material(const Material &rhs)
    : BaseColor(rhs.BaseColor),
      Metalness(rhs.Metalness),
      Roughness(rhs.Roughness),
      NormalScale(rhs.NormalScale),
      AlphaMode(rhs.AlphaMode)
{
    BaseColorTexture = rhs.BaseColorTexture
                           ? std::make_unique<Texture>(*rhs.BaseColorTexture)
                           : nullptr;
    MetallicRoughnessTexture =
        rhs.MetallicRoughnessTexture
            ? std::make_unique<Texture>(*rhs.MetallicRoughnessTexture)
            : nullptr;
    NormalTexture = rhs.NormalTexture
                        ? std::make_unique<Texture>(*rhs.NormalTexture)
                        : nullptr;
    OcclusionTexture = rhs.OcclusionTexture
                           ? std::make_unique<Texture>(*rhs.OcclusionTexture)
                           : nullptr;
    EmissiveTexture = rhs.EmissiveTexture
                          ? std::make_unique<Texture>(*rhs.EmissiveTexture)
                          : nullptr;
}

NEngine::Renderer::Material &
NEngine::Renderer::Material::operator=(const Material &rhs)
{
    Material tmp(rhs);
    *this = std::move(tmp);
    return *this;
}