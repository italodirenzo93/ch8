#pragma once

#include <map>
#include <functional>
#include <array>

#include <cstdint>

namespace ch8
{
    // CHIP-8 virtual machine CPU
    class cpu
    {
    public:
        // Types
        using opcode_t = uint16_t;
        using opcode_handler = std::function<int(cpu*, opcode_t)>;

        // Constructors
        cpu() noexcept;
        cpu(const std::map<opcode_t, opcode_handler>& opcodes) noexcept;
        ~cpu() noexcept;

        cpu(const cpu&) = delete;
        cpu& operator=(const cpu&) = delete;

        // Accessors
        bool is_running() const noexcept;
        bool get_pixel(int x, int y) const;
        opcode_handler get_opcode_handler(const opcode_t& opcode) const noexcept;

        // Mutators
        void reset() noexcept;
        void load_rom(const char* filename);
        void set_pixel(int x, int y, bool on);
        void set_opcode_handler(const opcode_handler& handler);
        void clock_cycle(float elapsed);

        // Constants
        static constexpr uint32_t MaxProgramSize = 3232;
        static constexpr uint16_t ProgramOffset = 0x200;

    private:
        std::map<opcode_t, opcode_handler> opcode_table;
        std::array<uint8_t, 4096> memory;
        std::array<uint8_t, 16> v;  // data registers

        uint16_t program_counter;
        uint16_t index_register;

        uint8_t delay_timer;
        uint8_t sound_timer;

        std::array<bool, 16> keypad;
        bool running;
    };
}
