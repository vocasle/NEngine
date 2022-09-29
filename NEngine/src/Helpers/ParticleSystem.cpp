#include "NEngine/Helpers/ParticleSystem.h"

#include <cassert>

#include "NEngine/Utils/Image.h"
#include "NEngine/Helpers/InputLayout.h"

namespace NEngine {
namespace Helpers {
using namespace Microsoft::WRL;
using namespace Math;
using namespace Utils;

ParticleSystem::ParticleSystem(const std::string &name,
                               const Vec3D &origin,
                               const Vec3D &acceleration,
                               const Vec3D &initVel,
                               const Camera &camera)
    : m_name{name},
      m_vertices{},
      m_indices{},
      m_options{},
      m_camera{&camera},
      m_emitter{
          ParticleType::Emitter, acceleration, initVel, origin, 0.0f, *this} {
    m_options.origin = origin;
    m_options.accel = acceleration;
    m_options.initVel = initVel;
    m_vertices.reserve(MAX_PARTICLES * 4);
    m_indices.reserve(MAX_PARTICLES * 6);
}

void
ParticleSystem::Init(ID3D11Device *device,
                     ID3D11DeviceContext *context,
                     const std::string &texFilePath) {
    CreateTexture(device, context, texFilePath);
    CreateBlendState(device);
    CreateDepthStencilState(device);
    CreateSamplerState(device);
    //	CreateEmitter();
    EmitParticle();
    CreateVertexBuffer(device);
    CreateIndexBuffer(device);
}

void
ParticleSystem::Tick(const float deltaTime) {
    static float elapsedTime = 0.0f;
    elapsedTime += deltaTime;
    if (m_particles.size() < static_cast<size_t>(m_options.maxParticles) &&
        elapsedTime > 0.1f) {
        for (size_t i = 0; i < m_options.burst; ++i)
            EmitParticle();
        elapsedTime = 0.0f;
    }

    for (Particle &p : m_particles) {
        if (p.IsAlive()) {
            p.Tick(deltaTime);
            p.CreateQuad(m_options.particleSize.X,
                         m_options.particleSize.Y,
                         m_camera->GetUp(),
                         m_camera->GetRight());
        } else {
            p.Reset();
        }
    }

    UpdateVertices();
}

size_t
ParticleSystem::GetNumAliveParticles() const {
    size_t num = 0;
    for (const Particle &p : m_particles) {
        if (p.IsAlive()) {
            ++num;
        }
    }
    return num;
}

void
ParticleSystem::CreateTexture(ID3D11Device *device,
                              ID3D11DeviceContext *context,
                              const std::string &filepath) {
    Image image{filepath};

    ComPtr<ID3D11Texture2D> texture;
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = image.GetWidth();
        desc.Height = image.GetHeight();
        desc.MipLevels = 0;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        HR(device->CreateTexture2D(
            &desc, nullptr, texture.ReleaseAndGetAddressOf()))

        context->UpdateSubresource(
            texture.Get(),
            0,
            nullptr,
            image.GetBytes(),
            image.GetWidth() * sizeof(uint8_t) * Image::DESIRED_CHANNELS,
            0);
    }

    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        HR(device->CreateShaderResourceView(
            texture.Get(), &srvDesc, m_diffuseTexture.ReleaseAndGetAddressOf()))
        context->GenerateMips(m_diffuseTexture.Get());
    }
}

void
ParticleSystem::CreateBlendState(ID3D11Device *device) {
    CD3D11_BLEND_DESC desc = CD3D11_BLEND_DESC{CD3D11_DEFAULT{}};
    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    // desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
    // desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    // desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    // desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    HR(device->CreateBlendState(&desc, m_blendState.ReleaseAndGetAddressOf()))
}

void
ParticleSystem::CreateDepthStencilState(ID3D11Device *device) {
    CD3D11_DEPTH_STENCIL_DESC desc =
        CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}};
    desc.DepthEnable = true;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    HR(device->CreateDepthStencilState(
        &desc, m_depthStencilState.ReleaseAndGetAddressOf()))
}

void
ParticleSystem::CreateVertexBuffer(ID3D11Device *device) {
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = &m_vertices[0];

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(Particle::Vertex) * MAX_PARTICLES * 4;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.StructureByteStride = sizeof(Particle::Vertex);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HR(device->CreateBuffer(
        &bufferDesc, &subresourceData, m_vertexBuffer.ReleaseAndGetAddressOf()))
}

void
ParticleSystem::CreateIndexBuffer(ID3D11Device *device) {
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = &m_indices[0];

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(uint32_t) * MAX_PARTICLES * 6;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HR(device->CreateBuffer(
        &bufferDesc, &subresourceData, m_indexBuffer.ReleaseAndGetAddressOf()))
}

void
ParticleSystem::CreateSamplerState(ID3D11Device *device) {
    CD3D11_SAMPLER_DESC desc = CD3D11_SAMPLER_DESC{CD3D11_DEFAULT{}};
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    HR(device->CreateSamplerState(&desc, m_sampler.ReleaseAndGetAddressOf()))
}

void
ParticleSystem::UpdateVertices() {
    m_vertices.clear();
    m_indices.clear();
    size_t indexOffset = 0;
    for (const Particle &p : m_particles) {
        m_vertices.insert(
            m_vertices.end(),
            p.GetVertices().begin(),
            p.GetVertices().end());
        for (int i = 0; i < 6; ++i) {
            m_indices.push_back(p.GetIndices()[i] + indexOffset);
        }
        indexOffset += 4;
    }
}

void
ParticleSystem::CreateEmitter() {
    const Vec3D accel = {0.0f, -9.8f, 0.0f};
    const Vec3D initVel = {};
    m_emitter = {
        ParticleType::Emitter, accel, initVel, m_options.origin, 0.0f, *this};
    EmitParticle();
}

void
ParticleSystem::EmitParticle() {
    Vec3D accel = m_options.accel;
    accel.X += MathRandom(-m_options.accelRandFact, m_options.accelRandFact);
    accel.Y += MathRandom(-m_options.accelRandFact, m_options.accelRandFact);
    accel.Z += MathRandom(-m_options.accelRandFact, m_options.accelRandFact);
    Vec3D initVel = m_options.initVel;
    initVel.X +=
        MathRandom(-m_options.initVelRandFact, m_options.initVelRandFact);
    initVel.Y +=
        MathRandom(-m_options.initVelRandFact, m_options.initVelRandFact);
    initVel.Z +=
        MathRandom(-m_options.initVelRandFact, m_options.initVelRandFact);
    Particle p = {ParticleType::Particle,
                  accel,
                  initVel,
                  m_emitter.GetInitPos(),
                  0,
                  *this};
    p.CreateQuad(m_options.particleSize.X,
                 m_options.particleSize.Y,
                 m_camera->GetUp(),
                 m_camera->GetRight());
    m_particles.push_back(p);
    m_vertices.insert(
        m_vertices.end(),
        p.GetVertices().begin(),
        p.GetVertices().end());
    m_indices.insert(
        m_indices.end(),
        p.GetIndices().begin(),
        p.GetIndices().end());
}

void
ParticleSystem::UpdateVertexBuffer(ID3D11DeviceContext *context) {
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        context->Map(m_vertexBuffer.Get(),
                     0,
                     D3D11_MAP_WRITE_DISCARD,
                     0,
                     &mappedResource);
        memcpy(mappedResource.pData,
               &m_vertices[0],
               sizeof(Particle::Vertex) * m_vertices.size());
        context->Unmap(m_vertexBuffer.Get(), 0);
    }
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        context->Map(m_indexBuffer.Get(),
                     0,
                     D3D11_MAP_WRITE_DISCARD,
                     0,
                     &mappedResource);
        memcpy(mappedResource.pData,
               &m_indices[0],
               sizeof(uint32_t) * m_indices.size());
        context->Unmap(m_indexBuffer.Get(), 0);
    }
}

Particle::Particle(ParticleType type,
                   const Vec3D &accel,
                   const Vec3D &initVel,
                   const Vec3D &initPos,
                   const float lifespan,
                   const ParticleSystem &ps)
    : m_accel{accel},
      m_initVel{initVel},
      m_initPos{initPos},
      m_pos{},
      m_lifespan{lifespan},
      m_type{type},
      m_particleSystem{&ps} {
}

Particle::Particle(const Particle &rhs) {
    m_accel = rhs.m_accel;
    m_initVel = rhs.m_initVel;
    m_initPos = rhs.m_initPos;
    m_pos = rhs.m_pos;
    m_lifespan = rhs.m_lifespan;
    m_type = rhs.m_type;
    m_vertices = rhs.m_vertices;
    m_indices = rhs.m_indices;
    m_particleSystem = rhs.m_particleSystem;
}

Particle &
Particle::operator=(const Particle &rhs) {
    if (this != &rhs) {
        m_accel = rhs.m_accel;
        m_initVel = rhs.m_initVel;
        m_initPos = rhs.m_initPos;
        m_pos = rhs.m_pos;
        m_lifespan = rhs.m_lifespan;
        m_type = rhs.m_type;
        m_vertices = rhs.m_vertices;
        m_indices = rhs.m_indices;
        m_particleSystem = rhs.m_particleSystem;
    }
    return *this;
}

void
Particle::CreateQuad(float width,
                     float height,
                     const Vec3D &up,
                     const Vec3D &right) {
    Vec3D R = right;
    MathVec3DNormalize(&R);
    Vec3D U = up;
    MathVec3DNormalize(&U);
    const Vec3D P = m_pos;

    Vec3D X = (width / 2.0f) * R;
    Vec3D Y = (width / 2.0f) * U;
    const Vec3D Q1 = P + X + Y;
    const Vec3D Q4 = P - X + Y; // q4
    const Vec3D Q3 = P - X - Y; // q3
    const Vec3D Q2 = P + X - Y; // q2

    m_vertices[0].Position = Q1; // top right
    m_vertices[1].Position = Q2; // bottom right
    m_vertices[2].Position = Q3; // bottom left
    m_vertices[3].Position = Q4; // top left
    m_vertices[0].TexCoords = {1.0f, 0.0f};
    m_vertices[1].TexCoords = {1.0f, 1.0f};
    m_vertices[2].TexCoords = {0.0f, 1.0f};
    m_vertices[3].TexCoords = {0.0f, 0.0f};
    m_vertices[0].Lifespan = m_lifespan;
    m_vertices[1].Lifespan = m_lifespan;
    m_vertices[2].Lifespan = m_lifespan;
    m_vertices[3].Lifespan = m_lifespan;

    m_indices[0] = 0;
    m_indices[1] = 1;
    m_indices[2] = 2;
    m_indices[3] = 2;
    m_indices[4] = 3;
    m_indices[5] = 0;
}

void
Particle::Tick(const float deltaTime) {
    m_pos = 0.5f * m_lifespan * m_lifespan * m_accel + m_lifespan * m_initVel +
            m_initPos;
    m_lifespan += deltaTime;
}

bool
Particle::IsAlive() const {
    return m_lifespan <
           static_cast<float>(m_particleSystem->GetOptions().lifespan);
}

void
Particle::Reset() {
    m_lifespan = 0.0f;
    m_pos = {};
}

void
ParticleSystem::ResetParticles() {
    m_particles.clear();
    m_emitter = {ParticleType::Emitter,
                 m_options.accel,
                 m_options.initVel,
                 m_options.origin,
                 0.0f,
                 *this};
    EmitParticle();
}

ParticleSystemOptions &
ParticleSystem::GetOptions() {
    return m_options;
}

const ParticleSystemOptions &
ParticleSystem::GetOptions() const {
    return m_options;
}

ParticleSystemOptions::ParticleSystemOptions()
    : isEnabled(true),
      lifespan(1),
      maxParticles(100),
      accelRandFact(0),
      initVelRandFact(0),
      burst(1),
      particleSize(1, 1),
      color(1, 1, 1, 1) {
}

ID3D11SamplerState *
ParticleSystem::GetSamplerState() const {
    return m_sampler.Get();
}

ID3D11ShaderResourceView *
ParticleSystem::GetSRV() const {
    return m_diffuseTexture.Get();
}

std::string
ParticleSystem::GetName() const {
    return m_name;
}

ID3D11BlendState *
ParticleSystem::GetBlendState() const {
    return m_blendState.Get();
}

ID3D11DepthStencilState *
ParticleSystem::GetDepthStencilState() const {
    return m_depthStencilState.Get();
}

ID3D11Buffer *
ParticleSystem::GetVertexBuffer() const {
    return m_vertexBuffer.Get();
}

ID3D11Buffer *
ParticleSystem::GetIndexBuffer() const {
    return m_indexBuffer.Get();
}

size_t
ParticleSystem::GetNumIndices() const {
    return m_indices.size();
}

void
ParticleSystem::Reset() {
    ResetParticles();
}

void
ParticleSystem::SetOptions(const ParticleSystemOptions &options) {
    m_options = options;
    ResetParticles();
}

const Vec3D &
Particle::GetAccel() const {
    return m_accel;
}

const Vec3D &
Particle::GetInitVel() const {
    return m_initVel;
}

const Vec3D &
Particle::GetInitPos() const {
    return m_initPos;
}

const Vec3D &
Particle::GetPos() const {
    return m_pos;
}

float
Particle::GetLifespan() const {
    return m_lifespan;
}

ParticleType
Particle::GetParticleType() const {
    return m_type;
}

const std::array<Particle::Vertex, 4> &
Particle::GetVertices() const {
    return m_vertices;
}

const std::array<uint32_t, 6> &
Particle::GetIndices() const {
    return m_indices;
}

Particle::Vertex::Vertex()
    : Lifespan(0) {
}

}
}
