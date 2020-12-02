#pragma once

#include <string>
#include <map>
#include <functional>
#include <array>

#include <cstdint>

namespace ch8
{

    // CHIP-8 virtual machine CPU
    class Cpu
    {
    public:
        // Types
        using opcode_t = uint16_t;
        using opcode_handler_t = std::function<int(Cpu&, opcode_t)>;

        // Constructors
        Cpu() noexcept;
        Cpu(const std::map<opcode_t, opcode_handler_t>& opcodes) noexcept;
        ~Cpu() noexcept;

        Cpu(const Cpu&) = delete;
        Cpu& operator=(const Cpu&) = delete;

        // Accessors
        bool IsRunning() const noexcept;
        bool GetPixel(int x, int y) const;
        opcode_handler_t GetOpcodeHandler(const opcode_t& opcode) const noexcept;

        // Mutators
        void Reset() noexcept;
        void LoadRomFromFile(const char* filename);
        void SetPixel(int x, int y, bool on);
        void SetOpcodeHandler(const opcode_t& opcode, const opcode_handler_t& handler);
        void ClockCycle(float elapsed);

        // Constants
        static constexpr uint32_t MaxProgramSize = 3232;
        static constexpr uint16_t ProgramOffset = 0x200;
        static constexpr uint16_t StackOffset = 0;
        static constexpr uint16_t DisplayOffset = 0;
        static constexpr int DisplayWidth = 64;
        static constexpr int DisplayHeight = 32;

    private:
        // Types
        using memory_t = std::array<uint8_t, 4096>;
        using memory_range_t = std::pair<memory_t::iterator, memory_t::iterator>;

        // Data
        std::map<opcode_t, opcode_handler_t> opcodeTable;

        memory_t memory;
        memory_range_t font;
        memory_range_t program;
        memory_range_t stack;
        memory_range_t display;

        std::array<uint8_t, 16> v;  // data registers

        memory_t::iterator pc;
        memory_t::iterator index;
        memory_t::iterator stackPointer;

        uint8_t delayTimer;
        uint8_t soundTimer;

        std::array<bool, 16> keypad;
        bool running;
    };

}
