#pragma once

#include <stdint.h>

#include "DirectXMath.h"
#include "NEngine/Math/Math.h"


namespace NEngine {
namespace Helpers {
#define CAMERA_SENSITIVITY 0.01f
#define MOUSE_SENSITIVITY 0.0001f

class Camera
{
public:
    Camera();
    Camera(const Math::Vec3D &cameraPos);
    Math::Mat4X4 GetViewMat() const;
    Math::Mat4X4 GetProjMat() const;
    void ProcessKeyboard(double deltaMillis);
    void ProcessMouse(double deltaMillis);
    void SetPosition(DirectX::XMFLOAT3 position);
    void SetPitch(float pitch);
    void SetYaw(float yaw);

    Math::Vec3D
    GetPos() const
    {
        return m_Pos;
    }

    Math::Vec3D
    GetAt() const
    {
        return m_At;
    }

    Math::Vec3D
    GetUp() const
    {
        return m_Up;
    }

    Math::Vec3D
    GetRight() const
    {
        return m_Right;
    }

    void SetViewDimensions(uint32_t width, uint32_t height);
    void SetZNear(const float zNear);
    void SetZFar(const float zFar);
    void LookAt(const Math::Vec3D &pos,
                const Math::Vec3D &target,
                const Math::Vec3D &up);
    void SetFov(float fov);

    float
    GetZFar() const
    {
        return m_zFar;
    }

    float
    GetZNear() const
    {
        return m_zNear;
    }

    void Tick(double deltaMillis);

private:
    void UpdateVectors();
    void UpdateSpeed();
    void ResetCamera();

    float m_Pitch;
    float m_Yaw;
    Math::Vec3D m_Pos;
    Math::Vec3D m_At;
    Math::Vec3D m_Right;
    Math::Vec3D m_Up;
    float m_Speed;
    float m_backBufferWidth;
    float m_backBufferHeight;
    float m_zNear;
    float m_zFar;
    float m_fov = 45.0f;
    const Math::Vec3D mOriginalPos;
};
}  // namespace Helpers
}  // namespace NEngine
