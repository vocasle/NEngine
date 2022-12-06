#include "NEngine/Input/Keyboard.h"

#include <cassert>

#include "NEngine/Utils/Utils.h"

namespace NEngine {
namespace Input {
using namespace NEngine::Utils;
std::unique_ptr<Keyboard> Keyboard::m_Instance = nullptr;

Keyboard::Keyboard()
    : Keys(),
      States()
{
    Keys = {'W',
            'A',
            'S',
            'D',
            VK_UP,
            VK_LEFT,
            VK_DOWN,
            VK_RIGHT,
            VK_OEM_PLUS,
            VK_OEM_MINUS,
            'R',
            'F'};

    States.resize(Keys.size());
}

Keyboard &
Keyboard::Get()
{
    if (!m_Instance) {
        m_Instance = std::unique_ptr<Keyboard>(new Keyboard());
    }
    return *m_Instance;
}

Keyboard::~Keyboard()
{
}

void
Keyboard::OnKeyDown(WPARAM wParam)
{
    for (size_t i = 0; i < Keys.size(); ++i) {
        if (Keys[i] == wParam) {
            States[i] = 1;
        }
    }
}

void
Keyboard::OnKeyUp(WPARAM wParam)
{
    for (size_t i = 0; i < Keys.size(); ++i) {
        if (Keys[i] == wParam) {
            States[i] = 0;
        }
    }
}

uint32_t
Keyboard::IsKeyDown(WPARAM key)
{
    for (size_t i = 0; i < Keys.size(); ++i) {
        if (Keys[i] == key) {
            return States[i];
        }
    }
    return 0;
}

void
Keyboard::RegisterKey(WPARAM key)
{
    if (std::find(std::begin(Keys), std::end(Keys), key) != std::end(Keys)) {
        UtilsDebugPrint("WARN: Key '%d' is already registered.");
        return;
    }
    Keys.push_back(key);
    States.push_back(0);
    assert(Keys.size() == States.size() &&
           "Size of Keys does not match size of States");
}

}  // namespace Input
}  // namespace NEngine