#pragma once

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <vector>


namespace NEngine {
namespace Input {
class Keyboard
{
public:
    static Keyboard &Get();
    ~Keyboard();

    void OnKeyDown(WPARAM wParam);
    void OnKeyUp(WPARAM wParam);
    uint32_t IsKeyDown(WPARAM key);
	void RegisterKey(WPARAM key);

private:
    Keyboard();

    static std::unique_ptr<Keyboard> m_Instance;

    std::vector<WPARAM> Keys;
    std::vector<uint32_t> States;
};
}  // namespace Input
}  // namespace NEngine