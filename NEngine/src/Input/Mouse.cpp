#include "NEngine/Input/Mouse.h"
#include "NEngine/Utils/Utils.h"

#include <string.h>
#include <windowsx.h>

namespace NEngine
{
	namespace Input
	{
		using namespace Math;

		std::unique_ptr<Mouse> Mouse::m_Instance = nullptr;

		Mouse& Mouse::Get()
		{
			if (!m_Instance) {
				m_Instance = std::unique_ptr<Mouse>(new Mouse());
			}
			return *m_Instance;
		}

		Mouse::Mouse()
			: m_LeftBtnState{ false }
			, m_RightBtnState{ false }
		{
		}

		Mouse::~Mouse()
		{
		}

		void Mouse::SetWindowDimensions(uint32_t width, uint32_t height)
		{
			WinSize.X = width;
			WinSize.Y = height;
		}

		Vec2D Mouse::GetCursorPos()
		{
			return MousePos;
		}

		// TODO: On first mouse move after right button click MousePos should be
		// fetched from last cursor position Try to save last mouse position when right
		// button is released
		void Mouse::OnMouseMove(uint32_t message, WPARAM wParam, LPARAM lParam)
		{
			if (m_RightBtnState) {
				MousePos.X = GET_X_LPARAM(lParam);
				MousePos.Y = GET_Y_LPARAM(lParam);
				POINT pos = {};
				::GetCursorPos(&pos);
				MousePos.X = pos.x;
				MousePos.Y = pos.y;
				SetCursorPos((int32_t)WinSize.X / 2, (int32_t)WinSize.Y / 2);
			}
		}

		void Mouse::OnMouseDown(uint32_t message, WPARAM wParam, LPARAM lParam,
			ButtonType type)
		{
			switch (type) {
			case Mouse::ButtonType::Left:
				m_LeftBtnState = true;
				break;
			case Mouse::ButtonType::Scroll:
				break;
			case Mouse::ButtonType::Right:
				m_RightBtnState = true;
				ShowCursor(false);
				break;
			default:
				break;
			}
		}

		void Mouse::OnMouseUp(uint32_t message, WPARAM wParam, LPARAM lParam,
			ButtonType type)
		{
			switch (type) {
			case Mouse::ButtonType::Left:
				m_LeftBtnState = false;
				break;
			case Mouse::ButtonType::Scroll:
				break;
			case Mouse::ButtonType::Right:
				m_RightBtnState = false;
				ShowCursor(true);
				break;
			default:
				break;
			}
		}

		Vec2D Mouse::GetMouseDelta()
		{
			Vec2D delta = {};
			if (m_RightBtnState && (MousePos.X > 0.0f || MousePos.Y > 0.0f)) {
				delta.X = WinSize.X / 2.0f - MousePos.X;
				delta.Y = WinSize.Y / 2.0f - MousePos.Y;
			}
			return delta;
		}

	}
}