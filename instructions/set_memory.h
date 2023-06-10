// set_memory.h

#pragma once

#include "../program_state.h"
#include "../memory_cell.h"
#include "instruction.h"

namespace spherehorn {

namespace Instructions {
    class SetMemory : public Instruction {
    private:
        MemoryCell value_;
    public:
        SetMemory(bool isConditional, MemoryCell& value) :
            Instruction(isConditional),
            value_(value) {}
        SetMemory(bool isConditional, MemoryCell&& value) :
            Instruction(isConditional),
            value_(value) {}
    protected:
        Status action(ProgramState& state);
    };
}

}

