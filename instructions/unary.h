// unary.h

#pragma once

#include <memory>
#include <utility>
#include "../definitions.h"
#include "../program_state.h"
#include "../arguments.h"
#include "instruction.h"

// lazy way to shorten repetitive class declarations
#define decl(A) \
    class A : public UnaryInstruction { \
    public: A(bool _isConditional, Arguments::Argument* _arg) : UnaryInstruction(_isConditional, _arg) {} \
    protected: Status action(ProgramState& state); \
    }

namespace spherehorn {

namespace Instructions {
    class UnaryInstruction : public Instruction {
    protected:
        std::unique_ptr<Arguments::Argument> arg;
    public:
        UnaryInstruction(bool _isConditional, Arguments::Argument* _arg) : // TODO: pass a unique_ptr instead of a raw pointer
            Instruction(_isConditional),
            arg(_arg) {}
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
}

}

#undef decl
