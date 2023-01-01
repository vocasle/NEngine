#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <set>

#include "Windows.h"
#include "glm/vec2.hpp"

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
        std::function<void(const glm::vec2 &position)> MouseMoveCallback;
        std::function<void(const glm::vec2 &position, ButtonType btnType)>
            MouseDownCallback;
        std::function<void(const glm::vec2 &position, ButtonType btnType)>
            MouseUpCallback;
    };

    static Mouse &Get();
    ~Mouse();

    void SetWindowDimensions(uint32_t width, uint32_t height);
    glm::vec2 GetCursorPos();
    void OnMouseMove(uint32_t message, WPARAM wParam, LPARAM lParam);
    void OnMouseDown(uint32_t message,
                     WPARAM wParam,
                     LPARAM lParam,
                     ButtonType type);
    void OnMouseUp(uint32_t message,
                   WPARAM wParam,
                   LPARAM lParam,
                   ButtonType type);

    void OnMouseMove(const glm::vec2 &pos);
    void OnMouseDown(const glm::vec2 &pos, ButtonType btnType);
    void OnMouseUp(const glm::vec2 &pos, ButtonType btnType);

    glm::vec2 GetMouseDelta();
    [[nodiscard]] bool IsLeftButtonPressed() const;
    [[nodiscard]] bool IsRightButtonPressed() const;

    void SetMouseEventListener(const MouseEventListener &listener);

private:
    Mouse();
    void StoreMousePosition(LPARAM lParam);

    static std::unique_ptr<Mouse> mInstance;
    bool mIsLeftButtonPressed;
    bool mIsRightButtonPressed;
    glm::vec2 mMousePos;
    glm::vec2 mWinSize;

    std::set<const MouseEventListener *> mMouseListeners;
};

}  // namespace Input
}  // namespace NEngine