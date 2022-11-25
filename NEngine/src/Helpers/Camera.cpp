#include "NEngine/Helpers/Camera.h"

#include <corecrt_math_defines.h>

#include <cmath>

#include "NEngine/Input/Keyboard.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/Math.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {

using namespace Math;
using namespace Input;

Camera::Camera()
    : Camera({0.0f, 0.0f, 0.0f})
{
}

Camera::Camera(const Vec3D &cameraPos)
    : m_Pitch(0),
      m_Yaw(MathToRadians(-90)),
      m_Pos(cameraPos),
      m_Speed(1),
      m_backBufferWidth(0),
      m_backBufferHeight(0),
      m_zNear(1),
      m_zFar(100),
      m_fov(MathToRadians(45))
{
    UpdateVectors();
}

Mat4X4
Camera::GetViewMat() const
{
    const Vec3D direction = MathVec3DAddition(&m_Pos, &m_At);
    const auto negDir = MathVec3DSubtraction(m_Pos, direction);
    return MathMat4X4ViewAt(&m_Pos, &negDir, &m_Up);
}

Mat4X4
Camera::GetProjMat() const
{
    return MathMat4X4PerspectiveFov(
        m_fov, m_backBufferWidth / m_backBufferHeight, m_zNear, m_zFar);
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

    m_Speed = MathClamp(0.1f, 100.0f, m_Speed);
}

void
Camera::ProcessKeyboard(double deltaMillis)
{
    UpdateSpeed();
    Vec3D cameraFocus = m_At;
    const float delta = (float)deltaMillis * CAMERA_SENSITIVITY * m_Speed;

    if (Keyboard::Get().IsKeyDown(VK_LEFT) || Keyboard::Get().IsKeyDown('A')) {
        Vec3D right = MathVec3DCross(&cameraFocus, &m_Up);
        MathVec3DNormalize(&right);
        right = MathVec3DModulateByScalar(&right, delta);
        m_Pos = MathVec3DSubtraction(&m_Pos, &right);
    }
    else if (Keyboard::Get().IsKeyDown(VK_RIGHT) ||
             Keyboard::Get().IsKeyDown('D')) {
        Vec3D right = MathVec3DCross(&cameraFocus, &m_Up);
        MathVec3DNormalize(&right);
        right = MathVec3DModulateByScalar(&right, delta);
        m_Pos = MathVec3DAddition(&m_Pos, &right);
    }
    else if (Keyboard::Get().IsKeyDown(VK_UP)) {
        const Vec3D up = MathVec3DModulateByScalar(&m_Up, delta);
        m_Pos = MathVec3DAddition(&m_Pos, &up);
    }
    else if (Keyboard::Get().IsKeyDown(VK_DOWN)) {
        const Vec3D up = MathVec3DModulateByScalar(&m_Up, delta);
        m_Pos = MathVec3DSubtraction(&m_Pos, &up);
    }
    else if (Keyboard::Get().IsKeyDown('W')) {
        cameraFocus = MathVec3DModulateByScalar(&cameraFocus, delta);
        m_Pos = MathVec3DAddition(&m_Pos, &cameraFocus);
    }
    else if (Keyboard::Get().IsKeyDown('S')) {
        cameraFocus = MathVec3DModulateByScalar(&cameraFocus, delta);
        m_Pos = MathVec3DSubtraction(&m_Pos, &cameraFocus);
    }
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

    Vec3D direction = {x, y, z};
    MathVec3DNormalize(&direction);

    m_At = direction;
    MathVec3DNormalize(&m_At);
    const Vec3D worldUp = {0.0f, 1.0f, 0.0f};
    m_Right = MathVec3DCross(&worldUp, &m_At);
    MathVec3DNormalize(&m_Right);
    m_Up = MathVec3DCross(&m_At, &m_Right);
    MathVec3DNormalize(&m_Up);
}

void
Camera::ProcessMouse(double deltaMillis)
{
    const Vec2D mouseDelta = Mouse::Get().GetMouseDelta();

    static const float MAX_PITCH = (float)(M_PI_2 - 0.1);
    m_Yaw += mouseDelta.X * MOUSE_SENSITIVITY * (float)deltaMillis;
    m_Pitch += mouseDelta.Y * MOUSE_SENSITIVITY * (float)deltaMillis;
    m_Pitch = MathClamp(-MAX_PITCH, MAX_PITCH, m_Pitch);

    // static float xLastPos = 0;
    // static float yLastPos = 0;

    // if (xLastPos == 0.0f || yLastPos == 0.0f)
    //{
    //	xLastPos = Mouse->MousePos.X;
    //	yLastPos = Mouse->MousePos.Y;
    //	return;
    // }

    // const float xOffset = (Mouse->MousePos.X - xLastPos) * MOUSE_SENSITIVITY
    // * (float)deltaMillis; const float yOffset = (yLastPos -
    // Mouse->MousePos.Y) * MOUSE_SENSITIVITY * (float)deltaMillis;

    // xLastPos = Mouse->MousePos.X;
    // yLastPos = Mouse->MousePos.Y;

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
Camera::LookAt(const Vec3D &pos, const Vec3D &target, const Vec3D &up)
{
    m_Pos = pos;
    m_At = target;
    m_Up = up;
}

void
Camera::SetFov(float fov)
{
    m_fov = MathToRadians(fov);
}

}  // namespace Helpers
}  // namespace NEngine
