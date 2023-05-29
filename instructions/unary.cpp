// unary.cpp

#include "../definitions.h"
#include "../program_state.h"
#include "../arguments.h"
#include "unary.h"

using namespace spherehorn;
// lazy way to shorten repetitive function implementations
#define impl(A) void Instructions::A::action(ProgramState& state)

impl(SetAccumulator) {
    state.accRegister = arg->get();
}

impl(SetConditional) {
    state.condRegister = arg->get();
}

impl(SetMemoryVal) {
    state.memoryPtr->setVal(arg->get());
}


impl(Add) {
    state.accRegister += arg->get();
}

impl(Subtract) {
    state.accRegister -= arg->get();
}

impl(ReverseSubtract) {
    state.accRegister = arg->get() - state.accRegister;
}

impl(Multiply) {
    state.accRegister *= arg->get();
}

impl(Divide) {
    state.accRegister /= arg->get();
}

impl(ReverseDivide) {
    state.accRegister = arg->get() / state.accRegister;
}

impl(Modulo) {
    state.accRegister %= arg->get();
}

impl(ReverseModulo) {
    state.accRegister = arg->get() % state.accRegister;
}


impl(And) {
    state.condRegister = state.condRegister && arg->get();
}

impl(Or) {
    state.condRegister = state.condRegister || arg->get();
}

impl(Xor) {
    state.condRegister = state.condRegister != !!arg->get();
}

#undef impl
