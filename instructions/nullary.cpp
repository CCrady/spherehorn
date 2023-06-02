// nullary.cpp

#include <iostream>
#include <string>
#include <climits>
#include "../program_state.h"
#include "../memory_cell.h"
#include "nullary.h"

using namespace spherehorn;
using std::string;
// lazy way to shorten repetitive function implementations
#define impl(A) Status Instructions::A::action(ProgramState& state)

impl(Break) {
    return Status::BREAK;
}


impl(Increment) {
    state.accRegister++;
    return Status::OKAY;
}

impl(Decrement) {
    if (state.accRegister == 0) {
        std::cerr << "Error: Attempted decrement past zero" << std::endl;
        return Status::ABORT;
    }
    state.accRegister--;
    return Status::OKAY;
}

impl(Invert) {
    state.condRegister = !state.condRegister;
    return Status::OKAY;
}


impl(InputChar) {
    unsigned char inChar = 0;
    std::cin >> inChar;
    state.memoryPtr->setVal(inChar);
    return Status::OKAY;
}

impl(InputNum) {
    num inNum = 0;
    std::cin >> inNum;
    state.memoryPtr->setVal(inNum);
    return Status::OKAY;
}

impl(InputString) {
    string inString;
    std::getline(std::cin, inString);
    state.memoryPtr->setVal(inString.length());

    MemoryCell* currChild = state.memoryPtr->getChild();
    for (auto it = inString.begin(); it != inString.end(); ++it) {
        currChild->setVal(*it);
        currChild = currChild->getNext();
    }
    return Status::OKAY;
}

impl(OutputChar) {
    num outNum = state.memoryPtr->getVal();
    // the given character needs to be ASCII
    if (outNum > UCHAR_MAX) {
        std::cerr << "Error: Attempted chout of invalid character ( #" << outNum << " )" << std::endl;
        return Status::ABORT;
    }
    std::cout << (unsigned char) outNum;
    return Status::OKAY;
}

impl(OutputNum) {
    num outNum = state.memoryPtr->getVal();
    std::cout << outNum;
    return Status::OKAY;
}

impl(OutputString) {
    MemoryCell* currChild = state.memoryPtr->getChild();
    num stringLength = state.memoryPtr->getVal();
    for (num i = 0; i < stringLength; i++) {
        std::cout << (unsigned char) currChild->getVal();
        currChild = currChild->getNext();
    }
    return Status::OKAY;
}

#undef impl
