// instruction_container.h

/* Class hierarchy:

             InstructionContainer
              /       /|\      \
InstructionBlock      ...     UnaryInstruction
                                     /|\
                                     ...

Individual isntruction classes branch off of InstructionContainer (for nullary instructions and
SetMemory) and UnaryInstruction (for unary instructions).
*/

#pragma once

#include <memory>
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

enum struct Condition {
    ALWAYS,
    WHEN_TRUE,
    WHEN_FALSE,
};

class InstructionContainer {
protected:
    Condition condition_;
    // .action() is overloaded by subclasses to implement their individual behaviors
    virtual Status action(ProgramState& state) = 0;
public:
    InstructionContainer(Condition condition) : condition_(condition) {}
    virtual ~InstructionContainer() {}
    // Run the overloaded .action() method, or simply do nothing if we shouldn't execute because of
    // a conditional.
    Status run(ProgramState& state) {
        if (condition_ == Condition::WHEN_TRUE && !state.condRegister) return Status::OKAY;
        if (condition_ == Condition::WHEN_FALSE && state.condRegister) return Status::OKAY;
        return action(state);
    }
};

using instr_ptr = std::unique_ptr<InstructionContainer>;

}

