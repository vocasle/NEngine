#include "NEngine/Input/Keyboard.h"

namespace NEngine
{
	namespace Input
	{
		std::unique_ptr<Keyboard> Keyboard::m_Instance = nullptr;

		Keyboard::Keyboard()
			: Keys(10)
			, States(10)
		{
			Keys[0] = static_cast<WPARAM>(Keys::W);
			Keys[1] = static_cast<WPARAM>(Keys::A);
			Keys[2] = static_cast<WPARAM>(Keys::S);
			Keys[3] = static_cast<WPARAM>(Keys::D);
			Keys[4] = static_cast<WPARAM>(Keys::Up);
			Keys[5] = static_cast<WPARAM>(Keys::Left);
			Keys[6] = static_cast<WPARAM>(Keys::Down);
			Keys[7] = static_cast<WPARAM>(Keys::Right);
			Keys[8] = static_cast<WPARAM>(Keys::Plus);
			Keys[9] = static_cast<WPARAM>(Keys::Minus);
		}

		Keyboard& Keyboard::Get()
		{
			if (!m_Instance) {
				m_Instance = std::unique_ptr<Keyboard>(new Keyboard());
			}
			return *m_Instance;
		}

		Keyboard::~Keyboard()
		{
		}

		void Keyboard::OnKeyDown(WPARAM wParam)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(Keys::Num); ++i) {
				if (Keys[i] == wParam) {
					States[i] = 1;
				}
			}
		}

		void Keyboard::OnKeyUp(WPARAM wParam)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(Keys::Num); ++i) {
				if (Keys[i] == wParam) {
					States[i] = 0;
				}
			}
		}

		uint32_t Keyboard::IsKeyDown(WPARAM key)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(Keys::Num); ++i) {
				if (Keys[i] == key) {
					return States[i];
				}
			}
			return 0;
		}

	}
}