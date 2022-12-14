#include "NEngine/Math/Math.h"

#include <assert.h>
#include <corecrt_math_defines.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#include <string>
#endif

namespace NEngine {
namespace Math {
#define EPSILON 0.00001f

// In this math helper file all matrices are row major matrices
// In other words Vec4D is 1x4 matrix
//
// This means that we have to multiply Mat4X4 by Vec4D to transform

unsigned int
MathNearlyEqual(const float lhs, const float rhs)
{
    float delta = lhs - rhs;
    return fabs(delta) <= EPSILON;
}

int32_t
MathIsNaN(float n)
{
    return n != n;
}

float
MathRandom(float min, float max)
{
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

Vec2D
MathVec2DZero(void)
{
    const Vec2D vec = {0.0f, 0.0f};
    return vec;
}

Vec3D
MathVec3DZero(void)
{
    const Vec3D vec = {0.0f, 0.0f, 0.0f};
    return vec;
}

Vec3D
MathVec3DFromXYZ(float x, float y, float z)
{
    Vec3D v = {x, y, z};
    return v;
}

Vec4D
MathVec4DZero()
{
    const Vec4D vec = {0.0f, 0.0f, 0.0f, 0.0f};
    return vec;
}

Vec4D
MathVec4DFromXYZW(float x, float y, float z, float w)
{
    Vec4D v = {x, y, z, w};
    return v;
}

Mat4X4
MathMat4X4Identity()
{
    Mat4X4 mat = {};
    mat.A00 = mat.A11 = mat.A22 = mat.A33 = 1.0f;
    return mat;
}

void
MathMat4X4Copy(const Mat4X4 *from, Mat4X4 *to)
{
    for (unsigned char i = 0; i < 4; ++i)
        to->V[i] = from->V[i];
}

void
MathMat4X4ModulateByScalar(Mat4X4 *mat, const float s)
{
    MathVec4DModulateByScalar(&mat->V[0], s, &mat->V[0]);
    MathVec4DModulateByScalar(&mat->V[1], s, &mat->V[1]);
    MathVec4DModulateByScalar(&mat->V[2], s, &mat->V[2]);
    MathVec4DModulateByScalar(&mat->V[3], s, &mat->V[3]);
}

Mat4X4
MathMat4X4Addition(const Mat4X4 *mat1, const Mat4X4 *mat2)
{
    Mat4X4 res = {};
    res.V[0] = MathVec4DAddition(&mat1->V[0], &mat2->V[0]);
    res.V[1] = MathVec4DAddition(&mat1->V[1], &mat2->V[1]);
    res.V[2] = MathVec4DAddition(&mat1->V[2], &mat2->V[2]);
    res.V[3] = MathVec4DAddition(&mat1->V[3], &mat2->V[3]);
    return res;
}

Vec4D
MathMat4X4MultVec4DByMat4X4(const Vec4D *vec, const Mat4X4 *mat)
{
    Vec4D res = {};
    res.X = vec->X * mat->A00 + vec->Y * mat->A10 + vec->Z * mat->A20 +
            vec->W * mat->A30;
    res.Y = vec->X * mat->A01 + vec->Y * mat->A11 + vec->Z * mat->A21 +
            vec->W * mat->A31;
    res.Z = vec->X * mat->A02 + vec->Y * mat->A12 + vec->Z * mat->A22 +
            vec->W * mat->A32;
    res.W = vec->X * mat->A03 + vec->Y * mat->A13 + vec->Z * mat->A23 +
            vec->W * mat->A33;
    return res;
}

Mat4X4
MathMat4X4MultMat4X4ByMat4X4(const Mat4X4 *mat1, const Mat4X4 *mat2)
{
    Mat4X4 res = {};
    float x = mat1->A00;
    float y = mat1->A01;
    float z = mat1->A02;
    float w = mat1->A03;

    res.A00 = x * mat2->A00 + y * mat2->A10 + z * mat2->A20 + w * mat2->A30;
    res.A01 = x * mat2->A01 + y * mat2->A11 + z * mat2->A21 + w * mat2->A31;
    res.A02 = x * mat2->A02 + y * mat2->A12 + z * mat2->A22 + w * mat2->A32;
    res.A03 = x * mat2->A03 + y * mat2->A13 + z * mat2->A23 + w * mat2->A33;

    x = mat1->A10;
    y = mat1->A11;
    z = mat1->A12;
    w = mat1->A13;

    res.A10 = x * mat2->A00 + y * mat2->A10 + z * mat2->A20 + w * mat2->A30;
    res.A11 = x * mat2->A01 + y * mat2->A11 + z * mat2->A21 + w * mat2->A31;
    res.A12 = x * mat2->A02 + y * mat2->A12 + z * mat2->A22 + w * mat2->A32;
    res.A13 = x * mat2->A03 + y * mat2->A13 + z * mat2->A23 + w * mat2->A33;

    x = mat1->A20;
    y = mat1->A21;
    z = mat1->A22;
    w = mat1->A23;

    res.A20 = x * mat2->A00 + y * mat2->A10 + z * mat2->A20 + w * mat2->A30;
    res.A21 = x * mat2->A01 + y * mat2->A11 + z * mat2->A21 + w * mat2->A31;
    res.A22 = x * mat2->A02 + y * mat2->A12 + z * mat2->A22 + w * mat2->A32;
    res.A23 = x * mat2->A03 + y * mat2->A13 + z * mat2->A23 + w * mat2->A33;

    x = mat1->A30;
    y = mat1->A31;
    z = mat1->A32;
    w = mat1->A33;

    res.A30 = x * mat2->A00 + y * mat2->A10 + z * mat2->A20 + w * mat2->A30;
    res.A31 = x * mat2->A01 + y * mat2->A11 + z * mat2->A21 + w * mat2->A31;
    res.A32 = x * mat2->A02 + y * mat2->A12 + z * mat2->A22 + w * mat2->A32;
    res.A33 = x * mat2->A03 + y * mat2->A13 + z * mat2->A23 + w * mat2->A33;

    return res;
}

void
MathMat4X4Transpose(Mat4X4 *mat)
{
    Mat4X4 out = {};
    for (unsigned char i = 0; i < 4; ++i)
        for (unsigned char j = 0; j < 4; ++j)
            out.A[i][j] = mat->A[j][i];
    *mat = out;
}

Mat4X4
MathMat4X4ScaleFromVec3D(const Vec3D *scale)
{
    Mat4X4 out = MathMat4X4Identity();
    out.A00 = scale->X;
    out.A11 = scale->Y;
    out.A22 = scale->Z;
    return out;
}

Mat4X4
MathMat4X4TranslateFromVec3D(const Vec3D *offset)
{
    Mat4X4 out = MathMat4X4Identity();
    out.A30 = offset->X;
    out.A31 = offset->Y;
    out.A32 = offset->Z;
    return out;
}

// Rotates about y-axis (Angles.y), then x-axis (Angles.x), then z-axis
// (Angles.z) taken from
// https://github.com/microsoft/DirectXMath/blob/cbcf8786ca55a0e48015399f443de7b63529f8da/Inc/DirectXMathMatrix.inl#L1697
Mat4X4
MathMat4X4RotateFromVec3D(const Vec3D *angles)
{
    const float cp = cosf(angles->X);
    const float sp = sinf(angles->X);

    const float cy = cosf(angles->Y);
    const float sy = sinf(angles->Y);

    const float cr = cosf(angles->Z);
    const float sr = sinf(angles->Z);

    Mat4X4 out = MathMat4X4Identity();

    out.A[0][0] = cr * cy + sr * sp * sy;
    out.A[0][1] = sr * cp;
    out.A[0][2] = sr * sp * cy - cr * sy;
    out.A[0][3] = 0.0f;

    out.A[1][0] = cr * sp * sy - sr * cy;
    out.A[1][1] = cr * cp;
    out.A[1][2] = sr * sy + cr * sp * cy;
    out.A[1][3] = 0.0f;

    out.A[2][0] = cp * sy;
    out.A[2][1] = -sp;
    out.A[2][2] = cp * cy;
    out.A[2][3] = 0.0f;

    out.A[3][0] = 0.0f;
    out.A[3][1] = 0.0f;
    out.A[3][2] = 0.0f;
    out.A[3][3] = 1.0f;

    return out;
}

float
MathMat4X4Determinant(const Mat4X4 *mat)
{
    return mat->A00 * (mat->A11 * (mat->A22 * mat->A33 - mat->A32 * mat->A23) -
                       mat->A21 * (mat->A12 * mat->A33 - mat->A32 * mat->A13) +
                       mat->A31 * (mat->A12 * mat->A23 - mat->A22 * mat->A13)) -
           mat->A10 * (mat->A01 * (mat->A22 * mat->A33 - mat->A32 * mat->A23) -
                       mat->A21 * (mat->A02 * mat->A33 - mat->A32 * mat->A03) +
                       mat->A31 * (mat->A02 * mat->A23 - mat->A22 * mat->A03)) +
           mat->A20 * (mat->A01 * (mat->A12 * mat->A33 - mat->A32 * mat->A13) -
                       mat->A11 * (mat->A02 * mat->A33 - mat->A32 * mat->A03) +
                       mat->A31 * (mat->A02 * mat->A13 - mat->A12 * mat->A03)) -
           mat->A30 * (mat->A01 * (mat->A12 * mat->A23 - mat->A22 * mat->A13) -
                       mat->A11 * (mat->A02 * mat->A23 - mat->A22 * mat->A03) +
                       mat->A21 * (mat->A02 * mat->A13 - mat->A12 * mat->A03));
}

void
MathMat4X4Normalize(Mat4X4 *mat)
{
    const float det = MathMat4X4Determinant(mat);
    for (unsigned char i = 0; i < 16; ++i)
        (&mat->A00)[i] /= det;
}

Mat4X4
MathMat4X4Orthographic(float viewWidth,
                       float viewHeight,
                       float zNear,
                       float zFar)
{
    assert(!MathNearlyEqual(viewWidth, 0.0f));
    assert(!MathNearlyEqual(viewHeight, 0.0f));
    assert(!MathNearlyEqual(zFar, zNear));

    Mat4X4 m = MathMat4X4Identity();
    // m.A00 = 2.0f / viewWidth;
    // m.A11 = 2.0f / viewHeight;
    m.A22 = 2.0f / (zFar - zNear);
    m.A23 = (zFar + zNear) / (zFar - zNear);
    return m;
}

Mat4X4
MathMat4X4ViewAt(const Vec3D *eyePos, const Vec3D *eyeDir, const Vec3D *upDir)
{
    Mat4X4 res = {};
    Vec3D dir = *eyeDir;
    MathVec3DNormalize(&dir);

    Vec3D right = MathVec3DCross(upDir, &dir);
    MathVec3DNormalize(&right);

    Vec3D up = MathVec3DCross(&dir, &right);
    MathVec3DNormalize(&up);

    res.A00 = right.X;
    res.A10 = right.Y;
    res.A20 = right.Z;
    res.A30 = -MathVec3DDot(eyePos, &right);

    res.A01 = up.X;
    res.A11 = up.Y;
    res.A21 = up.Z;
    res.A31 = -MathVec3DDot(eyePos, &up);

    res.A02 = dir.X;
    res.A12 = dir.Y;
    res.A22 = dir.Z;
    res.A32 = -MathVec3DDot(eyePos, &dir);

    res.A33 = 1.0f;

    return res;
}

Mat4X4
MathMat4X4RotateZ(const float angle)
{
    Mat4X4 rot = MathMat4X4Identity();
    rot.A00 = cosf(angle);
    rot.A01 = -sinf(angle);
    rot.A10 = sinf(angle);
    rot.A11 = cosf(angle);

    return rot;
}

Mat4X4
MathMat4X4RotateX(const float angle)
{
    Mat4X4 rot = MathMat4X4Identity();
    rot.A11 = cosf(angle);
    rot.A12 = -sinf(angle);
    rot.A21 = sinf(angle);
    rot.A22 = cosf(angle);

    return rot;
}

Mat4X4
MathMat4X4RotateY(float angle)
{
    Mat4X4 rot = MathMat4X4Identity();
    rot.A00 = cosf(angle);
    rot.A02 = sinf(angle);
    rot.A20 = -sinf(angle);
    rot.A22 = cosf(angle);

    return rot;
}

Mat4X4
MathMat4X4PerspectiveFov(float fovAngleY,
                         float aspectRatio,
                         float nearZ,
                         float farZ)
{
    Mat4X4 res = {};
    const float projPlaneZ = 1.0f / tanf(fovAngleY / 2.0f);
    res.A00 = 1.0f / aspectRatio * projPlaneZ;
    res.A11 = projPlaneZ;
    res.A22 = -farZ / (farZ - nearZ);
    res.A23 = -1.0f;
    res.A32 = -(nearZ * farZ) / (farZ - nearZ);
    return res;
}

Mat4X4
MathMat4X4OrthographicOffCenter(float viewLeft,
                                float viewRight,
                                float viewBottom,
                                float viewTop,
                                float nearZ,
                                float farZ)
{
    assert(!MathNearlyEqual(viewRight, viewLeft));
    assert(!MathNearlyEqual(viewTop, viewBottom));
    assert(!MathNearlyEqual(farZ, nearZ));

    float reciprocalWidth = 1.0f / (viewRight - viewLeft);
    float reciprocalHeight = 1.0f / (viewTop - viewBottom);
    float fRange = 1.0f / (farZ - nearZ);

    Mat4X4 m = {};
    m.A[0][0] = reciprocalWidth + reciprocalWidth;
    m.A[0][1] = 0.0f;
    m.A[0][2] = 0.0f;
    m.A[0][3] = 0.0f;

    m.A[1][0] = 0.0f;
    m.A[1][1] = reciprocalHeight + reciprocalHeight;
    m.A[1][2] = 0.0f;
    m.A[1][3] = 0.0f;

    m.A[2][0] = 0.0f;
    m.A[2][1] = 0.0f;
    m.A[2][2] = fRange;
    m.A[2][3] = 0.0f;

    m.A[3][0] = -(viewLeft + viewRight) * reciprocalWidth;
    m.A[3][1] = -(viewTop + viewBottom) * reciprocalHeight;
    m.A[3][2] = -fRange * nearZ;
    m.A[3][3] = 1.0f;
    return m;
}

bool
gluInvertMatrix(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
             m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] +
             m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] +
             m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
             m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] +
              m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] +
              m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] +
             m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] +
             m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
             m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
             m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
              m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
             m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
             m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
              m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
              m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
             m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
              m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

Mat4X4
MathMat4X4Inverse(const Mat4X4 *mat)
{
    Mat4X4 inv = {};
    gluInvertMatrix(&mat->A00, &inv.A00);
    return inv;
}

float
MathClamp(float min, float max, float v)
{
    if (v > max)
        return max;
    if (v < min)
        return min;
    return v;
}

float
MathToRadians(float degrees)
{
    return degrees * (float)(M_PI / 180.0);
}

float
MathToDegrees(float radians)
{
    return radians * (float)(180.0 / M_PI);
}

void
MathVec2DModulateByVec2D(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out)
{
    out->X = vec1->X * vec2->X;
    out->Y = vec1->Y * vec2->Y;
}

void
MathVec2DModulateByScalar(const Vec2D *vec1, const float s, Vec2D *out)
{
    out->X = vec1->X * s;
    out->Y = vec1->Y * s;
}

void
MathVec3DModulateByVec3D(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out)
{
    out->X = vec1->X * vec2->X;
    out->Y = vec1->Y * vec2->Y;
    out->Z = vec1->Z * vec2->Z;
}

Vec3D
MathVec3DModulateByScalar(const Vec3D *vec1, const float s)
{
    Vec3D out = {};
    out.X = vec1->X * s;
    out.Y = vec1->Y * s;
    out.Z = vec1->Z * s;
    return out;
}

void
MathVec4DModulateByVec4D(const Vec4D *vec1, const Vec4D *vec2, Vec4D *out)
{
    out->X = vec1->X * vec2->X;
    out->Y = vec1->Y * vec2->Y;
    out->Z = vec1->Z * vec2->Z;
    out->W = vec1->W * vec2->W;
}

void
MathVec4DModulateByScalar(const Vec4D *vec1, const float s, Vec4D *out)
{
    out->X = vec1->X * s;
    out->Y = vec1->Y * s;
    out->Z = vec1->Z * s;
    out->W = vec1->W * s;
}

void
MathVec2DAddition(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out)
{
    out->X = vec1->X + vec2->X;
    out->Y = vec1->Y + vec2->Y;
}

void
MathVec2DSubtraction(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out)
{
    out->X = vec1->X - vec2->X;
    out->Y = vec1->Y - vec2->Y;
}

float
MathVec2DDot(const Vec2D *vec1, const Vec2D *vec2)
{
    return vec1->X * vec2->X + vec1->Y * vec2->Y;
}

void
MathVec2DProj(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out)
{
    const float vec1vec2dot = MathVec2DDot(vec1, vec2);
    const float vec2dot = MathVec2DDot(vec2, vec2);
    const float r = vec1vec2dot / vec2dot;
    MathVec2DModulateByScalar(vec2, r, out);
}

void
MathVec2DPerp(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out)
{
    MathVec2DProj(vec1, vec2, out);
    MathVec2DSubtraction(vec1, out, out);
}

void
MathVec2DNormalize(Vec2D *vec1)
{
    const float norm = sqrtf(MathVec2DDot(vec1, vec1));
    vec1->X /= norm;
    vec1->Y /= norm;
}

Vec3D
MathVec3DAddition(const Vec3D *vec1, const Vec3D *vec2)
{
    Vec3D out = {};
    out.X = vec1->X + vec2->X;
    out.Y = vec1->Y + vec2->Y;
    out.Z = vec1->Z + vec2->Z;
    return out;
}

Vec3D
MathVec3DSubtraction(const Vec3D *vec1, const Vec3D *vec2)
{
    Vec3D out = {};
    out.X = vec1->X - vec2->X;
    out.Y = vec1->Y - vec2->Y;
    out.Z = vec1->Z - vec2->Z;
    return out;
}

float
MathVec3DDot(const Vec3D *vec1, const Vec3D *vec2)
{
    return vec1->X * vec2->X + vec1->Y * vec2->Y + vec1->Z * vec2->Z;
}

void
MathVec3DProj(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out)
{
    const float vec1vec2dot = MathVec3DDot(vec1, vec2);
    const float vec2dot = MathVec3DDot(vec2, vec2);
    const float r = vec1vec2dot / vec2dot;
    *out = MathVec3DModulateByScalar(vec2, r);
}

void
MathVec3DPerp(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out)
{
    MathVec3DProj(vec1, vec2, out);
    *out = MathVec3DSubtraction(vec1, out);
}

Vec3D
MathVec3DCross(const Vec3D *vec1, const Vec3D *vec2)
{
    Vec3D out = {};
    out.X = vec1->Y * vec2->Z - vec1->Z * vec2->Y;
    out.Y = vec1->Z * vec2->X - vec1->X * vec2->Z;
    out.Z = vec1->X * vec2->Y - vec1->Y * vec2->X;
    return out;
}

void
MathVec3DNormalize(Vec3D *vec1)
{
    const float norm = sqrtf(MathVec3DDot(vec1, vec1));
    vec1->X /= norm;
    vec1->Y /= norm;
    vec1->Z /= norm;
}

void
MathVec3DNegate(Vec3D *vec)
{
    vec->X = -vec->X;
    vec->Y = -vec->Y;
    vec->Z = -vec->Z;
}

void
MathVec3DPrint(const Vec3D *vec)
{
    printf("{ %f %f %f }\n", vec->X, vec->Y, vec->Z);
}

Vec4D
MathVec4DAddition(const Vec4D *vec1, const Vec4D *vec2)
{
    Vec4D res = {};
    res.X = vec1->X + vec2->X;
    res.Y = vec1->Y + vec2->Y;
    res.Z = vec1->Z + vec2->Z;
    res.W = vec1->W + vec2->W;
    return res;
}

void
MathVec4DSubtraction(const Vec4D *vec1, const Vec4D *vec2, Vec4D *out)
{
    out->X = vec1->X - vec2->X;
    out->Y = vec1->Y - vec2->Y;
    out->Z = vec1->Z - vec2->Z;
    out->W = vec1->W - vec2->W;
}

float
MathVec4DDot(const Vec4D *vec1, const Vec4D *vec2)
{
    return vec1->X * vec2->X + vec1->Y * vec2->Y + vec1->Z * vec2->Z +
           vec1->W * vec2->W;
}

void
MathVec4DNormalize(Vec4D *vec1)
{
    const float norm = sqrtf(MathVec4DDot(vec1, vec1));
    vec1->X /= norm;
    vec1->Y /= norm;
    vec1->Z /= norm;
    vec1->W /= norm;
}

void
MathVec4DPrint(const Vec4D *vec)
{
    printf("{ %f %f %f %f }\n", vec->X, vec->Y, vec->Z, vec->W);
}

Vec4D
MathVec4DVectorPermute(Vec4D V1,
                       Vec4D V2,
                       uint32_t PermuteX,
                       uint32_t PermuteY,
                       uint32_t PermuteZ,
                       uint32_t PermuteW)
{
    assert(PermuteX <= 7 && PermuteY <= 7 && PermuteZ <= 7 && PermuteW <= 7);

    const uint32_t *aPtr[2];
    aPtr[0] = reinterpret_cast<const uint32_t *>(&V1);
    aPtr[1] = reinterpret_cast<const uint32_t *>(&V2);

    Vec4D Result;
    auto pWork = reinterpret_cast<uint32_t *>(&Result);

    const uint32_t i0 = PermuteX & 3;
    const uint32_t vi0 = PermuteX >> 2;
    pWork[0] = aPtr[vi0][i0];

    const uint32_t i1 = PermuteY & 3;
    const uint32_t vi1 = PermuteY >> 2;
    pWork[1] = aPtr[vi1][i1];

    const uint32_t i2 = PermuteZ & 3;
    const uint32_t vi2 = PermuteZ >> 2;
    pWork[2] = aPtr[vi2][i2];

    const uint32_t i3 = PermuteW & 3;
    const uint32_t vi3 = PermuteW >> 2;
    pWork[3] = aPtr[vi3][i3];

    return Result;
}

#ifdef MATH_TEST
void
TestVec2D(void)
{
    Vec2D vec1 = MathVec2DZero();
    assert(MathNearlyEqual(vec1.X, 0.0f) && MathNearlyEqual(vec1.Y, 0.0f));

    vec1.X = vec1.Y = 1.0f;
    MathVec2DModulateByScalar(&vec1, 10.0f, &vec1);
    assert(MathNearlyEqual(vec1.X, 10.0f) && MathNearlyEqual(vec1.Y, 10.0f));

    Vec2D vec2 = {0.5f, 2.0f};
    MathVec2DModulateByVec2D(&vec1, &vec2, &vec1);
    assert(MathNearlyEqual(vec1.X, 5.0f) && MathNearlyEqual(vec1.Y, 20.0f));

    MathVec2DAddition(&vec1, &vec2, &vec2);
    assert(MathNearlyEqual(vec2.X, 5.5f) && MathNearlyEqual(vec2.Y, 22.0f));

    MathVec2DSubtraction(&vec1, &vec2, &vec1);
    assert(MathNearlyEqual(vec1.X, -0.5f) && MathNearlyEqual(vec1.Y, -2.0f));

    const float dot = MathVec2DDot(&vec1, &vec2);
    assert(MathNearlyEqual(dot, -46.75f));

    vec2.X = 10.0f;
    vec2.Y = 5.0f;
    Vec2D vec3;
    MathVec2DProj(&vec1, &vec2, &vec3);
    assert(MathNearlyEqual(vec3.X, -1.2f) && MathNearlyEqual(vec3.Y, -0.6f));

    MathVec2DPerp(&vec1, &vec2, &vec3);
    assert(MathNearlyEqual(vec3.X, 0.7f) && MathNearlyEqual(vec3.Y, -1.4f));

    MathVec2DNormalize(&vec1);
    assert(MathNearlyEqual(vec1.X, -0.242536f) &&
           MathNearlyEqual(vec1.Y, -0.970143f));
}

void
TestVec3D(void)
{
    Vec3D vec1 = MathVec3DZero();
    assert(vec1.X == 0.0f && vec1.Y == 0.0f);

    vec1.X = vec1.Y = 1.0f;
    vec1 = MathVec3DModulateByScalar(&vec1, 10.0f);
    assert(vec1.X == 10.0f && vec1.Y == 10.0f);

    Vec3D vec2 = vec1;
    MathVec3DModulateByVec3D(&vec1, &vec2, &vec1);
    assert(vec1.X == 100.0f && vec1.Y == 100.0f);

    vec2 = MathVec3DAddition(&vec1, &vec2);
    assert(vec2.X == 110.0f && vec2.Y == 110.0f);

    vec1 = MathVec3DSubtraction(&vec1, &vec2);
    assert(MathNearlyEqual(vec1.X, -10.0f) && MathNearlyEqual(vec1.Y, -10.0f));

    const float dot = MathVec3DDot(&vec1, &vec2);
    assert(MathNearlyEqual(dot, -2200.0f));

    vec2.X = 10.0f;
    vec2.Y = 5.0f;
    Vec3D vec3;
    MathVec3DProj(&vec1, &vec2, &vec3);
    assert(MathNearlyEqual(vec3.X, -12.0f) && MathNearlyEqual(vec3.Y, -6.0f));

    MathVec3DPerp(&vec1, &vec2, &vec3);
    assert(MathNearlyEqual(vec3.X, 2.0f) && MathNearlyEqual(vec3.Y, -4.0f));

    const float r = 1.0f / sqrtf(2.0f);
    MathVec3DNormalize(&vec1);
    assert(MathNearlyEqual(vec1.X, -r) && MathNearlyEqual(vec1.Y, -r));

    vec3 = MathVec3DCross(&vec1, &vec2);
    assert(MathNearlyEqual(vec3.X, 0.0f) && MathNearlyEqual(vec3.Y, 0.0f) &&
           MathNearlyEqual(vec3.Z, 3.53553391f));
}

void
TestMat3X3(void)
{
    Mat3X3 mat1 = MathMat3X3Identity();
    assert(mat1.A00 == 1.0f && mat1.A11 == 1.0f && mat1.A22 == 1.0f);
    MathMat3X3ModulateByScalar(&mat1, 10.0f);
    assert(MathNearlyEqual(mat1.A00, 10.0f) &&
           MathNearlyEqual(mat1.A11, 10.0f) &&
           MathNearlyEqual(mat1.A22, 10.0f));

    Mat3X3 mat2 = MathMat3X3Identity();
    MathMat3X3Addition(&mat1, &mat2);
    assert(MathNearlyEqual(mat1.A00, 10.0f) &&
           MathNearlyEqual(mat1.A11, 10.0f) &&
           MathNearlyEqual(mat1.A22, 10.0f));

    mat1 = MathMat3X3Identity();
    MathMat3X3ModulateByScalar(&mat1, 2.5f);
    Vec3D out = {1.0f, 3.0f, 2.0f};
    MathMat3X3MultByVec3D(&mat1, &out);
    assert(MathNearlyEqual(out.X, 1.0f) && MathNearlyEqual(out.Y, 3.0f) &&
           MathNearlyEqual(out.Z, 2.0f));

    mat1 = MathMat3X3Identity();
    for (unsigned char i = 0; i < 3; ++i) {
        mat1.V[i].X = (float)i * 2.0f + 1.0f;
        mat1.V[i].Y = (float)i * 2.0f + 2.0f;
        mat1.V[i].Z = (float)i * 2.0f + 3.0f;
    }
    MathMat3X3Copy(&mat1, &mat2);
    for (unsigned char i = 0; i < 3; ++i)
        assert(MathNearlyEqual(mat1.V[i].X, mat2.V[i].X) &&
               MathNearlyEqual(mat1.V[i].Y, mat2.V[i].Y) &&
               MathNearlyEqual(mat1.V[i].Z, mat2.V[i].Z));
    MathMat3X3Transpose(&mat1);
    assert(MathNearlyEqual(mat1.A10, mat2.A01) &&
           MathNearlyEqual(mat1.A20, mat2.A02) &&
           MathNearlyEqual(mat1.A21, mat2.A12));

    mat2 = MathMat3X3MultByMat3X3(&mat1, &mat2);
    mat1.V[0].X = 35.0f;
    mat1.V[0].Y = mat1.V[1].X = 44.0f;
    mat1.V[0].Z = mat1.V[2].X = 53.0f;
    mat1.V[1].Y = 56.0f;
    mat1.V[1].Z = mat1.V[2].Y = 68.0f;
    mat1.V[2].Z = 83.0f;
    for (unsigned char i = 0; i < 9; ++i)
        assert(MathNearlyEqual((&mat1.A00)[i], (&mat2.A00)[i]));

#if __cplusplus
    Mat3X3 mat = {};
    mat.V[0] = Vec3D(1, 2, 3);
    mat.V[1] = Vec3D(1, 2, 3);
    mat.V[2] = Vec3D(1, 2, 3);
    assert(MathNearlyEqual(mat.Determinant(), 0));

    mat.V[0] = Vec3D(1, 2, 3);
    mat.V[1] = Vec3D(3, 2, 1);
    mat.V[2] = Vec3D(5, 2, 3);
    assert(MathNearlyEqual(mat.Determinant(), -16));
#endif
}

void
TestMat4X4(void)
{
    Vec4D point = {1.0f, 0.0f, 0.0f, 1.0f};
    const Vec3D offsetXBy4 = {4.0f, 0.0f, 0.0f};
    const Mat4X4 matOffsetXBy4 = MathMat4X4TranslateFromVec3D(&offsetXBy4);

    point = MathMat4X4MultVec4DByMat4X4(&point, &matOffsetXBy4);
    assert(MathNearlyEqual(point.X, 5.0f) && MathNearlyEqual(point.Y, 0.0f) &&
           MathNearlyEqual(point.Z, 0.0f) && MathNearlyEqual(point.W, 1.0f));

    point.Y = -1.0f;
    const Vec3D offsetYByNeg3 = {0.0f, -3.0f, 0.0f};
    const Mat4X4 matOffsetYByNeg3 =
        MathMat4X4TranslateFromVec3D(&offsetYByNeg3);
    point = MathMat4X4MultVec4DByMat4X4(&point, &matOffsetYByNeg3);
    assert(MathNearlyEqual(point.X, 5.0f) && MathNearlyEqual(point.Y, -4.0f) &&
           MathNearlyEqual(point.Z, 0.0f) && MathNearlyEqual(point.W, 1.0f));

    point.Z = 0.25f;
    const Vec3D offsetZBy2 = {0.0f, 0.0f, 2.0f};
    const Mat4X4 matOffsetZBy2 = MathMat4X4TranslateFromVec3D(&offsetZBy2);
    point = MathMat4X4MultVec4DByMat4X4(&point, &matOffsetZBy2);
    assert(MathNearlyEqual(point.X, 5.0f) && MathNearlyEqual(point.Y, -4.0f) &&
           MathNearlyEqual(point.Z, 2.25f) && MathNearlyEqual(point.W, 1.0f));

    point.X = point.Y = point.Z = 1.0f;
    const Vec3D offset = {1.0f, 3.0f, 4.0f};
    const Mat4X4 matOffset = MathMat4X4TranslateFromVec3D(&offset);
    point = MathMat4X4MultVec4DByMat4X4(&point, &matOffset);
    assert(MathNearlyEqual(point.X, 2.0f) && MathNearlyEqual(point.Y, 4.0f) &&
           MathNearlyEqual(point.Z, 5.0f) && MathNearlyEqual(point.W, 1.0f));

    Mat4X4 mat1 = MathMat4X4Identity();
    Mat4X4 mat2 = MathMat4X4Identity();
    Mat4X4 res = MathMat4X4MultMat4X4ByMat4X4(&mat1, &mat2);
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 4; ++j) {
            if (i != j)
                assert(MathNearlyEqual(res.A[i][j], 0.0f));
            else
                assert(MathNearlyEqual(res.A[i][j], 1.0f));
        }
    }

    mat1.A00 = mat1.A11 = mat1.A22 = 2.0f;
    mat2.A30 = mat2.A31 = mat2.A32 = 4.0f;
    res = MathMat4X4MultMat4X4ByMat4X4(&mat1, &mat2);

    const Vec4D v = {1.0f, 1.0f, 1.0f, 1.0f};
    const Vec4D vec1 = MathMat4X4MultVec4DByMat4X4(&v, &res);
    Vec4D vec2 = MathMat4X4MultVec4DByMat4X4(&v, &mat1);
    vec2 = MathMat4X4MultVec4DByMat4X4(&vec2, &mat2);
    assert(MathNearlyEqual(vec1.X, vec2.X) && MathNearlyEqual(vec1.Y, vec2.Y) &&
           MathNearlyEqual(vec1.Z, vec2.Z) && MathNearlyEqual(vec1.W, vec2.W));
}

void
MathTest(void)
{
    TestVec2D();
    TestVec3D();
    TestMat3X3();
    TestMat4X4();
}
#endif

#ifdef __cplusplus
Mat4X4
operator*(const Mat4X4 &lhs, const Mat4X4 &rhs)
{
    return MathMat4X4MultMat4X4ByMat4X4(&lhs, &rhs);
}

Vec3D
MathVec3DSubtraction(const Vec3D &vec1, const Vec3D &vec2)
{
    return MathVec3DSubtraction(&vec1, &vec2);
}

Vec4D
MathVec4DAddition(const Vec4D &vec1, const Vec4D &vec2)
{
    return MathVec4DAddition(&vec1, &vec2);
}

float
MathVec3DLength(const Vec3D &v)
{
    return sqrtf(MathVec3DDot(&v, &v));
}

Mat4X4
MathMat4X4Inverse(const Mat4X4 &mat)
{
    return MathMat4X4Inverse(&mat);
}

std::string
Mat4X4::ToString() const
{
    std::stringstream out;
    out << "{\n";
    for (uint32_t i = 0; i < 4; ++i) {
        const Vec4D &v = V[i];
        out << "\t" << v.ToString() << ",\n";
    }
    out << "}";
    return out.str();
}

bool
Mat4X4::operator==(const Mat4X4 &rhs) const
{
    return V[0] == rhs.V[0] && V[1] == rhs.V[1] && V[2] == rhs.V[2] &&
           V[3] == rhs.V[3];
}

#endif

// TODO: This must be tested. Calculations were performed for column major that
// is why I do transpose. All my code uses row major
Mat4X4
MathQuaternionToRotationMat(const Vec4D &quat)
{
    Mat4X4 mat = MathMat4X4Identity();
    mat.A00 = 1 - 2 * quat.Y * quat.Y - 2 * quat.Z * quat.Z;
    mat.A01 = 2 * quat.X * quat.Y - 2 * quat.W * quat.Z;
    mat.A02 = 2 * quat.X * quat.Z + 2 * quat.W * quat.Y;

    mat.A10 = 2 * quat.X * quat.Y + 2 * quat.W * quat.Z;
    mat.A11 = 1 - 2 * quat.X * quat.X - 2 * quat.Z * quat.Z;
    mat.A12 = 2 * quat.Y * quat.Z - 2 * quat.W * quat.X;

    mat.A20 = 2 * quat.X * quat.Z - 2 * quat.W * quat.Y;
    mat.A21 = 2 * quat.Y * quat.Z + 2 * quat.W * quat.X;
    mat.A22 = 1 - 2 * quat.X * quat.X - 2 * quat.Y * quat.Y;

    MathMat4X4Transpose(&mat);

    return mat;
}

}  // namespace Math

}  // namespace NEngine
