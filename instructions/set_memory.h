// set_memory.h

#pragma once

#include "../program_state.h"
#include "../memory_cell.h"
#include "../instruction_container.h"

namespace spherehorn {

namespace Instructions {
    class SetMemory : public InstructionContainer {
    private:
        MemoryCell value_;
    public:
        SetMemory(Condition condition, MemoryCell& value) :
            InstructionContainer(condition),
            value_(value) {}
        SetMemory(Condition condition, MemoryCell&& value) :
            InstructionContainer(condition),
            value_(value) {}
        ~SetMemory() {}
    protected:
        Status action(ProgramState& state);
    };
}

}

