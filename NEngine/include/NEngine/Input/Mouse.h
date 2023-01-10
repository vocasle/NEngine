#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <set>

#include "NEngine/Math/NEMath.h"
#include "Windows.h"

namespace NEngine {
namespace Input {
class Mouse
{
public:
    enum class ButtonType {
        Left,
        Scroll,
        Right
    };

    struct MouseEventListener
    {
        std::function<void(const Math::Vec2D &position)> MouseMoveCallback;
        std::function<void(const Math::Vec2D &position, ButtonType btnType)>
            MouseDownCallback;
        std::function<void(const Math::Vec2D &position, ButtonType btnType)>
            MouseUpCallback;
    };

    static Mouse &Get();
    ~Mouse();

    void SetWindowDimensions(uint32_t width, uint32_t height);
    Math::Vec2D GetCursorPos();
    void OnMouseMove(uint32_t message, WPARAM wParam, LPARAM lParam);
    void OnMouseDown(uint32_t message,
                     WPARAM wParam,
                     LPARAM lParam,
                     ButtonType type);
    void OnMouseUp(uint32_t message,
                   WPARAM wParam,
                   LPARAM lParam,
                   ButtonType type);

    void OnMouseMove(const Math::Vec2D &pos);
    void OnMouseDown(const Math::Vec2D &pos, ButtonType btnType);
    void OnMouseUp(const Math::Vec2D &pos, ButtonType btnType);

    Math::Vec2D GetMouseDelta();
    [[nodiscard]] bool IsLeftButtonPressed() const;
    [[nodiscard]] bool IsRightButtonPressed() const;

    void SetMouseEventListener(const MouseEventListener &listener);
    void RemoveMouseEventListener(const MouseEventListener &listener);

private:
    Mouse();
    void StoreMousePosition(LPARAM lParam);

    static std::unique_ptr<Mouse> mInstance;
    bool mIsLeftButtonPressed;
    bool mIsRightButtonPressed;
    Math::Vec2D mMousePos;
    Math::Vec2D mWinSize;

    std::set<const MouseEventListener *> mMouseListeners;
};

}  // namespace Input
}  // namespace NEngine