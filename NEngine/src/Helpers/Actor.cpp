#include "NEngine/Helpers/Actor.h"
#include "NEngine/Helpers/MeshGenerator.h"
#include "NEngine/Utils/Utils.h"
#include "stb_image/stb_image.h"

namespace NEngine {
namespace Helpers {

using namespace Math;
using namespace Utils;

Actor::Actor()
    : m_IndexBuffer{nullptr},
      m_VertexBuffer{nullptr},
      m_Vertices{},
      m_Indices{},
      m_Rotation{MathMat4X4Identity()},
      m_Scale{MathMat4X4Identity()},
      m_Translation{MathMat4X4Identity()},
      m_DiffuseTexture{nullptr},
      m_SpecularTexture{nullptr},
      m_GlossTexture{nullptr},
      m_NormalTexture{nullptr},
      m_Material{},
      m_Name{},
      m_IsVisible{true} {
}

Actor::Actor(const Actor &actor) {
    m_IndexBuffer = actor.m_IndexBuffer;
    m_VertexBuffer = actor.m_VertexBuffer;
    m_Vertices = actor.m_Vertices;
    m_Indices = actor.m_Indices;
    m_Rotation = actor.m_Rotation;
    m_Scale = actor.m_Scale;
    m_Translation = actor.m_Translation;
    m_DiffuseTexture = actor.m_DiffuseTexture;
    m_SpecularTexture = actor.m_SpecularTexture;
    m_GlossTexture = actor.m_GlossTexture;
    m_NormalTexture = actor.m_NormalTexture;
    m_Material = actor.m_Material;
    m_Name = actor.m_Name;
    m_IsVisible = actor.m_IsVisible;
}

Actor &
Actor::operator=(const Actor &actor) {
    if (this != &actor) {
        Actor temp(actor);
        temp.Swap(*this);
    }
    return *this;
}

Actor::Actor(Actor &&actor) noexcept {
    actor.Swap(*this);
}

Actor &
Actor::operator=(Actor &&actor) noexcept {
    Actor temp(std::move(actor));
    temp.Swap(*this);
    return *this;
}

Actor::Actor(Mesh *mesh)
    : Actor() {
    LoadMesh(mesh);
}

Actor::~Actor() {
}

ID3D11ShaderResourceView **
Actor::GetShaderResources() const {
    static ID3D11ShaderResourceView *shaderResources[4] = {};
    shaderResources[0] = m_DiffuseTexture.Get();
    shaderResources[1] = m_SpecularTexture.Get();
    shaderResources[2] = m_GlossTexture.Get();
    shaderResources[3] = m_NormalTexture.Get();
    return shaderResources;
}

void
Actor::SetIsVisible(const bool isVisible) {
    m_IsVisible = isVisible;
}

void
Actor::SetName(const std::string &name) {
    m_Name = name;
}

void
Actor::Swap(Actor &actor) {
    std::swap(m_Rotation, actor.m_Rotation);
    std::swap(m_Scale, actor.m_Scale);
    std::swap(m_Translation, actor.m_Translation);
    std::swap(m_Vertices, actor.m_Vertices);
    std::swap(m_Indices, actor.m_Indices);
    std::swap(m_Material, actor.m_Material);
    std::swap(m_DiffuseTexture, actor.m_DiffuseTexture);
    std::swap(m_SpecularTexture, actor.m_SpecularTexture);
    std::swap(m_GlossTexture, actor.m_GlossTexture);
    std::swap(m_NormalTexture, actor.m_NormalTexture);
    std::swap(m_IndexBuffer, actor.m_IndexBuffer);
    std::swap(m_VertexBuffer, actor.m_VertexBuffer);
    std::swap(m_Name, actor.m_Name);
    std::swap(m_IsVisible, actor.m_IsVisible);
}

void
Actor::LoadMesh(Mesh *mesh) {
    m_Vertices.reserve(mesh->Faces.size() + m_Vertices.size());
    m_Indices.reserve(mesh->Faces.size() + m_Indices.size());

    Vertex vert = {};

    for (uint32_t j = 0; j < mesh->Faces.size(); ++j) {
        const Face &face = mesh->Faces[j];
        const Vec3D &pos = mesh->Positions[face.posIdx];
        const Vec3D &norm = mesh->Normals[face.normIdx];
        const Vec2D &tc = mesh->TexCoords[face.texIdx];
        vert.Position = pos;
        vert.Normal = norm;
        vert.TexCoords = tc;

        assert(m_Indices.size() + 1 <= mesh->Faces.size());
        m_Indices.emplace_back(m_Indices.size());
        assert(m_Vertices.size() + 1 <= mesh->Faces.size());
        m_Vertices.emplace_back(vert);
    }

    GenerateTangents();
}

void
Actor::GenerateTangents() {
    std::vector<Vec3D> tan1(m_Vertices.size());
    std::vector<Vec3D> tan2(m_Vertices.size());

    for (long a = 0; a < m_Indices.size(); a += 3) {
        long i1 = m_Indices[a];
        long i2 = m_Indices[a + 1];
        long i3 = m_Indices[a + 2];
        const Vec3D &v1 = m_Vertices[i1].Position;
        const Vec3D &v2 = m_Vertices[i2].Position;
        const Vec3D &v3 = m_Vertices[i3].Position;
        const Vec2D &w1 = m_Vertices[i1].TexCoords;
        const Vec2D &w2 = m_Vertices[i2].TexCoords;
        const Vec2D &w3 = m_Vertices[i3].TexCoords;
        float x1 = v2.X - v1.X;
        float x2 = v3.X - v1.X;
        float y1 = v2.Y - v1.Y;
        float y2 = v3.Y - v1.Y;
        float z1 = v2.Z - v1.Z;
        float z2 = v3.Z - v1.Z;
        float s1 = w2.X - w1.X;
        float s2 = w3.X - w1.X;
        float t1 = w2.Y - w1.Y;
        float t2 = w3.Y - w1.Y;
        float r = 1.0F / (s1 * t2 - s2 * t1);
        Vec3D sdir((t2 * x1 - t1 * x2) * r,
                   (t2 * y1 - t1 * y2) * r,
                   (t2 * z1 - t1 * z2) * r);
        Vec3D tdir((s1 * x2 - s2 * x1) * r,
                   (s1 * y2 - s2 * y1) * r,
                   (s1 * z2 - s2 * z1) * r);
        tan1[i1] = MathVec3DAddition(&tan1[i1], &sdir);
        tan1[i2] = MathVec3DAddition(&tan1[i2], &sdir);
        tan1[i3] = MathVec3DAddition(&tan1[i3], &sdir);
        tan2[i1] = MathVec3DAddition(&tan2[i1], &tdir);
        tan2[i2] = MathVec3DAddition(&tan2[i2], &tdir);
        tan2[i3] = MathVec3DAddition(&tan2[i3], &tdir);
    }

    for (long a = 0; a < m_Vertices.size(); a++) {
        const Vec3D &n = m_Vertices[a].Normal;
        const Vec3D &t = tan1[a];
        // Gram-Schmidt orthogonalize.
        Vec3D tmp = MathVec3DSubtraction(t, n);
        MathVec3DModulateByScalar(&tmp, MathVec3DDot(&n, &t));
        MathVec3DNormalize(&tmp);
        m_Vertices[a].Tangent = /*Vec4D(tmp, 0.0f);*/ tmp;
        // Calculate handedness.
        tmp = MathVec3DCross(&n, &t);
        m_Vertices[a].Bitangent = MathVec3DCross(
            &m_Vertices[a].Normal,
            &m_Vertices[a].Tangent);
        MathVec3DModulateByScalar(
            &m_Vertices[a].Bitangent,
            (MathVec3DDot(&tmp, &tan2[a]) < 0.0F) ? -1.0F : 1.0F);
    }
}

void
Actor::LoadModel(const char *filename) {
    std::unique_ptr<Model> model = OLLoad(filename);
    if (!model) {
        UTILS_FATAL_ERROR("Failed to load model %s", filename);
    }

    size_t numFaces = 0;
    for (uint32_t i = 0; i < model->Meshes.size(); ++i) {
        const Mesh *mesh = &model->Meshes[i];
        numFaces += mesh->Faces.size();
    }

    m_Vertices.reserve(numFaces);
    m_Indices.reserve(numFaces);

    size_t posOffs = 0;
    size_t normOffs = 0;
    size_t tcOffs = 0;
    Vertex vert = {};

    for (uint32_t i = 0; i < model->Meshes.size(); ++i) {
        const Mesh *mesh = &model->Meshes[i];
        for (uint32_t j = 0; j < mesh->Faces.size(); ++j) {
            const Face &face = model->Meshes[i].Faces[j];
            const Vec3D &pos =
                mesh->Positions[face.posIdx - posOffs];
            const Vec3D &norm =
                mesh->Normals[face.normIdx - normOffs];
            const Vec2D &tc = mesh->TexCoords[face.texIdx - tcOffs];

            vert.Position = pos;
            vert.Normal = norm;
            vert.TexCoords = tc;

            m_Indices.emplace_back(m_Indices.size());
            m_Vertices.emplace_back(vert);
        }
        posOffs += mesh->Positions.size();
        normOffs += mesh->Normals.size();
        tcOffs += mesh->TexCoords.size();
    }

    GenerateTangents();
}

void
Actor::CreateVertexBuffer(ID3D11Device *device) {
    UtilsCreateVertexBuffer(device,
                            &m_Vertices[0],
                            m_Vertices.size(),
                            sizeof(Vertex),
                            m_VertexBuffer.ReleaseAndGetAddressOf());
}

void
Actor::CreateIndexBuffer(ID3D11Device *device) {
    UtilsCreateIndexBuffer(device,
                           &m_Indices[0],
                           m_Indices.size(),
                           m_IndexBuffer.ReleaseAndGetAddressOf());
}

void
Actor::Translate(const Vec3D &offset) {
    m_Translation = MathMat4X4TranslateFromVec3D(&offset);
}

void
Actor::Rotate(const float pitch, const float yaw, const float roll) {
    const Vec3D angles = {pitch, yaw, roll};
    m_Rotation = MathMat4X4RotateFromVec3D(&angles);
}

void
Actor::Scale(const float s) {
    const Vec3D scale = {s, s, s};
    m_Scale = MathMat4X4ScaleFromVec3D(&scale);
}

void
Actor::Scale(const float x, const float y, const float z) {
    const Vec3D scale = {x, y, z};
    m_Scale = MathMat4X4ScaleFromVec3D(&scale);
}

void
Actor::LoadTexture(const char *filename,
                   enum TextureType type,
                   ID3D11Device *device,
                   ID3D11DeviceContext *context) {
    int width = 0;
    int height = 0;
    int channelsInFile = 0;
    const int desiredChannels = 4;

    unsigned char *bytes = stbi_load(filename,
                                     &width,
                                     &height,
                                     &channelsInFile,
                                     desiredChannels);
    if (!bytes) {
        UtilsDebugPrint("ERROR: Failed to load texture from %s\n",
                        filename);
        ExitProcess(EXIT_FAILURE);
    }
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 0;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
                         D3D11_BIND_RENDER_TARGET;
        desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        HR(device->CreateTexture2D(&desc, nullptr,
            texture.ReleaseAndGetAddressOf()))

        context->UpdateSubresource(
            texture.Get(),
            0,
            nullptr,
            bytes,
            width * sizeof(uint8_t) * desiredChannels,
            0);
    }

    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        memset(&srvDesc, 0, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        switch (type) {
            case TextureType::Diffuse:
                HR(device->CreateShaderResourceView(
                    texture.Get(), &srvDesc,
                    m_DiffuseTexture.ReleaseAndGetAddressOf()))
                context->GenerateMips(m_DiffuseTexture.Get());
                break;
            case TextureType::Specular:
                HR(device->CreateShaderResourceView(
                    texture.Get(), &srvDesc,
                    m_SpecularTexture.ReleaseAndGetAddressOf()));
                context->GenerateMips(m_SpecularTexture.Get());
                break;
            case TextureType::Gloss:
                HR(device->CreateShaderResourceView(
                    texture.Get(), &srvDesc,
                    m_GlossTexture.ReleaseAndGetAddressOf()));
                context->GenerateMips(m_GlossTexture.Get());
                break;
            case TextureType::Normal:
                HR(device->CreateShaderResourceView(
                    texture.Get(), &srvDesc,
                    m_NormalTexture.ReleaseAndGetAddressOf()));
                context->GenerateMips(m_NormalTexture.Get());
                break;
            default:
                break;
        }
    }

    stbi_image_free(bytes);
}

void
Actor::SetMaterial(const Material *material) {
    m_Material = *material;
}

void
Actor::SetTexture(ID3D11ShaderResourceView *srv, TextureType type) {
    switch (type) {
        case TextureType::Diffuse:
            m_DiffuseTexture = srv;
            break;
        case TextureType::Gloss:
            m_GlossTexture = srv;
            break;
        case TextureType::Normal:
            m_NormalTexture = srv;
            break;
        case TextureType::Specular:
            m_SpecularTexture = srv;
            break;
    }
}

void
Actor::SetTextures(ID3D11ShaderResourceView *srv[4]) {
    m_DiffuseTexture = srv[0];
    m_SpecularTexture = srv[1];
    m_GlossTexture = srv[2];
    m_NormalTexture = srv[3];
}

Mat4X4
Actor::GetWorld() const {
    Mat4X4 world = MathMat4X4MultMat4X4ByMat4X4(&m_Scale, &m_Rotation);
    world = MathMat4X4MultMat4X4ByMat4X4(&world, &m_Translation);
    return world;
}

}
}
