// unary.h

#pragma once

#include <utility>
#include "../definitions.h"
#include "../program_state.h"
#include "../arguments.h"
#include "../instruction_container.h"

// lazy way to shorten repetitive class declarations
#define decl(A) \
    class A : public UnaryInstruction { \
    public: \
        A(Condition condition, arg_ptr& _arg) : UnaryInstruction(condition, _arg) {} \
        A(Condition condition, arg_ptr&& _arg) : UnaryInstruction(condition, _arg) {} \
        ~A() {} \
    protected: \
        Status action(ProgramState& state); \
    }

namespace spherehorn {

namespace Instructions {
    class UnaryInstruction : public InstructionContainer {
    protected:
        arg_ptr arg;
    public:
        UnaryInstruction(Condition condition, arg_ptr& _arg) :
            InstructionContainer(condition),
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
