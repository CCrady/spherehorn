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
#include "instruction_container.h"
#include "program_state.h"

namespace spherehorn {

class InstructionBlock : public InstructionContainer {
private:
    std::vector<std::unique_ptr<InstructionContainer>> instrs;
public:
    void insertInstr(std::unique_ptr<InstructionContainer>& instr) {
        instrs.push_back(std::move(instr));
    }
    // execute each instruction in instrs in a loop until we break out
    Status run(ProgramState& state);
};

}

