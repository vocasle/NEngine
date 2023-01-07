#include "NEngine/Input/Mouse.h"

#include <string.h>
#include <windowsx.h>

#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Input {
using namespace Math;

std::unique_ptr<Mouse> Mouse::mInstance = nullptr;

Mouse &
Mouse::Get()
{
    if (!mInstance) {
        mInstance = std::unique_ptr<Mouse>(new Mouse());
    }
    return *mInstance;
}

Mouse::Mouse()
    : mIsLeftButtonPressed{false},
      mIsRightButtonPressed{false}
{
}

Mouse::~Mouse()
{
}

void
Mouse::SetWindowDimensions(uint32_t width, uint32_t height)
{
    mWinSize.X = width;
    mWinSize.Y = height;
}

Vec2D
Mouse::GetCursorPos()
{
    return mMousePos;
}

// TODO: On first mouse move after right button click mMousePos should be
// fetched from last cursor position Try to save last mouse position when right
// button is released
void
Mouse::OnMouseMove(uint32_t message, WPARAM wParam, LPARAM lParam)
{
    StoreMousePosition(lParam);
    // if (mIsRightButtonPressed) {
    //     mMousePos.X = GET_X_LPARAM(lParam);
    //     mMousePos.Y = GET_Y_LPARAM(lParam);
    //     POINT pos = {};
    //     ::GetCursorPos(&pos);
    //     mMousePos.X = pos.x;
    //     mMousePos.Y = pos.y;
    //     SetCursorPos((int32_t)mWinSize.X / 2, (int32_t)mWinSize.Y / 2);
    // }

    OnMouseMove(mMousePos);
}

void
Mouse::OnMouseDown(uint32_t message,
                   WPARAM wParam,
                   LPARAM lParam,
                   ButtonType type)
{
    StoreMousePosition(lParam);
    OnMouseDown(mMousePos, type);
}

void
Mouse::OnMouseUp(uint32_t message,
                 WPARAM wParam,
                 LPARAM lParam,
                 ButtonType type)
{
    StoreMousePosition(lParam);
    OnMouseUp(mMousePos, type);
}

void
Mouse::OnMouseMove(const Math::Vec2D &pos)
{
    for (auto &cb : mMouseListeners) {
        if (cb->MouseMoveCallback) {
            cb->MouseMoveCallback(pos);
        }
    }
}
void
Mouse::OnMouseDown(const Math::Vec2D &pos, ButtonType btnType)
{
    switch (btnType) {
        case Mouse::ButtonType::Left:
            mIsLeftButtonPressed = true;
            break;
        case Mouse::ButtonType::Scroll:
            break;
        case Mouse::ButtonType::Right:
            mIsRightButtonPressed = true;
            break;
        default:
            break;
    }

    for (auto &cb : mMouseListeners) {
        if (cb->MouseDownCallback) {
            cb->MouseDownCallback(pos, btnType);
        }
    }
}
void
Mouse::OnMouseUp(const Math::Vec2D &pos, ButtonType btnType)
{
    switch (btnType) {
        case Mouse::ButtonType::Left:
            mIsLeftButtonPressed = false;
            break;
        case Mouse::ButtonType::Scroll:
            break;
        case Mouse::ButtonType::Right:
            mIsRightButtonPressed = false;
            break;
        default:
            break;
    }

    for (auto &cb : mMouseListeners) {
        if (cb->MouseUpCallback) {
            cb->MouseUpCallback(pos, btnType);
        }
    }
}

void
Mouse::StoreMousePosition(LPARAM lParam)
{
    mMousePos.X = GET_X_LPARAM(lParam);
    mMousePos.Y = GET_Y_LPARAM(lParam);
}

Vec2D
Mouse::GetMouseDelta()
{
    // Vec2D delta = {};
    // if (mIsRightButtonPressed && (mMousePos.X > 0.0f || mMousePos.Y > 0.0f))
    // {
    //     delta.X = mWinSize.X / 2.0f - mMousePos.X;
    //     delta.Y = mWinSize.Y / 2.0f - mMousePos.Y;
    // }
    // return delta;
    return mMousePos;
}

bool
Mouse::IsLeftButtonPressed() const
{
    return mIsLeftButtonPressed;
}
bool
Mouse::IsRightButtonPressed() const
{
    return mIsRightButtonPressed;
}

void
Mouse::SetMouseEventListener(const MouseEventListener &listener)
{
    if (!mMouseListeners.contains(&listener)) {
        mMouseListeners.insert(&listener);
    }
}

void
Mouse::RemoveMouseEventListener(const MouseEventListener &listener)
{
    auto it = std::find(
        std::begin(mMouseListeners), std::end(mMouseListeners), &listener);
    if (it != std::end(mMouseListeners)) {
        mMouseListeners.erase(it);
    }
}

}  // namespace Input
}  // namespace NEngine