// nullary.h

#pragma once

#include "../definitions.h"
#include "instruction.h"
#include "../program_state.h"

// lazy way to shorten repetitive class declarations
#define decl(A) \
    class A : public Nullary { \
        public: A(bool _isConditional) : Nullary(_isConditional) {} \
        protected: Result action(ProgramState& state); \
    }

namespace spherehorn {

namespace Instructions {
    class Nullary : public ActiveInstruction {
    public:
        Nullary(bool _isConditional) : ActiveInstruction(_isConditional) {}
    };

    decl(Increment);
    decl(Decrement);
    decl(Invert);

    decl(InputChar);
    decl(InputNum);
    decl(InputString);
    decl(OutputChar);
    decl(OutputNum);
    decl(OutputString);
}

}

#undef decl