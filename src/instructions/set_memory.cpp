// set_memory.cpp

#include "../program_state.h"
#include "../memory_cell.h"
#include "set_memory.h"
using namespace spherehorn;

Status Instructions::SetMemory::action(ProgramState& state) {
    *state.memoryPtr = value_; // copy assignment
    return Status::OKAY;
}

