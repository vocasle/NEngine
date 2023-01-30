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
}  // namespace NEngine::Math