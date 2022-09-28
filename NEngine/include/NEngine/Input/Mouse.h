#pragma once
#include "NEngine/Math/Math.h"

#include <cstdint>
#include <memory>

#include "Windows.h"

namespace NEngine
{
	namespace Input
	{
		class Mouse {
		public:
			enum class ButtonType { Left, Scroll, Right };

		public:
			static Mouse& Get();
			~Mouse();

			void SetWindowDimensions(uint32_t width, uint32_t height);
			Math::Vec2D GetCursorPos();
			void OnMouseMove(uint32_t message, WPARAM wParam, LPARAM lParam);
			void OnMouseDown(uint32_t message, WPARAM wParam, LPARAM lParam,
				ButtonType type);
			void OnMouseUp(uint32_t message, WPARAM wParam, LPARAM lParam,
				ButtonType type);
			Math::Vec2D GetMouseDelta();

		private:
			Mouse();

			static std::unique_ptr<Mouse> m_Instance;

			bool m_LeftBtnState;
			bool m_RightBtnState;
			Math::Vec2D MousePos;
			Math::Vec2D WinSize;
		};

	}
}