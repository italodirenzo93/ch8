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
        using byte_t = uint8_t;
        using opcode_t = uint16_t;
        using address_t = uint16_t;
        using timer_t = uint8_t;

        // Constructors
        Cpu() noexcept;
        ~Cpu() noexcept;

        Cpu(const Cpu&) = delete;
        Cpu& operator=(const Cpu&) = delete;

        // Accessors
        bool IsRunning() const noexcept;
        bool GetPixel(int x, int y) const noexcept;
        opcode_t GetNextOpcode() const noexcept;

        // Mutators
        void Reset() noexcept;
        void Start() noexcept;
        void Stop() noexcept;
        bool LoadRomFromFile(const char* filename) noexcept;
        bool SetPixel(int x, int y, bool on) noexcept;
        bool ClockCycle(float elapsed) noexcept;

        void PushAddress(address_t address) noexcept;
        void PopAddress() noexcept;
        address_t JumpTo(address_t address) noexcept;

        // Constants
        static constexpr int TotalMemory = 4096;
        static constexpr uint32_t MaxProgramSize = 3232;

        static constexpr uint16_t ProgramOffset = 0x200;
        static constexpr uint16_t StackOffset = 0xEA0;
        static constexpr uint16_t DisplayOffset = 0xF00;

        static constexpr int DisplayWidth = 64;
        static constexpr int DisplayHeight = 32;

        static constexpr float TimerFrequency = 16.666f;

    private:
        // Data
        byte_t memory[TotalMemory];
        byte_t* font;
        byte_t* program;
        address_t* stack;
        byte_t* display;

        byte_t v[16];  // data registers

        address_t pc;
        address_t index;
        byte_t stackPointer;

        timer_t delayTimer;
        float delayTimerMs;

        timer_t soundTimer;
        float soundTimerMs;

        bool keypad[16];
        bool running;
    };

}
