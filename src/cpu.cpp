#include "cpu.hpp"

#include <fstream>

#include "log.hpp"

namespace ch8
{
    // Constructors
    cpu::cpu() noexcept
        :
        opcode_table(),
        memory(),
        v(),
        program_counter(ProgramOffset),
        index_register(0),
        delay_timer(0),
        sound_timer(0),
        keypad(),
        running(false) {}

    cpu::cpu(const std::map<opcode_t, opcode_handler>& opcodes) noexcept
        :
        opcode_table(opcodes),
        memory(),
        v(),
        program_counter(ProgramOffset),
        index_register(0),
        delay_timer(0),
        sound_timer(0),
        keypad(),
        running(false) {}

    cpu::~cpu() noexcept {}

    // Accessors
    bool cpu::is_running() const noexcept
    {
        return running;
    }

    bool cpu::get_pixel(int x, int y) const
    {
        return false;
    }

    cpu::opcode_handler cpu::get_opcode_handler(const opcode_t& opcode) const noexcept
    {
        auto iter = opcode_table.find(opcode);
        if (iter == opcode_table.end()) {
            return nullptr;
        }
        
        return iter->second;
    }

    // Mutators
    void cpu::reset() noexcept
    {
        memory.fill(0);
        v.fill(0);

        program_counter = 0;
        index_register = 0;

        delay_timer = 0;
        sound_timer = 0;

        keypad.fill(false);
        running = false;
    }

    void cpu::load_rom(const char* filename)
    {
        log::debug("Loading ROM file %s...", filename);

        // Open the ROM file for reading
        std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
        int size = static_cast<int>(ifs.tellg());
        ifs.seekg(0, std::ios::beg);

        if (size > MaxProgramSize) {
            std::string msg = "File size too large (";
            msg += size;
            msg += ")";
            throw std::exception(msg.c_str());
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

    void cpu::set_pixel(int x, int y, bool on)
    {
    }

    void cpu::set_opcode_handler(const opcode_handler& handler)
    {
    }

    void cpu::clock_cycle(float elapsed)
    {
    }
}
