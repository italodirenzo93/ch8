#include <bandit/bandit.h>

#include "Cpu.hpp"

using namespace bandit;
using namespace snowhouse;

go_bandit([]() {
    describe("CPU", []() {
        it("begins in initialized state", [&]() {
            ch8::Cpu cpu;
            ch8::CpuState_t state = cpu.GetState();

            AssertThat(state.memory, Is().Not().Null());
            /*for (int i = 0; i < ch8::Cpu::TotalMemory; i++) {
                AssertThat(state.memory[i], Equals(0));
            }*/

            AssertThat(state.programCounter, Equals(ch8::Cpu::ProgramOffset));
            AssertThat(state.indexRegister, Equals(0));
            AssertThat(state.stackPointer, Equals(0));
            AssertThat(state.delayTimer, Equals(0));
            AssertThat(state.soundTimer, Equals(0));
        });
    });
});

int main(int argc, char* argv[])
{
    return bandit::run(argc, argv);
}
