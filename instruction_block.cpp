// instruction.h

#include "program_state.h"
#include "instruction_block.h"
using namespace spherehorn;

Status InstructionBlock::run(ProgramState& state) {
    Status result = Status::OKAY;
    for (int i = 0; result == Status::OKAY; i = (i + 1) % instrs.size()) {
        result = instrs.at(i)->run(state);
    }
    if (result == Status::ABORT) {
        return Status::ABORT;
    } else {
        return Status::OKAY;
    }
}
