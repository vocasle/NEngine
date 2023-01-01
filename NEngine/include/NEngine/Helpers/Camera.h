#pragma once

#include <stdint.h>

#include "NEngine/Input/Mouse.h"
#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace NEngine {
namespace Helpers {
#define CAMERA_SENSITIVITY 0.01f
#define MOUSE_SENSITIVITY 0.0001f

class Camera
{
public:
    Camera();
    Camera(const glm::vec3 &cameraPos);

    glm::vec3 GetPos() const;
    glm::vec3 GetAt() const;
    glm::vec3 GetUp() const;
    glm::vec3 GetRight() const;
    float GetZFar() const;
    float GetZNear() const;
    glm::mat4x4 GetViewMat() const;
    glm::mat4x4 GetProjMat() const;

    void SetViewDimensions(uint32_t width, uint32_t height);
    void SetZNear(const float zNear);
    void SetZFar(const float zFar);
    void LookAt(const glm::vec3 &pos,
                const glm::vec3 &target,
                const glm::vec3 &up);
    void SetFov(float fov);
    void SetPosition(const glm::vec3 &position);
    void SetPitch(float pitch);
    void SetYaw(float yaw);

    void Tick(double deltaMillis);
    void ProcessKeyboard(double deltaMillis);
    void ProcessMouse(double deltaMillis);

private:
    void UpdateVectors();
    void UpdateSpeed();
    void ResetCamera();
    void Arcball(double deltaMillis);
    void OnMouseDown(const glm::vec2 &pos, Input::Mouse::ButtonType btnType);
    void OnMouseUp(const glm::vec2 &pos, Input::Mouse::ButtonType btnType);
    void OnMouseMove(const glm::vec2 &pos);
    void SetupMouseListener();

    float m_Pitch;
    float m_Yaw;
    glm::vec3 m_Pos;
    glm::vec3 m_At;
    glm::vec3 m_Right;
    glm::vec3 m_Up;
    float m_Speed;
    float m_backBufferWidth;
    float m_backBufferHeight;
    float m_zNear;
    float m_zFar;
    float m_fov = 45.0f;
    const glm::vec3 mOriginalPos;
    // Arcball
    Input::Mouse::MouseEventListener mMouseListener;
    glm::vec2 mPrevMousePos;
    glm::vec2 mCurMousePos;
    glm::mat4x4 mViewMat = glm::mat4x4(1);
};
}  // namespace Helpers
}  // namespace NEngine
