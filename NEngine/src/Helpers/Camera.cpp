#include "NEngine/Helpers/Camera.h"

#include <corecrt_math_defines.h>

#include <algorithm>
#include <cmath>

#include "NEngine/Input/Keyboard.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Utils/Utils.h"
#include "glm/ext.hpp"

namespace NEngine {
namespace Helpers {

using namespace Input;

Camera::Camera()
    : Camera({0.0f, 0.0f, 0.0f})
{
    SetupMouseListener();
}

void
Camera::SetupMouseListener()
{
    mMouseListener.MouseDownCallback =
        [this](const glm::vec2 &pos, Mouse::ButtonType btnType)
    { OnMouseDown(pos, btnType); };
    mMouseListener.MouseUpCallback =
        [this](const glm::vec2 &pos, Mouse::ButtonType btnType)
    { OnMouseUp(pos, btnType); };
    mMouseListener.MouseMoveCallback = [this](const glm::vec2 &pos)
    { OnMouseMove(pos); };
    Mouse::Get().SetMouseEventListener(mMouseListener);
}

Camera::Camera(const glm::vec3 &cameraPos)
    : m_Pitch(0),
      m_Yaw(glm::radians(-90.0f)),
      m_Pos(cameraPos),
      m_Speed(1),
      m_backBufferWidth(0),
      m_backBufferHeight(0),
      m_zNear(1),
      m_zFar(100),
      m_fov(glm::radians(45.0f)),
      mOriginalPos(cameraPos)
{
    UpdateVectors();
    SetupMouseListener();
}

glm::mat4x4
Camera::GetViewMat() const
{
    auto at = glm::vec3(0, 0, 0);
    const auto negDir = m_Pos - (at - m_Pos);
    return glm::lookAt(m_Pos, negDir, m_Up);
}

glm::mat4x4
Camera::GetProjMat() const
{
    return glm::perspectiveFov(
        m_fov, m_backBufferWidth, m_backBufferHeight, m_zNear, m_zFar);
}

void
Camera::UpdateSpeed()
{
    if (Keyboard::Get().IsKeyDown(VK_OEM_PLUS)) {
        m_Speed += 0.5f;
    }
    else if (Keyboard::Get().IsKeyDown(VK_OEM_MINUS)) {
        m_Speed -= 0.5f;
    }

    m_Speed = glm::clamp(0.1f, 100.0f, m_Speed);
}

void
Camera::ProcessKeyboard(double deltaMillis)
{
    UpdateSpeed();
    // glm::vec3 cameraFocus = m_At;
    // const float delta = (float)deltaMillis * CAMERA_SENSITIVITY * m_Speed;
    // const float rotDelta = (float)deltaMillis * MOUSE_SENSITIVITY * 4.0f;

    // if (Keyboard::Get().IsKeyDown('A')) {
    //     glm::vec3 right = MathVec3DCross(&cameraFocus, &m_Up);
    //     MathVec3DNormalize(&right);
    //     right = MathVec3DModulateByScalar(&right, delta);
    //     m_Pos = MathVec3DSubtraction(&m_Pos, &right);
    // }
    // else if (Keyboard::Get().IsKeyDown('D')) {
    //     glm::vec3 right = MathVec3DCross(&cameraFocus, &m_Up);
    //     MathVec3DNormalize(&right);
    //     right = MathVec3DModulateByScalar(&right, delta);
    //     m_Pos = MathVec3DAddition(&m_Pos, &right);
    // }
    // else if (Keyboard::Get().IsKeyDown('W')) {
    //     cameraFocus = MathVec3DModulateByScalar(&cameraFocus, delta);
    //     m_Pos = MathVec3DAddition(&m_Pos, &cameraFocus);
    // }
    // else if (Keyboard::Get().IsKeyDown('S')) {
    //     cameraFocus = MathVec3DModulateByScalar(&cameraFocus, delta);
    //     m_Pos = MathVec3DSubtraction(&m_Pos, &cameraFocus);
    // }
    // else if (Keyboard::Get().IsKeyDown('R')) {
    //     const glm::vec3 up = MathVec3DModulateByScalar(&m_Up, delta);
    //     m_Pos = MathVec3DAddition(&m_Pos, &up);
    // }
    // else if (Keyboard::Get().IsKeyDown('F')) {
    //     const glm::vec3 up = MathVec3DModulateByScalar(&m_Up, delta);
    //     m_Pos = MathVec3DSubtraction(&m_Pos, &up);
    // }

    // else if (Keyboard::Get().IsKeyDown(VK_LEFT)) {
    //     m_Yaw += rotDelta;
    // }
    // else if (Keyboard::Get().IsKeyDown(VK_RIGHT)) {
    //     m_Yaw -= rotDelta;
    // }
    // else if (Keyboard::Get().IsKeyDown(VK_UP)) {
    //     m_Pitch += rotDelta;
    // }
    // else if (Keyboard::Get().IsKeyDown(VK_DOWN)) {
    //     m_Pitch -= rotDelta;
    // }
    // else if (Keyboard::Get().IsKeyDown('X')) {
    //     ResetCamera();
    // }
}

void
Camera::UpdateVectors()
{
    // World axis positions:
    // Right:       +X
    // Up:          +Y
    // Forward:     +Z
    //
    // Camera axis positions relative to World:
    // Right:       -X
    // Up:          +Y
    // Forward:     -Z
    const float d = cosf(m_Pitch);
    const float x = -d * cosf(m_Yaw);
    const float z = d * sinf(m_Yaw);
    const float y = sinf(m_Pitch);

    m_At = glm::normalize(glm::vec3(x, y, z));
    const glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};
    m_Right = glm::normalize(glm::cross(worldUp, m_At));
    m_Up = glm::normalize(glm::cross(m_At, m_Right));
}

void
Camera::ProcessMouse(double deltaMillis)
{
    if (Mouse::Get().IsLeftButtonPressed()) {
        Arcball(deltaMillis);
        return;
    }
    return;

    const glm::vec2 mouseDelta = Mouse::Get().GetMouseDelta();

    static const float MAX_PITCH = (float)(M_PI_2 - 0.1);
    m_Yaw += mouseDelta.x * MOUSE_SENSITIVITY * (float)deltaMillis;
    m_Pitch += mouseDelta.y * MOUSE_SENSITIVITY * (float)deltaMillis;
    m_Pitch = glm::clamp(-MAX_PITCH, MAX_PITCH, m_Pitch);

    // static float xLastPos = 0;
    // static float yLastPos = 0;

    // if (xLastPos == 0.0f || yLastPos == 0.0f)
    //{
    //	xLastPos = Mouse->MousePos.x;
    //	yLastPos = Mouse->MousePos.y;
    //	return;
    // }

    // const float xOffset = (Mouse->MousePos.x - xLastPos) * MOUSE_SENSITIVITY
    // * (float)deltaMillis; const float yOffset = (yLastPos -
    // Mouse->MousePos.y) * MOUSE_SENSITIVITY * (float)deltaMillis;

    // xLastPos = Mouse->MousePos.x;
    // yLastPos = Mouse->MousePos.y;

    // static const float MAX_PITCH = (float)(M_PI_2 - 0.1);

    // Yaw -= xOffset;
    // Pitch += yOffset; // reverse y because in screen space y goes from top to
    // bottom of the screen Pitch = MathClamp(-MAX_PITCH, MAX_PITCH, Pitch);

    UpdateVectors();
}

void
Camera::SetViewDimensions(uint32_t width, uint32_t height)
{
    m_backBufferWidth = width;
    m_backBufferHeight = height;
}

void
Camera::SetZNear(const float zNear)
{
    m_zNear = zNear;
}

void
Camera::SetZFar(const float zFar)
{
    m_zFar = zFar;
}

void
Camera::LookAt(const glm::vec3 &pos,
               const glm::vec3 &target,
               const glm::vec3 &up)
{
    m_Pos = pos;
    m_At = target;
    m_Up = up;
}

void
Camera::SetFov(float fov)
{
    m_fov = glm::radians(fov);
}

void
Camera::SetPosition(const glm::vec3 &position)
{
    m_Pos = position;
}
void
Camera::SetPitch(float pitch)
{
    m_Pitch = pitch;
}
void
Camera::SetYaw(float yaw)
{
    m_Yaw = yaw;
}

void
Camera::Tick(double deltaMillis)
{
    ProcessMouse(deltaMillis);
    ProcessKeyboard(deltaMillis);
}

void
Camera::ResetCamera()
{
    m_Pitch = 0;
    m_Yaw = glm::radians(-90.0f);
    m_Pos = mOriginalPos;
    m_Speed = 1;
    m_fov = glm::radians(45.0f);
}

void
Camera::Arcball(double deltaMillis)
{
    if (mPrevMousePos.x != mCurMousePos.x ||
        mPrevMousePos.y != mCurMousePos.y) {
        const auto pitchDelta = mCurMousePos.y - mPrevMousePos.y;
        const auto yawDelta = mCurMousePos.x - mPrevMousePos.x;

        constexpr auto maxPitch = float(M_PI_2) - 0.1f;
        m_Pitch += glm::radians(pitchDelta * 0.4);
        m_Pitch = std::clamp(m_Pitch, -maxPitch, maxPitch);
        m_Yaw += glm::radians(yawDelta * 0.4);

        Utils::UtilsDebugPrint("pitch: %f, yaw: %f\n", m_Pitch, m_Yaw);
        float camRadius = m_Pos.length();
        // calculate camera position depending on pitch
        const auto h = camRadius * cos(m_Pitch);
        const auto x = -h * cos(m_Yaw);
        const auto z = -h * sin(m_Yaw);
        const auto y = camRadius * sin(m_Pitch);
        // XMFLOAT3 posCalculated(camRadius * h * cosf(m_Yaw), camRadius *
        // sinf(m_Pitch), camRadius * cosf(m_Pitch));
        m_Pos = glm::vec3(x, y, z);
        // XMFLOAT3 posCalculated(x, y, z);

        // XMFLOAT3 at(0, 0, 0);
        // XMFLOAT3 up(0, 1, 0);

        // XMVECTOR vAt = XMLoadFloat3(&at);
        // XMVECTOR vPos = XMLoadFloat3(&posCalculated);

        // Utils::UtilsDebugPrint("cam distance: %f\n", length);

        // auto viewMat = XMMatrixLookAtRH(vPos, vAt, XMLoadFloat3(&up));
        // XMFLOAT4X4 tmp;
        // XMStoreFloat4x4(&tmp, viewMat);
        // mViewMat = glm::mat4x4(&tmp._11);

        mPrevMousePos = mCurMousePos;
    }
}

void
Camera::OnMouseDown(const glm::vec2 &pos, Input::Mouse::ButtonType btnType)
{
    if (btnType == Mouse::ButtonType::Left) {
        mPrevMousePos = pos;
    }
    // Utils::UtilsDebugPrint("%s: pos: %s\n", __FUNCTION__,
    // pos.ToString().c_str());
}
void
Camera::OnMouseUp(const glm::vec2 &pos, Input::Mouse::ButtonType btnType)
{
    // Utils::UtilsDebugPrint("%s: pos: %s\n", __FUNCTION__,
    // pos.ToString().c_str());
}
void
Camera::OnMouseMove(const glm::vec2 &pos)
{
    mCurMousePos = pos;
    // Utils::UtilsDebugPrint("%s: pos: %s\n", __FUNCTION__,
    // pos.ToString().c_str());
}

glm::vec3
Camera::GetPos() const
{
    return m_Pos;
}

glm::vec3
Camera::GetAt() const
{
    return m_At;
}

glm::vec3
Camera::GetUp() const
{
    return m_Up;
}

glm::vec3
Camera::GetRight() const
{
    return m_Right;
}

float
Camera::GetZFar() const
{
    return m_zFar;
}

float
Camera::GetZNear() const
{
    return m_zNear;
}

}  // namespace Helpers
}  // namespace NEngine
