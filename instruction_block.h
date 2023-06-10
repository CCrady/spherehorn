// instruction_block.h

/* Class hierarchy:

InstructionContainer
         |
  InstructionBlock

See instruction_container.h for a more complete view of the tree.
*/

#pragma once

#include <vector>
#include <memory>
#include <utility>
#include "program_state.h"
#include "instruction_container.h"

namespace spherehorn {

class InstructionBlock : public InstructionContainer {
private:
    std::vector<instr_ptr> instrs;
public:
    InstructionBlock(Condition condition = Condition::ALWAYS) : InstructionContainer(condition) {}
    void insertInstr(instr_ptr& instr) {
        instrs.push_back(std::move(instr));
    }
    // execute each instruction in instrs in a loop until we break out
    Status action(ProgramState& state);
};

}

