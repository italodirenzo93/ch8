#include "chip8.hpp"

#include <fstream>
#include <sstream>
#include <SDL.h>

namespace ch8
{
    sdl_exception::sdl_exception() noexcept
    {
    }

    sdl_exception::sdl_exception(const std::string& message)
        :
        message(message)
    {
    }

    const char* sdl_exception::what() const noexcept
    {
        std::ostringstream oss;
        if (!message.empty()) {
            oss << message << std::endl << '\t';
        }
        oss << "SDL error: " << SDL_GetError() << std::endl;
        return oss.str().c_str();
    }

    keypad::keypad()
        :
        keys() {}

    keypad::~keypad() {}

    bool keypad::is_key_up(const key_t& key) const noexcept
    {
        return keys[static_cast<int>(key)] == keystate_t::up;
    }

    bool keypad::is_key_down(const key_t& key) const noexcept
    {
        return keys[static_cast<int>(key)] == keystate_t::down;
    }

    keypad::key_t keypad::await_keypress()
    {
        SDL_Event e = { 0 };
        while (e.type != SDL_KEYDOWN) {
            if (SDL_WaitEvent(&e) == -1) {
                throw std::exception("Error waiting for event");
            }
        }

        return key_t::up;
    }

    void keypad::set_key_up(const key_t& key) noexcept
    {
        keys[static_cast<int>(key)] = keystate_t::up;
    }

    void keypad::set_key_down(const key_t& key) noexcept
    {
        keys[static_cast<int>(key)] = keystate_t::down;
    }

    chip8::chip8()
        :
        memory(),
        program_counter(program_start_offset),
        registers(),
        index_register(0),
        stack(),
        framebuffer(),
        delay_timer(0),
        sound_timer(0),
        running(true)
    {
    }

    chip8::~chip8()
    {
    }

    void chip8::reset() noexcept
    {
        memory.fill(0);
        registers.fill(0);
        framebuffer.fill(pixel_t::OFF);

        index_register = 0;
        program_counter = program_start_offset;
        running = true;

        delay_timer = 0;
        sound_timer = 0;

        //log_debug("CHIP-VM (re)-initialized");
    }

    void chip8::load_rom(const std::string& file)
    {
        std::ifstream ifs(file, std::ifstream::binary);
        int i = 0;
        while (!ifs.eof()) {
            ifs >> memory[i++];
        }
    }

    void chip8::clock_cycle()
    {
        if (!running) {
            throw std::exception("CHIP-8 VM is not running");
        }

        const uint8_t msb = memory[program_counter];
        const uint8_t lsb = memory[static_cast<size_t>(program_counter) + 1];
        const uint16_t opcode = msb << 8 | lsb;

        if (opcode == 0x0) {
            running = false;
            return;
        }

        // big switch...

        program_counter += step_size;
    }
}
