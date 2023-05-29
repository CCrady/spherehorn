// unary.cpp

#include "../definitions.h"
#include "../program_state.h"
#include "../arguments.h"
#include "unary.h"

using namespace spherehorn;
using Instructions::Result;
// lazy way to shorten repetitive function implementations
#define impl(A) Result Instructions::A::action(ProgramState& state)

impl(SetAccumulator) {
    state.accRegister = arg->get();
    return OKAY;
}

impl(SetConditional) {
    state.condRegister = arg->get();
    return OKAY;
}

impl(SetMemoryVal) {
    state.memoryPtr->setVal(arg->get());
    return OKAY;
}


impl(Add) {
    state.accRegister += arg->get();
    return OKAY;
}

impl(Subtract) {
    // abort if we would underflow
    if (arg->get() > state.accRegister) return ABORT;
    state.accRegister -= arg->get();
    return OKAY;
}

impl(ReverseSubtract) {
    // abort if we would underflow
    if (state.accRegister > arg->get()) return ABORT;
    state.accRegister = arg->get() - state.accRegister;
    return OKAY;
}

impl(Multiply) {
    state.accRegister *= arg->get();
    return OKAY;
}

impl(Divide) {
    // abort if we would divide by zero
    if (arg->get() == 0) return ABORT;
    state.accRegister /= arg->get();
    return OKAY;
}

impl(ReverseDivide) {
    // abort if we would divide by zero
    if (state.accRegister == 0) return ABORT;
    state.accRegister = arg->get() / state.accRegister;
    return OKAY;
}

impl(Modulo) {
    // abort if we would mod by zero
    if (arg->get() == 0) return ABORT;
    state.accRegister %= arg->get();
    return OKAY;
}

impl(ReverseModulo) {
    // abort if we would mod by zero
    if (state.accRegister == 0) return ABORT;
    state.accRegister = arg->get() % state.accRegister;
    return OKAY;
}


impl(And) {
    state.condRegister = state.condRegister && arg->get();
    return OKAY;
}

impl(Or) {
    state.condRegister = state.condRegister || arg->get();
    return OKAY;
}

impl(Xor) {
    state.condRegister = state.condRegister != !!arg->get();
    return OKAY;
}

#undef impl
