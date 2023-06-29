// instruction.h

#include "program_state.h"
#include "instruction_block.h"
using namespace spherehorn;

Status InstructionBlock::action(ProgramState& state) {
    Status result = Status::OKAY;
    for (unsigned int i = 0; result == Status::OKAY; i = (i + 1) % instrs.size()) {
        result = instrs.at(i)->run(state);
    }
    return result == Status::BREAK ? Status::OKAY : result;
}

