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
    : m_ib(deviceResources, indices),
      m_vb(deviceResources, vertices),
      m_vertices(std::move(vertices)),
      m_indices(std::move(indices)),
      m_morph_targets(std::move(morph_targets))

{
}

void
NEngine::Renderer::MeshPrimitive::SetMaterial(Material material)
{
    m_material = std::move(material);
}

void
NEngine::Renderer::MeshPrimitive::Bind(
    Helpers::DeviceResources &deviceResources) const
{
    m_vb.Bind(deviceResources);
    m_ib.Bind(deviceResources);

    m_material.BaseColorTexture.Bind(deviceResources);
    m_material.MetallicRoughnessTexture.Bind(deviceResources);
    m_material.EmissiveTexture.Bind(deviceResources);
    m_material.NormalTexture.Bind(deviceResources);
    m_material.OcclusionTexture.Bind(deviceResources);
    m_material.KHRMaterial.DiffuseTexture.Bind(deviceResources);
}
size_t
NEngine::Renderer::MeshPrimitive::GetIndexNum() const
{
    return m_indices.size();
}

const NEngine::Renderer::Material &
NEngine::Renderer::MeshPrimitive::GetMaterial() const
{
    return m_material;
}

const std::vector<NEngine::Renderer::MorphTarget> &
NEngine::Renderer::MeshPrimitive::get_morph_targets() const
{
    return m_morph_targets;
}
