#include "cpu.hpp"

#include <cstdio>

#include "Log.hpp"
#include "Exception.hpp"

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

    Cpu::Cpu(const std::map<opcode_t, opcode_handler_t>& opcodes) noexcept
        : opcodeTable(opcodes)
    {
        Reset();
    }

    Cpu::~Cpu() noexcept {}

    // Accessors
    bool Cpu::IsRunning() const noexcept
    {
        return running;
    }

    bool Cpu::GetPixel(int x, int y) const
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
        const uint8_t msb = memory[pc];
        const uint8_t lsb = memory[pc + 1];

        return msb << 8 | lsb;
    }

    Cpu::opcode_handler_t Cpu::GetOpcodeHandler(const opcode_t& opcode) const noexcept
    {
        const auto iter = opcodeTable.find(opcode);
        if (iter == opcodeTable.end()) {
            return nullptr;
        }

        return iter->second;
    }

    // Mutators
    void Cpu::Reset() noexcept
    {
        // Initialize all memory to 0
        memset(this->memory, 0, TotalMemory);

        // Font data sits at beginning
        memcpy(this->memory, FontData, FontSize);

        // Stack sits at offset 0xEA0-0xEFF
        this->stack = (uint16_t*)(this->memory + StackOffset);

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

    void Cpu::LoadRomFromFile(const char* filename)
    {
        log::debug("Loading ROM file %s...", filename);

        // Open the ROM file for reading
        size_t len = 0;
        FILE* f = fopen(filename, "rb");
        if (f == nullptr) {
            throw Exception("Could not open file %s...");
        }

        // get file size
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        rewind(f);
        if (len > MaxProgramSize) {
            fclose(f);
            throw Exception("File size too large (%zu bytes)");
        }

        memset(this->memory + ProgramOffset, 0, MaxProgramSize);
        fread(this->memory + ProgramOffset, sizeof(uint8_t), len, f);

        fclose(f);
    }

    void Cpu::SetPixel(int x, int y, bool on)
    {
        if ((x < 0 || x > DisplayWidth) && (y < 0 || y > DisplayHeight)) {
            throw Exception("X/Y coordinates must be within the DisplayWidth and DisplayHeight");
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
    }

    void Cpu::SetOpcodeHandler(const opcode_t& opcode, const opcode_handler_t& handler)
    {
        // TODO: implement this
        if (true) {
            throw Exception("Invalid opcode");
        }
        opcodeTable[opcode] = handler;
    }

    bool Cpu::ClockCycle(float elapsed)
    {
        // Check if running
        if (!running) {
            throw Exception("CHIP-8 CPU is not running");
        }

        // Grab the next opcode
        const opcode_t opcode = GetNextOpcode();
        if (opcode == 0) {
            return false;
        }

        opcode_handler_t* handler = nullptr;
        try {
            handler = &opcodeTable.at(opcode);
        }
        catch (const std::out_of_range& e) {
            throw Exception("Unknown opcode");
        }

        // Execute the handler for this clock cycle's opcode
        const int result = (*handler)(this, opcode);

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

}
