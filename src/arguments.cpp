// arguments.cpp

#include "definitions.h"
#include "arguments.h"
using namespace spherehorn;


ProgramState* Arguments::Argument::state = nullptr; // TODO: is this the right way to initialize a static variable?

num Arguments::Constant::get() {
    return value;
}

num Arguments::Accumulator::get() {
    return state->accRegister;
}

num Arguments::MemoryCell::get() {
    return state->memoryPtr->getVal();
}

