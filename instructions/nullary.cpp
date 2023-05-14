// nullary.cpp

#include "../program_state.h"
#include "nullary.h"

using namespace spherehorn;
// lazy way to shorten repetitive function implementations
#define impl(A) void Instructions::A::action(ProgramState& state)

impl(Increment) {
    state.accRegister++;
}

impl(Decrement) {
    state.accRegister--;
}

#undef impl
