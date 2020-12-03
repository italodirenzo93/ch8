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

        // Constructors
        Cpu() noexcept;
        ~Cpu() noexcept;

        Cpu(const Cpu&) = delete;
        Cpu& operator=(const Cpu&) = delete;

        // Accessors
        bool IsRunning() const noexcept;
        bool GetPixel(int x, int y) const;
        opcode_t GetNextOpcode() const noexcept;

        // Mutators
        void Reset() noexcept;
        void Start() noexcept;
        void Stop() noexcept;
        void LoadRomFromFile(const char* filename);
        void SetPixel(int x, int y, bool on);
        bool ClockCycle(float elapsed);

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
        uint8_t memory[TotalMemory];
        uint8_t* font;
        uint8_t* program;
        uint16_t* stack;
        uint8_t* display;

        uint8_t v[16];  // data registers

        uint16_t pc;
        uint16_t index;
        uint16_t stackPointer;

        uint8_t delayTimer;
        float delayTimerMs;

        uint8_t soundTimer;
        float soundTimerMs;

        bool keypad[16];
        bool running;
    };

}
