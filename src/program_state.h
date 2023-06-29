// program_state.h

#pragma once

#include "definitions.h"
#include "memory_cell.h"

namespace spherehorn {

struct ProgramState {
    num accRegister = 0;
    bool condRegister = false;
    MemoryCell* memoryPtr = nullptr;
};

}

