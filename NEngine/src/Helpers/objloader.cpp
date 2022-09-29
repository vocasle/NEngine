#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NEngine/Math/Math.h"
#include "NEngine/Helpers/objloader.h"

namespace NEngine {
namespace Helpers {
void
OLLogInfo(const char *fmt, ...) {
#if OBJLOADER_VERBOSE
			static char out[512];
			va_list args;
			va_start(args, fmt);
			vsprintf_s(out, 512, fmt, args);
			va_end(args);
			fprintf(stdout, "%s\n", out);
#endif
}

void
OLLogError(const char *fmt, ...) {
    static char out[512];
    va_list args;
    va_start(args, fmt);
    vsprintf_s(out, 512, fmt, args);
    va_end(args);
    fprintf(stderr, "ERROR: %s\n", out);
}

uint32_t
OLReadLine(FILE *f, char *out, uint32_t sz) {
    uint32_t len = 0;
    char ch = 0;
    while (sz--) {
        if ((ch = fgetc(f)) != EOF) {
            if (ch == '\n') {
                break;
            }
            out[len++] = ch;
        }
    }
    out[len] = 0;
    return len;
}

struct MeshInfo {
    MeshInfo()
        : NumPositions{0},
          NumNormals{0},
          NumTexCoords{0},
          NumFaces{0} {
    }

    uint32_t NumPositions;
    uint32_t NumNormals;
    uint32_t NumTexCoords;
    uint32_t NumFaces;
};

static uint32_t
OLGetNumMeshes(FILE *objfile) {
    uint32_t numMeshes = 0;
    char line[128];
    while (OLReadLine(objfile, line, 128)) {
        if (line[0] == 'o') {
            ++numMeshes;
        }
    }
    rewind(objfile);
    return numMeshes;
}

static uint32_t
OLNumFacesInLine(const char *line) {
    uint32_t numSlashes = 0;
    while (*line++) {
        if (*line == '/') {
            ++numSlashes;
        }
    }
    assert(numSlashes % 2 == 0 && "Mesh is not triangulated");
    return numSlashes / 2;
}

static void
OLGetMeshInfos(std::vector<MeshInfo> &infos, FILE *objfile) {
    char line[128];
    int32_t meshIdx = -1;
    while (OLReadLine(objfile, line, 128)) {
        if (strstr(line, "o ")) {
            ++meshIdx;
        } else if (strstr(line, "v ")) {
            infos[meshIdx].NumPositions++;
        } else if (strstr(line, "vt")) {
            infos[meshIdx].NumTexCoords++;
        } else if (strstr(line, "vn")) {
            infos[meshIdx].NumNormals++;
        } else if (strstr(line, "f ")) {
            infos[meshIdx].NumFaces += OLNumFacesInLine(
                line); // we expect 3 faces in row
        }
    }
    assert(meshIdx + 1 == infos.size());
    rewind(objfile);
}

static void
OLParseMeshes(std::vector<Mesh> &meshes,
              std::vector<MeshInfo> &infos,
              FILE *objfile) {
    char line[128];
    char prefix[4];
    float vec[3];
    uint32_t idx[9];
    int32_t result = 0;
    int32_t meshIdx = -1;
    Mesh *mesh = nullptr;
    MeshInfo *info = nullptr;
    Face face = {};

    while (OLReadLine(objfile, line, 128)) {
        strncpy_s(prefix, 4, line, 2);
        if (strcmp(prefix, "o ") == 0) {
            ++meshIdx;
            mesh = &meshes[meshIdx];
            info = &infos[meshIdx];
            mesh->Name = std::string(line + 2);
        } else if (strcmp(prefix, "v ") == 0) {
            result = sscanf_s(line,
                              "%s%f%f%f",
                              prefix,
                              4,
                              vec,
                              vec + 1,
                              vec + 2);
            assert(result == 4);
            mesh->Positions.emplace_back(vec[0], vec[1], vec[2]);
            assert(mesh->Positions.size() <= info->NumPositions);
            OLLogInfo("Position { %f %f %f }",
                      vec[0],
                      vec[1],
                      vec[2]);
        } else if (strcmp(prefix, "vt") == 0) {
            result = sscanf_s(line,
                              "%s%f%f",
                              prefix,
                              4,
                              vec,
                              vec + 1);
            assert(result == 3);
            mesh->TexCoords.emplace_back(vec[0], vec[1]);
            assert(mesh->TexCoords.size() <= info->NumTexCoords);
            OLLogInfo("TexCoord { pref: %s %f %f }",
                      prefix,
                      vec[0],
                      vec[1]);
        } else if (strcmp(prefix, "vn") == 0) {
            result = sscanf_s(line,
                              "%s%f%f%f",
                              prefix,
                              4,
                              vec,
                              vec + 1,
                              vec + 2);
            assert(result == 4);
            mesh->Normals.emplace_back(vec[0], vec[1], vec[2]);
            assert(mesh->Normals.size() <= info->NumNormals);
            OLLogInfo("Normal { %f %f %f }",
                      vec[0],
                      vec[1],
                      vec[2]);
        } else if (strcmp(prefix, "f ") == 0) {
            result = sscanf_s(line,
                              "%s%d/%d/%d%d/%d/%d%d/%d/%d",
                              prefix,
                              4,
                              idx,
                              idx + 1,
                              idx + 2,
                              idx + 3,
                              idx + 4,
                              idx + 5,
                              idx + 6,
                              idx + 7,
                              idx + 8);
            assert(result == 10);
            OLLogInfo("Face { %d/%d/%d %d/%d/%d %d/%d/%d }",
                      idx[0],
                      idx[1],
                      idx[2],
                      idx[3],
                      idx[4],
                      idx[5],
                      idx[6],
                      idx[7],
                      idx[8]);
            for (uint32_t i = 0; i < 9; i += 3) {
                face.posIdx = idx[i] - 1;
                face.texIdx = idx[i + 1] - 1;
                face.normIdx = idx[i + 2] - 1;
                mesh->Faces.emplace_back(face);
                assert(mesh->Faces.size() <= info->NumFaces);
            }
        } else {
            OLLogInfo("Skip %s, because unknown prefix: %s\n",
                      line,
                      prefix);
        }
    }
}

static uint32_t
OLValidateMeshes(const std::vector<Mesh> &meshes,
                 const std::vector<MeshInfo> &infos) {
    uint32_t areMeshesValid = 1;
    for (uint32_t i = 0; i < meshes.size(); ++i) {
        const Mesh *m = &meshes[i];
        const MeshInfo *mi = &infos[i];
        areMeshesValid = areMeshesValid &&
                         (m->Faces.size() == mi->NumFaces);
        assert(m->Faces.size() == mi->NumFaces);
        areMeshesValid = areMeshesValid &&
                         (m->Normals.size() == mi->NumNormals);
        assert(m->Normals.size() == mi->NumNormals);
        areMeshesValid = areMeshesValid &&
                         (m->TexCoords.size() == mi->NumTexCoords);
        assert(m->TexCoords.size() == mi->NumTexCoords);
        areMeshesValid = areMeshesValid &&
                         (m->Positions.size() == mi->NumPositions);
        assert(m->Positions.size() == mi->NumPositions);
    }
    return areMeshesValid;
}

static std::string
OLGetCwd(const char *filename) {
    const char *lastSlash = NULL;
    uint32_t len = 0;
    lastSlash = strrchr(filename, '/');
    lastSlash = lastSlash ? lastSlash : strrchr(filename, '\\');
    if (lastSlash) {
        len = lastSlash - filename;
    }
    std::string dir;
    if (len > 0) {
        dir = std::string(filename, len);
        return dir;
    } else {
        dir = filename;
    }
    return dir;
}

void
OLDumpModelToFile(const struct Model *model, const char *filename) {
    FILE *f = NULL;
    fopen_s(&f, filename, "w");
    if (!f) {
        OLLogError("Failed to create file %s", filename);
    }

    fprintf(f,
            "NumMeshes: %llu\nDirectory: %s\n",
            model->Meshes.size(),
            model->Directory.c_str());
    for (uint32_t i = 0; i < model->Meshes.size(); ++i) {
        const Mesh *m = &model->Meshes[i];
        fprintf(f, "MeshName: %s\n", m->Name.c_str());

        fprintf(f, "NumPositions: %llu\n", m->Positions.size());
        for (uint32_t j = 0; j < m->Positions.size(); ++j) {
            fprintf(f,
                    "Position { %f %f %f }\n",
                    m->Positions[j].X,
                    m->Positions[j].Y,
                    m->Positions[j].Z);
        }

        fprintf(f, "NumTexCoords: %llu\n", m->TexCoords.size());
        for (uint32_t j = 0; j < m->TexCoords.size(); ++j) {
            fprintf(f,
                    "TexCoord { %f %f }\n",
                    m->TexCoords[j].X,
                    m->TexCoords[j].Y);
        }

        fprintf(f, "NumNormals: %llu\n", m->Normals.size());
        for (uint32_t j = 0; j < m->Normals.size(); ++j) {
            fprintf(f,
                    "Normal { %f %f %f }\n",
                    m->Normals[j].X,
                    m->Normals[j].Y,
                    m->Normals[j].Z);
        }

        fprintf(f, "NumFaces: %llu\n", m->Faces.size());
        for (uint32_t j = 0; j < m->Faces.size(); ++j) {
            fprintf(f,
                    "Face { %d %d %d }\n",
                    m->Faces[j].posIdx,
                    m->Faces[j].texIdx,
                    m->Faces[j].normIdx);
        }
    }
    fclose(f);
}

std::unique_ptr<Model>
OLLoad(const char *filename) {
    FILE *f = NULL;
    fopen_s(&f, filename, "r");
    if (!f) {
        OLLogError("Failed to open %s", filename);
        return nullptr;
    }

    const uint32_t numMeshes = OLGetNumMeshes(f);
    if (numMeshes == 0) {
        OLLogError("No meshes found in %s", filename);
        return nullptr;
    }

    std::vector<Mesh> meshes(numMeshes);
    std::vector<MeshInfo> infos(numMeshes);

    OLGetMeshInfos(infos, f);
    for (uint32_t i = 0; i < numMeshes; ++i) {
        meshes[i].Faces.reserve(infos[i].NumFaces);
        meshes[i].Normals.reserve(infos[i].NumNormals);
        meshes[i].Positions.reserve(infos[i].NumPositions);
        meshes[i].TexCoords.reserve(infos[i].NumTexCoords);
    }
    OLParseMeshes(meshes, infos, f);
    fclose(f);

    assert(OLValidateMeshes(meshes, infos));

    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Meshes = meshes;
    model->Directory = OLGetCwd(filename);
    // OLDumpModelToFile(model, "model.txt");

    return model;
}

}
}
