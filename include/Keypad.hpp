#pragma once

namespace ch8
{
    enum class Key
    {
        Zero, One
    };

    class Keypad
    {
    public:
        Keypad() noexcept;

        bool IsKeyUp(Key key) const noexcept;
        bool IsKeyDown(Key key) const noexcept;

        bool SetKeyUp(Key key) noexcept;
        bool SetKeyDown(Key key) noexcept;

        bool AwaitKeypress(Key& key) noexcept;

    private:
        static constexpr int NumKeys = 16;

        bool keys[NumKeys];
    };
}
