// program_state.cpp

#include "definitions.h"
#include "program_state.h"

// TODO: see if I can figure out a better way of doing this
#ifndef NDEBUG

#include <string>
#include <sstream>
#include <ostream>

std::string spherehorn::ProgramState::to_string() {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::ostream& operator<<(std::ostream& out, const spherehorn::ProgramState& state) {
    out << "ProgramState @ " << &state << " {\n"
           "    accRegister = " << state.accRegister << "\n"
           "    condRegister = " << state.condRegister << "\n"
           "    memoryPtr = " << state.memoryPtr << "\n"
           "}" << std::endl;
    return out;
}

#endif
