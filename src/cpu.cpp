#include "Cpu.hpp"

#include <cstdio>

#include "Log.hpp"

// Font data
static constexpr int FontSize = 80;
static const uint8_t FontData[] = {
    // 4x5 font sprites (0-F)
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0xA0, 0xA0, 0xF0, 0x20, 0x20,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80,
};

namespace ch8
{
    // Constructors
    Cpu::Cpu() noexcept
    {
        Reset();
    }

    Cpu::~Cpu() noexcept
    {
    }

    // Accessors
    bool Cpu::IsRunning() const noexcept
    {
        return running;
    }

    bool Cpu::GetPixel(int x, int y) const noexcept
    {
        int index = y * DisplayWidth + x;
        int byteIndex = index / 8;
        int offset = index % 8;

        int pixel = display[byteIndex] & (0x80 >> offset);

        return pixel != 0;
    }

    Cpu::opcode_t Cpu::GetNextOpcode() const noexcept
    {
        // If the loaded program has exited, return 0
        if (memory[pc] == 0) {
            return 0;
        }

        // Grab the next opcode from the next 2 bytes
        const byte_t msb = memory[pc];
        const byte_t lsb = memory[pc + 1];

        return msb << 8 | lsb;
    }

    // Mutators
    void Cpu::Reset() noexcept
    {
        // Initialize all memory to 0
        memset(this->memory, 0, TotalMemory);

        // Font data sits at beginning
        memcpy(this->memory, FontData, FontSize);
        this->font = this->memory;
        this->program = this->memory + ProgramOffset;

        // Stack sits at offset 0xEA0-0xEFF
        this->stack = (address_t*)(this->memory + StackOffset);

        // Display refresh sits at 0xF00-0xFFF
        this->display = (this->memory + DisplayOffset);

        this->index = 0;
        this->pc = ProgramOffset;
        this->stackPointer = 0;
        this->running = false;

        this->delayTimer = 0;
        this->delayTimerMs = 0.0f;

        this->soundTimer = 0;
        this->soundTimerMs = 0.0f;

        for (int i = 0; i < 16; i++) {
            keypad[i] = false;
        }
        running = false;
    }

    void Cpu::Start() noexcept
    {
        running = true;
    }

    void Cpu::Stop() noexcept
    {
        running = false;
    }

    bool Cpu::LoadRomFromFile(const char* filename) noexcept
    {
        log::debug("Loading ROM file %s...", filename);

        // Open the ROM file for reading
        size_t len = 0;
        FILE* f = fopen(filename, "rb");
        if (f == nullptr) {
            return false;
        }

        // get file size
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        rewind(f);
        if (len > MaxProgramSize) {
            fclose(f);
            //throw Exception("File size too large (%zu bytes)");
            return false;
        }

        memset(this->program, 0, MaxProgramSize);
        fread(this->program, sizeof(byte_t), len, f);

        fclose(f);

        return true;
    }

    bool Cpu::SetPixel(int x, int y, bool on) noexcept
    {
        if ((x < 0 || x > DisplayWidth) && (y < 0 || y > DisplayHeight)) {
            //throw Exception("X/Y coordinates must be within the DisplayWidth and DisplayHeight");
            return false;
        }

        const int index = y * DisplayWidth + x;
        const int byteIndex = index / 8;
        const int offset = index % 8;
        uint8_t byte = display[byteIndex];

        if (on) {
            byte = byte | (0x80 >> offset);
        }
        else {
            byte = byte & (~(0x80 >> offset));
        }

        display[byteIndex] = byte;

        return true;
    }

    bool Cpu::ClockCycle(float elapsed) noexcept
    {
        // Check if running
        if (!running) {
            return false;
        }

        // Grab the next opcode
        const opcode_t opcode = GetNextOpcode();
        if (opcode == 0) {
            return false;
        }

        // Set the PC to the next instruction
        pc += 2;

        // Update the timers
        if (delayTimer != 0) {
            delayTimerMs += elapsed;
            if (delayTimerMs >= TimerFrequency) {
                delayTimer--;
                delayTimerMs = 0.0f;
            }
        }

        if (soundTimer != 0) {
            soundTimerMs += elapsed;
            if (soundTimerMs >= TimerFrequency) {
                soundTimer--;
                soundTimerMs = 0.0f;
            }
        }

        return true;
    }

    void Cpu::PushAddress(address_t address) noexcept
    {
        stack[++stackPointer] = address;
    }

    void Cpu::PopAddress() noexcept
    {
        stack[stackPointer] = 0;
        this->pc = stack[--stackPointer];
    }

    Cpu::address_t Cpu::JumpTo(address_t address) noexcept
    {
        if (address > MaxProgramSize) {
            //throw std::out_of_range("Address out of program memory range");
            return 0;
        }

        this->pc = address;

        return pc;
    }

}
