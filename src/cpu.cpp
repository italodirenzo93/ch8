#include "cpu.hpp"

#include <sstream>
#include <fstream>

#include "log.hpp"
#include "Exception.hpp"

static const std::array<uint8_t, 80> fontData{
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
        :
        opcodeTable(),
        memory(),
        v(),
        pc(memory.begin() + ProgramOffset),
        index(memory.begin()),
        delayTimer(0),
        delayTimerMs(0.0f),
        soundTimer(0),
        soundTimerMs(0.0f),
        keypad(),
        running(false) {}

    Cpu::Cpu(const std::map<opcode_t, opcode_handler_t>& opcodes) noexcept
        :
        opcodeTable(opcodes),
        memory(),
        v(),
        pc(memory.begin() + ProgramOffset),
        index(memory.begin()),
        delayTimer(0),
        delayTimerMs(0.0f),
        soundTimer(0),
        soundTimerMs(0.0f),
        keypad(),
        running(false) {}

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

        int pixel = *(display.first + offset) & (0x80 >> offset);

        return pixel != 0;
    }

    Cpu::opcode_t Cpu::GetNextOpcode() const noexcept
    {
        // If the loaded program has exited, return 0
        if (pc == program.second) {
            return 0;
        }

        // Grab the next opcode from the next 2 bytes
        const uint8_t msb = *pc;
        const uint8_t lsb = *(pc + 1);

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
        memory.fill(0);
        v.fill(0);

        font = std::make_pair(memory.begin(), memory.begin() + ProgramOffset);
        program = std::make_pair(memory.begin() + ProgramOffset, memory.begin() + StackOffset);
        stack = std::make_pair(memory.begin() + StackOffset, memory.begin() + DisplayOffset);
        display = std::make_pair(memory.begin() + DisplayOffset, memory.end());

        // Load font data into memory
        auto itA = fontData.begin();
        auto itB = font.first;
        for (; itA != fontData.end() && itB != font.second; itA++, itB++) {
            *itB = *itA;
        }

        pc = program.first;
        index = memory.begin();
        stackPointer = stack.first;

        delayTimer = 0;
        delayTimerMs = 0.0f;

        soundTimer = 0;
        soundTimerMs = 0.0f;

        keypad.fill(false);
        running = false;
    }

    void Cpu::Start()
    {
        if (pc == program.second) {
            throw Exception("Cannot start because program has exited");
        }
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
        std::ifstream ifs(filename, std::ios::binary | std::ios::ate);

        const int size = static_cast<int>(ifs.tellg());
        ifs.seekg(0, std::ios::beg);

        if (size == -1) {
            std::ostringstream oss;
            oss << "ROM file " << filename << " not found" << std::endl;
            throw Exception(oss.str());
        }

        if (size > MaxProgramSize) {
            std::ostringstream oss;
            oss << "File size too large (" << size << " bytes)" << std::endl;
            throw Exception(oss.str());
        }

        // Iterators for program memory range
        auto programStartIter = memory.begin() + ProgramOffset;
        auto programEndIter = programStartIter + MaxProgramSize;

        // Start by initializing the program range in memory to 0
        std::fill(programStartIter, programEndIter, 0);

        // Load the ROM into memory
        auto iter = programStartIter;
        while (ifs.good()) {
            ifs >> *iter;
            iter++;
        }
    }

    void Cpu::SetPixel(int x, int y, bool on)
    {
        int index = y * DisplayWidth + x;
        int byteIndex = index / 8;
        int offset = index % 8;
        auto byte = display.first + offset;

        if (on) {
            *byte = *byte | (0x80 >> offset);
        }
        else {
            *byte = *byte & (~(0x80 >> offset));
        }
    }

    void Cpu::SetOpcodeHandler(const opcode_t& opcode, const opcode_handler_t& handler)
    {
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
        const opcode_handler_t& handler = opcodeTable.at(opcode);

        // Execute the handler for this clock cycle's opcode
        const int result = handler(*this, opcode);

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
