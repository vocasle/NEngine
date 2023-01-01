#include "NEngine/Helpers/MeshGenerator.h"

#include <corecrt_math_defines.h>

#include <cassert>

#include "glm/vec2.hpp"

namespace NEngine {
namespace Helpers {

std::unique_ptr<Mesh>
MGGeneratePlane(const glm::vec3 *origin, const float width, const float height)
{
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

    mesh->Positions.reserve(4);
    mesh->Positions.emplace_back(
        origin->x + width / 2.0f, origin->y, origin->z - height / 2.0f);
    mesh->Positions.emplace_back(
        origin->x - width / 2.0f, origin->y, origin->z - height / 2.0f);
    mesh->Positions.emplace_back(
        origin->x - width / 2.0f, origin->y, origin->z + height / 2.0f);
    mesh->Positions.emplace_back(
        origin->x + width / 2.0f, origin->y, origin->z + height / 2.0f);

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
MGCreateSphere(float radius, unsigned int rings, unsigned int sectors)
{
    const float R = 1.0f / (float)(rings - 1);
    const float S = 1.0f / (float)(sectors - 1);
    int r, s;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<Face> indices;

    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<glm::vec3>::iterator v = vertices.begin();
    std::vector<glm::vec3>::iterator n = normals.begin();
    std::vector<glm::vec2>::iterator t = texcoords.begin();
    for (r = 0; r < rings; r++) {
        for (s = 0; s < sectors; s++) {
            const float y = sin(-M_PI_2 + M_PI * r * R);
            const float x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            const float z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

            t->x = s * S;
            t->y = r * R;
            ++t;

            v->x = x * radius;
            v->y = y * radius;
            v->z = z * radius;
            ++v;

            n->x = x;
            n->y = y;
            n->z = z;
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
                    (r + 1) * sectors + (s + 1)};  // 2
            *i++ = {r * sectors + (s + 1),
                    r * sectors + (s + 1),
                    r * sectors + (s + 1)};                              // 1
            *i++ = {r * sectors + s, r * sectors + s, r * sectors + s};  // 0

            // 0 2 3
            *i++ = {(r + 1) * sectors + s,
                    (r + 1) * sectors + s,
                    (r + 1) * sectors + s};  // 3
            *i++ = {(r + 1) * sectors + (s + 1),
                    (r + 1) * sectors + (s + 1),
                    (r + 1) * sectors + (s + 1)};                        // 2
            *i++ = {r * sectors + s, r * sectors + s, r * sectors + s};  // 0
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

std::unique_ptr<Mesh>
MGCreateCube(const glm::vec3 &origin, const float width)
{
    auto mesh = std::make_unique<Mesh>();
    mesh->Name = "Cube";

    // front
    mesh->Positions.push_back(
        glm::vec3(origin.x + width / 2,
                  origin.y - width / 2,
                  origin.z + width / 2));  // bottom right 0
    mesh->Positions.push_back(glm::vec3(origin.x + width / 2,
                                        origin.y + width / 2,
                                        origin.z + width / 2));  // top right 1
    mesh->Positions.push_back(glm::vec3(origin.x - width / 2,
                                        origin.y + width / 2,
                                        origin.z + width / 2));  // top left 2
    mesh->Positions.push_back(
        glm::vec3(origin.x - width / 2,
                  origin.y - width / 2,
                  origin.z + width / 2));  // bottom left 3

    // top
    mesh->Positions.push_back(
        glm::vec3(origin.x + width / 2,
                  origin.y + width / 2,
                  origin.z + width / 2));  // bottom right 4
    mesh->Positions.push_back(glm::vec3(origin.x + width / 2,
                                        origin.y + width / 2,
                                        origin.z - width / 2));  // top right 5
    mesh->Positions.push_back(glm::vec3(origin.x - width / 2,
                                        origin.y + width / 2,
                                        origin.z - width / 2));  // top left 6
    mesh->Positions.push_back(
        glm::vec3(origin.x - width / 2,
                  origin.y + width / 2,
                  origin.z + width / 2));  // bottom left 7

    // right
    mesh->Positions.push_back(
        glm::vec3(origin.x + width / 2,
                  origin.y - width / 2,
                  origin.z - width / 2));  // bottom right 8
    mesh->Positions.push_back(glm::vec3(origin.x + width / 2,
                                        origin.y + width / 2,
                                        origin.z - width / 2));  // top right 9
    mesh->Positions.push_back(glm::vec3(origin.x + width / 2,
                                        origin.y + width / 2,
                                        origin.z + width / 2));  // top left 10
    mesh->Positions.push_back(
        glm::vec3(origin.x + width / 2,
                  origin.y - width / 2,
                  origin.z + width / 2));  // bottom left 11

    mesh->Normals = {{0, 0, 1}, {0, 1, 0}, {1, 0, 0}};

    mesh->Indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11};

    return mesh;
}

}  // namespace Helpers
}  // namespace NEngine
