#include "NEngine/Helpers/MeshGenerator.h"
#include "NEngine/Math/Math.h"

#include <cassert>
#include <corecrt_math_defines.h>

namespace NEngine {
namespace Helpers {
using namespace Math;

std::unique_ptr<Mesh>
MGGeneratePlane(const Vec3D *origin,
                const float width,
                const float height) {
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

    mesh->Positions.reserve(4);
    mesh->Positions.emplace_back(origin->X + width / 2.0f,
                                 origin->Y,
                                 origin->Z - height / 2.0f);
    mesh->Positions.emplace_back(origin->X - width / 2.0f,
                                 origin->Y,
                                 origin->Z - height / 2.0f);
    mesh->Positions.emplace_back(origin->X - width / 2.0f,
                                 origin->Y,
                                 origin->Z + height / 2.0f);
    mesh->Positions.emplace_back(origin->X + width / 2.0f,
                                 origin->Y,
                                 origin->Z + height / 2.0f);

    mesh->Normals.emplace_back(0.0f, 1.0f, 0.0f);

    mesh->TexCoords.reserve(4);
    const float multiplier = width > 10.0f ? width / 10.0f : 1.0f;
    mesh->TexCoords.emplace_back(1.0f * multiplier, 1.0f * multiplier);
    mesh->TexCoords.emplace_back(0.0f, 1.0f * multiplier);
    mesh->TexCoords.emplace_back(0.0f, 0.0f);
    mesh->TexCoords.emplace_back(1.0f * multiplier, 0.0f);

    mesh->Faces.reserve(6);
    mesh->Faces.emplace_back(0, 0, 0);
    mesh->Faces.emplace_back(1, 1, 0);
    mesh->Faces.emplace_back(2, 2, 0);
    mesh->Faces.emplace_back(2, 2, 0);
    mesh->Faces.emplace_back(3, 3, 0);
    mesh->Faces.emplace_back(0, 0, 0);

    return mesh;
}

std::unique_ptr<Mesh>
MGCreateSphere(float radius,
               unsigned int rings,
               unsigned int sectors) {
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    int r, s;

    std::vector<Vec3D> vertices;
    std::vector<Vec3D> normals;
    std::vector<Vec2D> texcoords;
    std::vector<Face> indices;

    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<Vec3D>::iterator v = vertices.begin();
    std::vector<Vec3D>::iterator n = normals.begin();
    std::vector<Vec2D>::iterator t = texcoords.begin();
    for (r = 0; r < rings; r++) {
        for (s = 0; s < sectors; s++) {
            float const y = sin(-M_PI_2 + M_PI * r * R);
            float const x =
                cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float const z =
                sin(2 * M_PI * s * S) * sin(M_PI * r * R);

            t->X = s * S;
            t->Y = r * R;
            ++t;

            v->X = x * radius;
            v->Y = y * radius;
            v->Z = z * radius;
            ++v;

            n->X = x;
            n->Y = y;
            n->Z = z;
            ++n;
        }
    }

    indices.resize(rings * sectors * 6);
    std::vector<Face>::iterator i = indices.begin();
    for (r = 0; r < rings; r++) {
        for (s = 0; s < sectors; s++) {
            // 0 1 2
            *i++ = {(r + 1) * sectors + (s + 1),
                    (r + 1) * sectors + (s + 1),
                    (r + 1) * sectors + (s + 1)}; // 2
            *i++ = {r * sectors + (s + 1), r * sectors + (s + 1),
                    r * sectors + (s + 1)}; // 1
            *i++ = {r * sectors + s, r * sectors + s,
                    r * sectors + s}; // 0

            // 0 2 3
            *i++ = {(r + 1) * sectors + s, (r + 1) * sectors + s,
                    (r + 1) * sectors + s}; // 3
            *i++ = {(r + 1) * sectors + (s + 1),
                    (r + 1) * sectors + (s + 1),
                    (r + 1) * sectors + (s + 1)}; // 2
            *i++ = {r * sectors + s, r * sectors + s,
                    r * sectors + s}; // 0
        }
    }

    std::unique_ptr<Mesh> meshData = std::make_unique<Mesh>();
    meshData->Faces = indices;
    meshData->Normals = normals;
    meshData->Positions = vertices;
    meshData->TexCoords = texcoords;
    meshData->Name = "Sphere";

    return meshData;
}
}
}
