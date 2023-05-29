// nullary.cpp

#include <iostream>
#include <string>
#include <climits>
#include "../program_state.h"
#include "../memory_cell.h"
#include "nullary.h"

using namespace spherehorn;
using std::string;
using Instructions::Result;
// lazy way to shorten repetitive function implementations
#define impl(A) Result Instructions::A::action(ProgramState& state)

impl(Increment) {
    state.accRegister++;
    return OKAY;
}

impl(Decrement) {
    if (state.accRegister == 0) return ABORT;
    state.accRegister--;
    return OKAY;
}

impl(Invert) {
    state.condRegister = !state.condRegister;
    return OKAY;
}


impl(InputChar) {
    unsigned char inChar = 0;
    std::cin >> inChar;
    state.memoryPtr->setVal(inChar);
    return OKAY;
}

impl(InputNum) {
    num inNum = 0;
    std::cin >> inNum;
    state.memoryPtr->setVal(inNum);
    return OKAY;
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
    return OKAY;
}

impl(OutputChar) {
    num outNum = state.memoryPtr->getVal();
    // the given character needs to be ASCII
    if (outNum > UCHAR_MAX) return ABORT;
    std::cout << (unsigned char) outNum;
    return OKAY;
}

impl(OutputNum) {
    num outNum = state.memoryPtr->getVal();
    std::cout << outNum;
    return OKAY;
}

impl(OutputString) {
    MemoryCell* currChild = state.memoryPtr->getChild();
    num stringLength = state.memoryPtr->getVal();
    for (num i = 0; i < stringLength; i++) {
        std::cout << (unsigned char) currChild->getVal();
        currChild = currChild->getNext();
    }
    return OKAY;
}

#undef impl
