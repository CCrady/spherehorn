// instruction.h

/* Class hierarchy:

InstructionContainer
         |
    Instruction
     /|\     \
     ...    UnaryInstruction
                  /|\
                  ...

Individual isntruction classes branch off of Instruction (for nullary instructions) and
UnaryInstruction (for unary instructions).
*/

#pragma once

#include <iostream>
#include "../definitions.h"
#include "../program_state.h"
#include "../instruction_container.h"

namespace spherehorn {

namespace Instructions {
    class Instruction : public InstructionContainer {
    protected:
        bool isConditional = false;
        // .action() is overloaded by subclasses to implement their individual behaviors
        virtual Status action(ProgramState& state) = 0;
    public:
        Instruction(bool _isConditional) : isConditional(_isConditional) {}
        // Run the instruction's overloaded .action() method, or simply do nothing if the
        // instruction shouldn't execute because of a conditional. Overload from
        // InstructionContainer.
        Status run(ProgramState& state) {
            if (isConditional && !state.condRegister) return Status::OKAY;
            return action(state);
        }
    };
}

}

