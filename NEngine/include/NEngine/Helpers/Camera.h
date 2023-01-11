#pragma once

#include <stdint.h>

#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/NEMath.h"

namespace NEngine {
namespace Helpers {
#define CAMERA_SENSITIVITY 0.01f
#define MOUSE_SENSITIVITY 0.0001f

class Camera
{
public:
    Camera();
    Camera(const Math::Vec3D &cameraPos);
    ~Camera();

    Math::Vec3D GetPos() const;
    Math::Vec3D GetAt() const;
    Math::Vec3D GetUp() const;
    Math::Vec3D GetRight() const;
    float GetZFar() const;
    float GetZNear() const;
    Math::Mat4X4 GetViewMat() const;
    Math::Mat4X4 GetProjMat() const;

    void SetViewDimensions(uint32_t width, uint32_t height);
    void SetZNear(const float zNear);
    void SetZFar(const float zFar);
    void LookAt(const Math::Vec3D &pos,
                const Math::Vec3D &target,
                const Math::Vec3D &up);
    void SetFov(float fov);
    void SetPosition(const Math::Vec3D &position);
    void SetPitch(float pitch);
    void SetYaw(float yaw);

    void Tick(double deltaMillis);
    void ProcessKeyboard(double deltaMillis);
    void ProcessMouse(double deltaMillis);

//private:
    void UpdateVectors();
    void UpdateSpeed();
    void ResetCamera();
    void Arcball(double deltaMillis);
    void OnMouseDown(const Math::Vec2D &pos, Input::Mouse::ButtonType btnType);
    void OnMouseUp(const Math::Vec2D &pos, Input::Mouse::ButtonType btnType);
    void OnMouseMove(const Math::Vec2D &pos);
    void SetupMouseListener();

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
    // Arcball
    Input::Mouse::MouseEventListener mMouseListener;
    Math::Vec2D mPrevMousePos;
    Math::Vec2D mCurMousePos;
    Math::Mat4X4 mViewMat = Math::MathMat4X4Identity();
};
}  // namespace Helpers
}  // namespace NEngine
