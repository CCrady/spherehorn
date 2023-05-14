// arguments.cpp

#include "definitions.h"
#include "arguments.h"
using namespace spherehorn;

num Arguments::Constant::get() {
    return value;
}

num Arguments::Accumulator::get() {
    return state->accRegister;
}

/* // TODO: implement this
num Arguments::MemoryCell::get() {
    return state->
}
*/
