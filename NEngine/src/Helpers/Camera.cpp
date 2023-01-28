#include "NEngine/Helpers/Camera.h"

#include <DirectXMath.h>
#include <corecrt_math_defines.h>

#include <algorithm>
#include <cmath>

#include "NEngine/Input/Keyboard.h"
#include "NEngine/Input/Mouse.h"
#include "NEngine/Math/MathUtils.h"
#include "NEngine/Math/NEMath.h"
#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Helpers {

using namespace Math;
using namespace Input;
using namespace DirectX;

Camera::Camera()
    : Camera({0.0f, 0.0f, 0.0f})
{
    SetupMouseListener();
}

void
Camera::SetupMouseListener()
{
    mMouseListener.MouseDownCallback =
        [this](const Vec2D &pos, Mouse::ButtonType btnType)
    { OnMouseDown(pos, btnType); };
    mMouseListener.MouseUpCallback =
        [this](const Vec2D &pos, Mouse::ButtonType btnType)
    { OnMouseUp(pos, btnType); };
    mMouseListener.MouseMoveCallback = [this](const Vec2D &pos)
    { OnMouseMove(pos); };
    mMouseListener.MouseScrollCallback = [this](float mouseOffset)
    {
        auto fov = ToDegrees(m_fov);
        fov = fov - 5 * mouseOffset;
        fov = Clamp(30.0f, 120.0f, fov);
        m_fov = ToRadians(fov);
    };
    Mouse::Get().SetMouseEventListener(mMouseListener);
}

Camera::Camera(const Vec3D &cameraPos)
    : m_Pitch(0),
      m_Yaw(ToRadians(0)),
      m_Pos(cameraPos),
      m_Speed(1),
      m_backBufferWidth(0),
      m_backBufferHeight(0),
      m_zNear(1),
      m_zFar(100),
      m_fov(ToRadians(45)),
      mOriginalPos(cameraPos)
{
    UpdateVectors();
    SetupMouseListener();
}

Camera::Camera(const Camera &camera)
    : m_Pitch(camera.m_Pitch),
      m_Yaw(camera.m_Yaw),
      m_Pos(camera.m_Pos),
      m_At(camera.m_At),
      m_Right(camera.m_Right),
      m_Up(camera.m_Up),
      m_Speed(camera.m_Speed),
      m_backBufferWidth(camera.m_backBufferWidth),
      m_backBufferHeight(camera.m_backBufferHeight),
      m_zNear(camera.m_zNear),
      m_zFar(camera.m_zFar),
      m_fov(camera.m_fov),
      mOriginalPos(camera.mOriginalPos),
      mMouseListener(camera.mMouseListener),
      mPrevMousePos(camera.mPrevMousePos),
      mCurMousePos(camera.mCurMousePos),
      mViewMat(camera.mViewMat)
{
    SetupMouseListener();
}

Camera::~Camera()
{
    Mouse::Get().RemoveMouseEventListener(mMouseListener);
}

Mat4X4
Camera::GetViewMat() const
{
    return Math::LookAt(m_Pos + m_At, m_At, m_Up);
}

Mat4X4
Camera::GetProjMat() const
{
    return PerspectiveFov(
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

    m_Speed = Clamp(0.1f, 100.0f, m_Speed);
}

void
Camera::ProcessKeyboard(double deltaMillis)
{
    UpdateSpeed();
    // Vec3D cameraFocus = m_At;
    // const float delta = (float)deltaMillis * CAMERA_SENSITIVITY * m_Speed;
    // const float rotDelta = (float)deltaMillis * MOUSE_SENSITIVITY * 4.0f;

    // if (Keyboard::Get().IsKeyDown('A')) {
    //     Vec3D right = MathVec3DCross(&cameraFocus, &m_Up);
    //     MathVec3DNormalize(&right);
    //     right = MathVec3DModulateByScalar(&right, delta);
    //     m_Pos = MathVec3DSubtraction(&m_Pos, &right);
    // }
    // else if (Keyboard::Get().IsKeyDown('D')) {
    //     Vec3D right = MathVec3DCross(&cameraFocus, &m_Up);
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
    //     const Vec3D up = MathVec3DModulateByScalar(&m_Up, delta);
    //     m_Pos = MathVec3DAddition(&m_Pos, &up);
    // }
    // else if (Keyboard::Get().IsKeyDown('F')) {
    //     const Vec3D up = MathVec3DModulateByScalar(&m_Up, delta);
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
    const float x = d * cosf(m_Yaw);
    const float z = d * sinf(m_Yaw);
    const float y = sinf(m_Pitch);

    const Vec3D direction = vec3(x, y, z).Normalize();

    m_At = direction;
    const Vec3D worldUp = {0.0f, 1.0f, 0.0f};
    m_Right = Vec3D::Cross(worldUp, m_At).Normalize();
    m_Up = Vec3D::Cross(m_At, m_Right).Normalize();
}

void
Camera::ProcessMouse(double deltaMillis)
{
    // if (Mouse::Get().IsLeftButtonPressed()) {
    //     Arcball(deltaMillis);
    //     return;
    // }
    // return;

    const Vec2D mouseDelta = Mouse::Get().GetMouseDelta();

    static const float MAX_PITCH = (float)(M_PI_2 - 0.1);
    m_Yaw += mouseDelta.X * MOUSE_SENSITIVITY * (float)deltaMillis;
    m_Pitch += mouseDelta.Y * MOUSE_SENSITIVITY * (float)deltaMillis;
    m_Pitch = Clamp(-MAX_PITCH, MAX_PITCH, m_Pitch);

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
    m_backBufferWidth = static_cast<float>(width);
    m_backBufferHeight = static_cast<float>(height);
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
    // m_Up = m_Up;
}

void
Camera::SetFov(float fov)
{
    m_fov = ToRadians(fov);
}

void
Camera::SetPosition(const Math::Vec3D &position)
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
    // ProcessMouse(deltaMillis);
    // ProcessKeyboard(deltaMillis);
}

void
Camera::ResetCamera()
{
    m_Pitch = 0;
    m_Yaw = ToRadians(-90);
    m_Pos = mOriginalPos;
    m_Speed = 1;
    m_fov = ToRadians(45);
}

void
Camera::Arcball(double deltaMillis)
{
    // if (mPrevMousePos.X != mCurMousePos.X ||
    //     mPrevMousePos.Y != mCurMousePos.Y) {
    // const auto pitchDelta = mCurMousePos.Y - mPrevMousePos.Y;
    // const auto yawDelta = mCurMousePos.X - mPrevMousePos.X;

    // constexpr auto maxPitch = float(M_PI_2) - 0.1f;
    // m_Pitch += MathToRadians(pitchDelta * 0.4);
    // m_Pitch = std::clamp(m_Pitch, -maxPitch, maxPitch);
    // m_Yaw += MathToRadians(yawDelta * 0.4);

    float camRadius = 10.0f;
    // calculate camera position depending on pitch
    const auto h = camRadius * cos(m_Pitch);
    const auto z = -h * cos(m_Yaw);
    const auto x = -h * sin(m_Yaw);
    const auto y = camRadius * sin(m_Pitch);
    // XMFLOAT3 posCalculated(camRadius * h * cosf(m_Yaw), camRadius *
    // sinf(m_Pitch), camRadius * cosf(m_Pitch));
    m_Pos = Vec3D(x, y, z);
    // XMFLOAT3 posCalculated(x, y, z);

    // XMFLOAT3 at(0, 0, 0);
    // XMFLOAT3 up(0, 1, 0);

    // XMVECTOR vAt = XMLoadFloat3(&at);
    // XMVECTOR vPos = XMLoadFloat3(&posCalculated);

    // Utils::UtilsDebugPrint("cam distance: %f\n", length);

    // auto viewMat = XMMatrixLookAtRH(vPos, vAt, XMLoadFloat3(&up));
    // XMFLOAT4X4 tmp;
    // XMStoreFloat4x4(&tmp, viewMat);
    // mViewMat = Mat4X4(&tmp._11);

    // mPrevMousePos = mCurMousePos;
    //}
}

void
Camera::OnMouseDown(const Math::Vec2D &pos, Input::Mouse::ButtonType btnType)
{
    if (btnType == Mouse::ButtonType::Left) {
        mPrevMousePos = pos;
    }
    // Utils::UtilsDebugPrint("%s: pos: %s\n", __FUNCTION__,
    // pos.ToString().c_str());
}
void
Camera::OnMouseUp(const Math::Vec2D &pos, Input::Mouse::ButtonType btnType)
{
    // Utils::UtilsDebugPrint("%s: pos: %s\n", __FUNCTION__,
    // pos.ToString().c_str());
}
void
Camera::OnMouseMove(const Math::Vec2D &pos)
{
    mCurMousePos = pos;
    // Utils::UtilsDebugPrint("%s: pos: %s\n", __FUNCTION__,
    // pos.ToString().c_str());
}

Math::Vec3D
Camera::GetPos() const
{
    return m_Pos;
}

Math::Vec3D
Camera::GetAt() const
{
    return m_At;
}

Math::Vec3D
Camera::GetUp() const
{
    return m_Up;
}

Math::Vec3D
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
