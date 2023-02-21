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

mat4 RotateAxis(float radians, const vec3 &axis);
mat4 LookTo(const vec3 &camPos, const vec3 &target, const vec3 &up);
mat4 LookAt(const vec3 &camPos, const vec3 &target, const vec3 &up);
mat4 PerspectiveFov(float fov, float aspectRatio, float zNear, float zFar);
mat4 QuatToMat(const vec4 &quat);
vec4 QuatSlerp(const vec4 &q1, const vec4 &q2, float t);
vec4 QuatFromEuler(const vec3 &angles);
vec4 MatToQuat(const mat4 &rot_mat);
//vec3 Lerp(const vec3 &v1, const vec3 &v2, float t);
//vec4 Lerp(const vec4 &v1, const vec4 &v2, float t);

template <typename T>
T
Lerp(const T &v1, const T &v2, float t)
{
    return v1 * (1 - t) + v2 * t;
}

}  // namespace Math

}  // namespace NEngine
