#pragma once
#include <cstdint>
#include <cstring>
#include <sstream>

#include "Mat3X3.h"
#include "Mat4X4.h"
#include "Vec2D.h"
#include "Vec3D.h"

namespace NEngine {
namespace Math {

using mat4 = NEngine::Math::Mat4X4;
using mat3 = NEngine::Math::Mat3X3;
using vec2 = NEngine::Math::Vec2D;
using vec3 = NEngine::Math::Vec3D;
using vec4 = NEngine::Math::Vec4D;

namespace XM {
mat4 RotateAxis(float radians, const vec3 &axis);
mat4 Inverse(const mat4 &mat);
mat4 LookTo(const vec3 &camPos, const vec3 &target, const vec3 &up);
mat4 LookAt(const vec3 &camPos, const vec3 &target, const vec3 &up);
}  // namespace XM

// *** 2D vector math ***
Vec2D MathVec2DZero(void);

void MathVec2DModulateByVec2D(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DModulateByScalar(const Vec2D *vec1, float s, Vec2D *out);

void MathVec2DAddition(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DSubtraction(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

float MathVec2DDot(const Vec2D *vec1, const Vec2D *vec2);

void MathVec2DProj(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DPerp(const Vec2D *vec1, const Vec2D *vec2, Vec2D *out);

void MathVec2DNormalize(Vec2D *vec1);

// *** 3D vector math ***
Vec3D MathVec3DZero(void);

Vec3D MathVec3DFromXYZ(float x, float y, float z);

void MathVec3DModulateByVec3D(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out);

Vec3D MathVec3DModulateByScalar(const Vec3D *vec1, float s);

Vec3D MathVec3DAddition(const Vec3D *vec1, const Vec3D *vec2);

Vec3D MathVec3DSubtraction(const Vec3D *vec1, const Vec3D *vec2);

float MathVec3DDot(const Vec3D *vec1, const Vec3D *vec2);

void MathVec3DProj(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out);

void MathVec3DPerp(const Vec3D *vec1, const Vec3D *vec2, Vec3D *out);

Vec3D MathVec3DCross(const Vec3D *vec1, const Vec3D *vec2);

void MathVec3DNormalize(Vec3D *vec1);

void MathVec3DNegate(Vec3D *vec);

void MathVec3DPrint(const Vec3D *vec);

// *** 4D vector math ***
Vec4D MathVec4DZero(void);

Vec4D MathVec4DFromXYZW(float x, float y, float z, float w);

void MathVec4DModulateByVec4D(const Vec4D *vec1, const Vec4D *vec2, Vec4D *out);

void MathVec4DModulateByScalar(const Vec4D *vec1, float s, Vec4D *out);

Vec4D MathVec4DAddition(const Vec4D *vec1, const Vec4D *vec2);

void MathVec4DSubtraction(const Vec4D *vec1, const Vec4D *vec2, Vec4D *out);

float MathVec4DDot(const Vec4D *vec1, const Vec4D *vec2);

void MathVec4DNormalize(Vec4D *vec1);

void MathVec4DPrint(const Vec4D *vec);

Vec4D MathVec4DVectorPermute(Vec4D V1,
                             Vec4D V2,
                             uint32_t PermuteX,
                             uint32_t PermuteY,
                             uint32_t PermuteZ,
                             uint32_t PermuteW);

// *** 3X3 matrix math ***
Mat3X3 MathMat3X3Identity(void);

Mat3X3 MathMat3X3Addition(const Mat3X3 *mat1, const Mat3X3 *mat2);

void MathMat3X3ModulateByScalar(Mat3X3 *mat, float s);

Vec3D MathMat3X3MultByVec3D(const Mat3X3 *mat, const Vec3D *vec);

Mat3X3 MathMat3X3MultByMat3X3(const Mat3X3 *mat1, const Mat3X3 *mat2);

void MathMat3X3Transpose(Mat3X3 *mat);

void MathMat3X3Copy(const Mat3X3 *from, Mat3X3 *to);

// *** 4X4 matrix math ***
Mat4X4 MathMat4X4Identity(void);

void MathMat4X4Copy(const Mat4X4 *from, Mat4X4 *to);

void MathMat4X4ModulateByScalar(Mat4X4 *mat, float s);

Mat4X4 MathMat4X4Addition(const Mat4X4 *mat1, const Mat4X4 *mat2);

Vec4D MathMat4X4MultVec4DByMat4X4(const Vec4D *vec, const Mat4X4 *mat);

Mat4X4 MathMat4X4MultMat4X4ByMat4X4(const Mat4X4 *mat1, const Mat4X4 *mat2);

void MathMat4X4Transpose(Mat4X4 *mat);

Mat4X4 MathMat4X4ScaleFromVec3D(const Vec3D *scale);

Mat4X4 MathMat4X4TranslateFromVec3D(const Vec3D *offset);

Mat4X4 MathMat4X4RotateFromVec3D(const Vec3D *angles);

float MathMat4X4Determinant(const Mat4X4 *mat);

void MathMat4X4Normalize(Mat4X4 *mat);

Mat4X4 MathMat4X4Orthographic(float viewWidth,
                              float viewHeight,
                              float zNear,
                              float zFar);

Mat4X4 MathMat4X4ViewAt(const Vec3D *eyePos,
                        const Vec3D *eyeDir,
                        const Vec3D *upDir);

Mat4X4 MathMat4X4RotateZ(float angle);

Mat4X4 MathMat4X4RotateX(float angle);

Mat4X4 MathMat4X4RotateY(float angle);

Mat4X4 MathMat4X4PerspectiveFov(float fovAngleY,
                                float aspectRatio,
                                float nearZ,
                                float farZ);

Mat4X4 MathMat4X4OrthographicOffCenter(float viewLeft,
                                       float viewRight,
                                       float viewBottom,
                                       float viewTop,
                                       float nearZ,
                                       float farZ);

Mat4X4 MathMat4X4Inverse(const Mat4X4 *mat);

// *** misc math helpers ***
float MathClamp(float min, float max, float v);

float MathToRadians(float degrees);

float MathToDegrees(float radians);

unsigned int MathNearlyEqual(const float lhs, const float rhs);

int32_t MathIsNaN(float n);

float MathRandom(float min, float max);

Mat4X4 MathQuaternionToRotationMat(const Vec4D &quat);

Mat4X4 operator*(const Mat4X4 &lhs, const Mat4X4 &rhs);

Vec3D MathVec3DSubtraction(const Vec3D &vec1, const Vec3D &vec2);

Vec4D MathVec4DAddition(const Vec4D &vec1, const Vec4D &vec2);

float MathVec3DLength(const Vec3D &v);

Mat4X4 MathMat4X4Inverse(const Mat4X4 &mat);

float MathMat3X3Determinant(const Mat3X3 &mat);

}  // namespace Math

}  // namespace NEngine