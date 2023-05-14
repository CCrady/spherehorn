// program_state.h

#pragma once

#include "definitions.h"
#include "memory_cell.h"

#ifndef NDEBUG
#include <string>
#include <ostream>
#endif

namespace spherehorn {

struct ProgramState {
    num accRegister;
    bool condRegister;
    MemoryCell* memoryPtr;

// TODO: see if I can figure out a better way of doing this
#ifndef NDEBUG
    std::string to_string();
#endif
};

}

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& out, const spherehorn::ProgramState& state);
#endif

