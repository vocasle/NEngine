#include <d3d11.h>
#include <wrl/client.h>

#include <array>
#include <string>
#include <vector>

#include "Camera.h"
#include "NEngine/Math/Math.h"

namespace NEngine {
namespace Helpers {
enum class ParticleType {
    Particle,
    Emitter
};

#define MAX_PARTICLES 10000

class ParticleSystem;

class Particle {
public:
    struct Vertex {
        Vertex();
        Math::Vec3D Position;
        Math::Vec2D TexCoords;
        float Lifespan;
    };

    Particle(ParticleType type,
             const Math::Vec3D &accel,
             const Math::Vec3D &initVel,
             const Math::Vec3D &initPos,
             float lifespan,
             const ParticleSystem &ps);
    Particle(const Particle &rhs);
    Particle &operator=(const Particle &rhs);
    void CreateQuad(float width,
                    float height,
                    const Math::Vec3D &up,
                    const Math::Vec3D &right);
    [[nodiscard]] const Math::Vec3D &GetAccel() const;
    [[nodiscard]] const Math::Vec3D &GetInitVel() const;
    [[nodiscard]] const Math::Vec3D &GetInitPos() const;
    [[nodiscard]] const Math::Vec3D &GetPos() const;
    [[nodiscard]] float GetLifespan() const;
    [[nodiscard]] ParticleType GetParticleType() const;
    [[nodiscard]] const std::array<Vertex, 4> &GetVertices() const;
    [[nodiscard]] const std::array<uint32_t, 6> &GetIndices() const;
    void Tick(float deltaTime);
    [[nodiscard]] bool IsAlive() const;
    void Reset();

private:
    Math::Vec3D m_accel;
    Math::Vec3D m_initVel;
    Math::Vec3D m_initPos;
    Math::Vec3D m_pos;
    float m_lifespan;
    ParticleType m_type;
    std::array<Vertex, 4> m_vertices;
    std::array<uint32_t, 6> m_indices;
    const ParticleSystem *m_particleSystem;
};

struct ParticleSystemOptions {
    ParticleSystemOptions();
    bool isEnabled;
    float lifespan;
    int maxParticles;
    float accelRandFact;
    float initVelRandFact;
    int burst{};
    Math::Vec3D initVel;
    Math::Vec3D accel;
    Math::Vec3D origin;
    Math::Vec2D particleSize;
    Math::Vec4D color;
};

class ParticleSystem {
public:
    ParticleSystem(const std::string &name,
                   const Math::Vec3D &origin,
                   const Math::Vec3D &acceleration,
                   const Math::Vec3D &initVel,
                   const Camera &camera);
    ~ParticleSystem() = default;

    void Init(ID3D11Device *device,
              ID3D11DeviceContext *context,
              const std::string &texFilePath);
    void Tick(float deltaTime);
    void UpdateVertexBuffer(ID3D11DeviceContext *context);
    void Reset();

    [[nodiscard]] ID3D11BlendState *GetBlendState() const;
    [[nodiscard]] ID3D11DepthStencilState *GetDepthStencilState() const;
    [[nodiscard]] ID3D11Buffer *GetVertexBuffer() const;
    [[nodiscard]] ID3D11Buffer *GetIndexBuffer() const;
    [[nodiscard]] size_t GetNumIndices() const;
    [[nodiscard]] std::string GetName() const;
    [[nodiscard]] ID3D11ShaderResourceView *GetSRV() const;
    [[nodiscard]] ID3D11SamplerState *GetSamplerState() const;
    [[nodiscard]] size_t GetNumAliveParticles() const;

    ParticleSystemOptions &GetOptions();
    [[nodiscard]] const ParticleSystemOptions &GetOptions() const;

    void SetOptions(const ParticleSystemOptions &options);

private:
    void CreateTexture(ID3D11Device *device,
                       ID3D11DeviceContext *context,
                       const std::string &filepath);
    void CreateBlendState(ID3D11Device *device);
    void CreateDepthStencilState(ID3D11Device *device);
    void CreateVertexBuffer(ID3D11Device *device);
    void CreateIndexBuffer(ID3D11Device *device);
    void CreateSamplerState(ID3D11Device *device);
    void UpdateVertices();
    void CreateEmitter();
    void EmitParticle();
    void ResetParticles();

    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_diffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
    std::string m_name;
    std::vector<Particle> m_particles;
    std::vector<Particle::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    Particle m_emitter;
    const Camera *m_camera;
    ParticleSystemOptions m_options;
};

}
}
