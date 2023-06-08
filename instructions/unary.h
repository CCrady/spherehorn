// unary.h

#pragma once

#include <utility>
#include "../definitions.h"
#include "../program_state.h"
#include "../arguments.h"
#include "instruction.h"

// lazy way to shorten repetitive class declarations
#define decl(A) \
    class A : public UnaryInstruction { \
    public: A(bool _isConditional, arg_ptr& _arg) : UnaryInstruction(_isConditional, _arg) {} \
    public: A(bool _isConditional, arg_ptr&& _arg) : UnaryInstruction(_isConditional, _arg) {} \
    protected: Status action(ProgramState& state); \
    }

namespace spherehorn {

namespace Instructions {
    class UnaryInstruction : public Instruction {
    protected:
        arg_ptr arg;
    public:
        UnaryInstruction(bool _isConditional, arg_ptr& _arg) :
            Instruction(_isConditional),
            arg(std::move(_arg)) {}
    };

    decl(SetAccumulator);
    decl(SetConditional);
    decl(SetMemoryVal);

    decl(Add);
    decl(Subtract);
    decl(ReverseSubtract);
    decl(Multiply);
    decl(Divide);
    decl(ReverseDivide);
    decl(Modulo);
    decl(ReverseModulo);

    decl(And);
    decl(Or);
    decl(Xor);

    decl(Greater);
    decl(Equal);
    decl(Less);
    decl(GreaterOrEqual);
    decl(LessOrEqual);
    decl(NotEqual);

    decl(MemoryBack);
    decl(MemoryForward);
}

}

#undef decl
