#include "Keypad.hpp"

#include <SDL.h>

namespace ch8
{
    Keypad::Keypad() noexcept
        : keys()
    {
        for (int i = 0; i < NumKeys; i++) {
            keys[i] = false;
        }
    }

    bool Keypad::IsKeyUp(Key key) const noexcept
    {
        int keyIdx = static_cast<int>(key);
        return !keys[keyIdx];
    }

    bool Keypad::IsKeyDown(Key key) const noexcept
    {
        const int keyIdx = static_cast<int>(key);
        return keys[keyIdx];
    }

    bool Keypad::SetKeyUp(Key key) noexcept
    {
        int keyIdx = static_cast<int>(key);
        keys[keyIdx] = false;
        return true;
    }

    bool Keypad::SetKeyDown(Key key) noexcept
    {
        int keyIdx = static_cast<int>(key);
        keys[keyIdx] = true;
        return true;
    }

    bool Keypad::AwaitKeypress(Key& key) noexcept
    {
        SDL_Event e = { 0 };
        while (e.type != SDL_KEYDOWN) {
            SDL_WaitEvent(&e);
        }

        key = Key::One;
        SetKeyDown(key);

        return true;
    }
}
