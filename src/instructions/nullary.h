// nullary.h

#pragma once

#include "../definitions.h"
#include "../program_state.h"
#include "../instruction_container.h"

// lazy way to shorten repetitive class declarations
#define decl(A) \
    class A : public InstructionContainer { \
    public: \
        A(Condition condition) : InstructionContainer(condition) {} \
        ~A() {} \
    protected: \
        Status action(ProgramState& state); \
    }

namespace spherehorn {

namespace Instructions {
    decl(Break);

    decl(Increment);
    decl(Decrement);
    decl(Invert);

    decl(InputChar);
    decl(InputNum);
    decl(InputString);
    decl(OutputChar);
    decl(OutputNum);
    decl(OutputString);

    decl(MemoryUp);
    decl(MemoryDown);
    decl(MemoryPrev);
    decl(MemoryNext);
    decl(MemoryRestart);
    decl(MemoryRotate);

    decl(InsertBefore);
    decl(InsertAfter);
    decl(DeleteBefore);
    decl(DeleteAfter);
}

}

#undef decl
