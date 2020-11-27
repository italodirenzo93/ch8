#pragma once

#include <cstdint>
#include <array>
#include <stack>
#include <string>

namespace ch8
{
    class sdl_exception : public std::exception
    {
    public:
        sdl_exception() noexcept;
        sdl_exception(const std::string& message) noexcept;
        virtual const char* what() const noexcept override;

    private:
        std::string message;
    };

    class keypad
    {
        friend class chip8;
    public:
        enum class keystate_t { up, down };
        enum class key_t { up = 0x08, down = 0x04, left = 0x06, right = 0x02 };

    public:
        keypad() = default;
        ~keypad();

        keypad(const keypad&) = delete;

        bool is_key_up(const key_t& key) const noexcept;
        bool is_key_down(const key_t& key) const noexcept;
        key_t await_keypress();

    private:
        void set_key_up(const key_t& key) noexcept;
        void set_key_down(const key_t& key) noexcept;

    private:
        std::array<keystate_t, 16> keys;
    };

    class chip8 final
    {
    public:
        enum class pixel_t { ON, OFF };

    public:
        chip8();
        ~chip8();

        void reset() noexcept;
        void load_rom(const std::string& file);
        void clock_cycle();

    public:
        static constexpr uint16_t program_start_offset = 0x200;
        static constexpr uint8_t step_size = 2;

        keypad kp;

    private:
        std::array<uint8_t, 4096> memory;
        uint16_t index_register;
        std::array<uint8_t, 16> registers;
        uint16_t program_counter;
        std::stack<uint16_t> stack;
        std::array<pixel_t, 2048> framebuffer;
        uint8_t delay_timer;
        uint8_t sound_timer;
        bool running;
    };
}
