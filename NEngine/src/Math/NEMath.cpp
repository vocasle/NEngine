#include "NEngine/Math/NEMath.h"

#if NENGINE_USE_DIRECTXMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

namespace NEngine::Math {
mat4
RotateAxis(float radians, const vec3 &axis)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixRotationAxis(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&axis)), radians);
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
mat4
LookTo(const vec3 &camPos, const vec3 &target, const vec3 &up)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixLookToRH(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&camPos)),
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&target)),
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&up)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
mat4
LookAt(const vec3 &camPos, const vec3 &target, const vec3 &up)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixLookAtRH(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&camPos)),
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&target)),
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&up)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
mat4
PerspectiveFov(float fov, float aspectRatio, float zNear, float zFar)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixPerspectiveFovRH(fov, aspectRatio, zNear, zFar);
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}
mat4
QuatToMat(const vec4 &quat)
{
    auto ret = mat4();
#if NENGINE_USE_DIRECTXMATH
    const auto mat = XMMatrixRotationQuaternion(
        XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&quat)));
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4 *>(&ret), mat);
#endif
    return ret;
}

vec4
QuatSlerp(const vec4 &q1, const vec4 &q2, float t)
{
    auto ret = vec4();
#if NENGINE_USE_DIRECTXMATH
    const auto quat1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&q1));
    const auto quat2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4 *>(&q2));
    const auto res = XMQuaternionSlerp(quat1, quat2, t);
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), res);
#endif
    return ret;
}

vec4
QuatFromEuler(const vec3 &angles)
{
    auto ret = vec4(0.0f, 0.0f, 0.0f, 1.0f);
#if NENGINE_USE_DIRECTXMATH
    const auto quat = XMQuaternionRotationRollPitchYawFromVector(
        XMLoadFloat3(reinterpret_cast<const XMFLOAT3 *>(&angles)));
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), quat);
#endif
    return ret;
}

vec4
MatToQuat(const mat4 &rot_mat)
{
    auto ret = vec4(0.0f, 0.0f, 0.0f, 1.0f);
#if NENGINE_USE_DIRECTXMATH
    const auto quat = XMQuaternionRotationMatrix(
        XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4 *>(&rot_mat)));
    XMStoreFloat4(reinterpret_cast<XMFLOAT4 *>(&ret), quat);
#endif
    return ret;
}
}  // namespace NEngine::Math