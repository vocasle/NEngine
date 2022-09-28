#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <Windows.h>

namespace NEngine
{
	namespace Input
	{
		enum class Keys {
			W = 'W',
			A = 'A',
			S = 'S',
			D = 'D',
			Up = VK_UP,
			Left = VK_LEFT,
			Down = VK_DOWN,
			Right = VK_RIGHT,
			Plus = VK_OEM_PLUS,
			Minus = VK_OEM_MINUS,
			Num = 10
		};

		class Keyboard {
		public:
			static Keyboard& Get();
			~Keyboard();

			void OnKeyDown(WPARAM wParam);
			void OnKeyUp(WPARAM wParam);

			uint32_t IsKeyDown(WPARAM key);

		private:
			Keyboard();

			static std::unique_ptr<Keyboard> m_Instance;

			std::vector<WPARAM> Keys;
			std::vector<uint32_t> States;
		};
	}
}