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
#define impl(A) void Instructions::A::action(ProgramState& state)

impl(Increment) {
    state.accRegister++;
}

impl(Decrement) {
    state.accRegister--;
}

impl(Invert) {
    state.condRegister = !state.condRegister;
}


impl(InputChar) {
    unsigned char inChar = 0;
    std::cin >> inChar;
    state.memoryPtr->setVal(inChar);
}

impl(InputNum) {
    num inNum = 0;
    std::cin >> inNum;
    state.memoryPtr->setVal(inNum);
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
}

impl(OutputChar) {
    num outNum = state.memoryPtr->getVal();
    if (outNum > UCHAR_MAX) return; // TODO: this should result in an abort
    std::cout << (unsigned char) outNum;
}

impl(OutputNum) {
    num outNum = state.memoryPtr->getVal();
    std::cout << outNum;
}

impl(OutputString) {
    MemoryCell* currChild = state.memoryPtr->getChild();
    num stringLength = state.memoryPtr->getVal();
    for (num i = 0; i < stringLength; i++) {
        std::cout << (unsigned char) currChild->getVal();
        currChild = currChild->getNext();
    }
}

#undef impl
