#include "NEngine/Renderer/MeshPrimitive.h"

#include "NEngine/Renderer/InputLayout.h"
#include "NEngine/Renderer/VertexBuffer.h"
#include "NEngine/Utils/Utils.h"

using namespace NEngine::Helpers;
using namespace NEngine::Utils;

NEngine::Renderer::MeshPrimitive::MeshPrimitive(
    Helpers::DeviceResources &deviceResources,
    std::vector<PosNormTangTex> vertices,
    std::vector<unsigned int> indices,
    std::vector<MorphTarget> morph_targets)
    : mIndexBuffer(deviceResources, indices),
      mVertexBuffer(deviceResources, vertices),
      mVertices(std::move(vertices)),
      mIndices(std::move(indices)),
      m_morph_targets(std::move(morph_targets))

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

    mMaterial.BaseColorTexture.Bind(deviceResources);
    mMaterial.MetallicRoughnessTexture.Bind(deviceResources);
    mMaterial.EmissiveTexture.Bind(deviceResources);
    mMaterial.NormalTexture.Bind(deviceResources);
    mMaterial.OcclusionTexture.Bind(deviceResources);
    mMaterial.KHRMaterial.DiffuseTexture.Bind(deviceResources);
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