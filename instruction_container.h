// instruction_container.h

/* Class hierarchy:

             InstructionContainer
              /                \
InstructionBlock              Instruction
                                  /|\
                                  ...

Various instruction types branch off of Instruction.
*/

#pragma once

#include "program_state.h"

namespace spherehorn {

// The value returned by a call to .run(), indicating whether to continue execution as normal, break
// out of the current loop, exit the program gracefully, or abort termination with an error message.
enum struct Status {
    OKAY,
    BREAK,
    EXIT,
    ABORT,
};

class InstructionContainer {
public:
    virtual Status run(ProgramState& state) = 0;
};

}

