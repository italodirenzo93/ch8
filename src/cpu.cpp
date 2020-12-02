#include "cpu.hpp"

#include <sstream>
#include <fstream>

#include "log.hpp"
#include "Exception.hpp"

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
        soundTimer(0),
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
        soundTimer(0),
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

    Cpu::opcode_handler_t Cpu::GetOpcodeHandler(const opcode_t& opcode) const noexcept
    {
        auto iter = opcodeTable.find(opcode);
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

        pc = program.first;
        index = memory.begin();
        stackPointer = stack.first;

        delayTimer = 0;
        soundTimer = 0;

        keypad.fill(false);
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

    void Cpu::ClockCycle(float elapsed)
    {
    }

}
