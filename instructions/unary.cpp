// unary.cpp

#include <iostream>
#include "../definitions.h"
#include "../program_state.h"
#include "../arguments.h"
#include "unary.h"

using namespace spherehorn;
// lazy way to shorten repetitive function implementations
#define impl(A) Status Instructions::A::action([[maybe_unused]] ProgramState& state)

impl(SetAccumulator) {
    state.accRegister = arg->get();
    return Status::OKAY;
}

impl(SetConditional) {
    state.condRegister = arg->get();
    return Status::OKAY;
}

impl(SetMemoryVal) {
    state.memoryPtr->setVal(arg->get());
    return Status::OKAY;
}


impl(Add) {
    state.accRegister += arg->get();
    return Status::OKAY;
}

impl(Subtract) {
    // abort if we would underflow
    if (arg->get() > state.accRegister) {
        std::cerr << "Error: Attempted to perform invalid SUB "
                     "( " << state.accRegister << " - " << arg->get() << " )" << std::endl;
        return Status::ABORT;
    }
    state.accRegister -= arg->get();
    return Status::OKAY;
}

impl(ReverseSubtract) {
    // abort if we would underflow
    if (state.accRegister > arg->get()) {
        std::cerr << "Error: Attempted to perform invalid RSUB "
                     "( " << arg->get() << " - " << state.accRegister << " )" << std::endl;
        return Status::ABORT;
    }
    state.accRegister = arg->get() - state.accRegister;
    return Status::OKAY;
}

impl(Multiply) {
    state.accRegister *= arg->get();
    return Status::OKAY;
}

impl(Divide) {
    // abort if we would divide by zero
    if (arg->get() == 0) {
        std::cerr << "Error: Attempted to perform DIV by zero "
                     "( " << state.accRegister << " / " << arg->get() << " )" << std::endl;
        return Status::ABORT;
    }
    state.accRegister /= arg->get();
    return Status::OKAY;
}

impl(ReverseDivide) {
    // abort if we would divide by zero
    if (state.accRegister == 0) {
        std::cerr << "Error: Attempted to perform RDIV by zero "
                     "( " << arg->get() << " / " << " )" << std::endl;
        return Status::ABORT;
    }
    state.accRegister = arg->get() / state.accRegister;
    return Status::OKAY;
}

impl(Modulo) {
    // abort if we would mod by zero
    if (arg->get() == 0) {
        std::cerr << "Error: Attempted to perform MOD by zero "
                     "( " << state.accRegister << " % " << arg->get() << " )" << std::endl;
        return Status::ABORT;
    }
    state.accRegister %= arg->get();
    return Status::OKAY;
}

impl(ReverseModulo) {
    // abort if we would mod by zero
    if (state.accRegister == 0) {
        std::cerr << "Error: Attempted to perform RMOD by zero "
                     "( " << arg->get() << " % " << state.accRegister << " )" << std::endl;
        return Status::ABORT;
    }
    state.accRegister = arg->get() % state.accRegister;
    return Status::OKAY;
}


impl(And) {
    state.condRegister = state.condRegister && arg->get();
    return Status::OKAY;
}

impl(Or) {
    state.condRegister = state.condRegister || arg->get();
    return Status::OKAY;
}

impl(Xor) {
    state.condRegister = state.condRegister != !!arg->get();
    return Status::OKAY;
}


impl(Greater) {
    state.condRegister = state.accRegister > arg->get();
    return Status::OKAY;
}

impl(Equal) {
    state.condRegister = state.accRegister == arg->get();
    return Status::OKAY;
}

impl(Less) {
    state.condRegister = state.accRegister < arg->get();
    return Status::OKAY;
}

impl(GreaterOrEqual) {
    state.condRegister = state.accRegister >= arg->get();
    return Status::OKAY;
}

impl(LessOrEqual) {
    state.condRegister = state.accRegister <= arg->get();
    return Status::OKAY;
}

impl(NotEqual) {
    state.condRegister = state.accRegister != arg->get();
    return Status::OKAY;
}


impl(MemoryBack) {
    state.memoryPtr = state.memoryPtr->shiftBack(arg->get());
    return Status::OKAY;
}

impl(MemoryForward) {
    state.memoryPtr = state.memoryPtr->shiftForward(arg->get());
    return Status::OKAY;
}

#undef impl
