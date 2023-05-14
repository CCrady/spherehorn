// unit_tests.cpp

// this is terrible but I need to test MemoryCell's internals
#define private public
#include "memory_cell.h"
#undef private

#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include "definitions.h"
#include "program_state.h"
#include "arguments.h"
#include "instructions/instructions.h"

#define assert(name, var, test) \
    if (var test) { \
        std::cout << "  PASSED " << setw(20) << name << ":  " #var " " #test << std::endl; \
    } else { \
        std::cout << "! FAILED " << setw(20) << name << ":  " #var " " #test "\n" \
                      #var " = " << var << std::endl; \
    }

#define assertAccEq(name, val) \
    assert(name, state.accRegister, == val) \
    if (state.condRegister != false || state.memoryPtr != nullptr) { \
        std::cout << "! FAILED state side effects: \n" \
                     "state.condRegister = " << state.condRegister << "\n" \
                     "state.memoryPtr = " << state.memoryPtr << endl; \
    }

using namespace spherehorn;
using namespace std;

// test instructions that rely only on the program state, not any memory manipulation
void testStateInstructions();
// test the MemoryCell class
void testMemoryCell();

int main() {
    testStateInstructions();
    testMemoryCell();
    return 0;
}

void resetState(ProgramState& state) {
    state.accRegister = 10;
    state.condRegister = false;
    state.memoryPtr = nullptr;
}

Arguments::Argument* createConstArg(num x) {
    return new Arguments::Constant(x);
}

void testStateInstructions() {
    cout << "### Testing instructions relying only on program state" << endl;

    ProgramState state;

    resetState(state);
    Instructions::Increment inc(false);
    inc.call(state);
    assertAccEq("Increment", 11);

    resetState(state);
    Instructions::Decrement dec(false);
    dec.call(state);
    assertAccEq("Decrement", 9);

    Arguments::Argument* arg;

    resetState(state);
    arg = createConstArg(3);
    Instructions::Add add(false, arg);
    add.call(state);
    assertAccEq("Add", 13);

    resetState(state);
    arg = createConstArg(3);
    Instructions::Subtract sub(false, arg);
    sub.call(state);
    assertAccEq("Subtract", 7);

    resetState(state);
    arg = createConstArg(15);
    Instructions::ReverseSubtract rsub(false, arg);
    rsub.call(state);
    assertAccEq("Reverse Subtract", 5);

    resetState(state);
    arg = createConstArg(3);
    Instructions::Multiply mul(false, arg);
    mul.call(state);
    assertAccEq("Multiply", 30);

    resetState(state);
    arg = createConstArg(3);
    Instructions::Divide div(false, arg);
    div.call(state);
    assertAccEq("Divide", 3);

    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseDivide rdiv(false, arg);
    rdiv.call(state);
    assertAccEq("Reverse Divide", 2);

    resetState(state);
    arg = createConstArg(3);
    Instructions::Modulo mod(false, arg);
    mod.call(state);
    assertAccEq("Modulo", 1);

    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseModulo rmod(false, arg);
    rmod.call(state);
    assertAccEq("Reverse Modulo", 6);
}

void testMemoryCell() {
    cout << "### Testing the MemoryCell class" << endl;

    MemoryCell topCell (5);
    assert("getVal", topCell.getVal(), == 5);

    MemoryCell* childCell = topCell.getChild();
    assert("getChild", childCell->getVal(), == 0);
    assert("Num Children", topCell.numChildrenInstantiated, == 1);

    childCell->setVal(3);
    assert("setVal", childCell->getVal(), == 3);

    MemoryCell* grandchildCell = childCell->getChild();
    assert("getChild", grandchildCell->getVal(), == 0);

    MemoryCell* nextCell = childCell->getNext();
    assert("getNext", nextCell->getVal(), == 0);
    assert("Num Children", topCell.numChildrenInstantiated, == 2);

    MemoryCell* cellptr = childCell;
    for (int i = 0; i < 4; i++) {
        cellptr = cellptr->getPrev();
    }
    assert("Looping", cellptr, == nextCell);
    assert("Num Children", topCell.numChildrenInstantiated, == 5);
    cellptr = cellptr->getPrev();
    assert("Looping", cellptr, == childCell);
    assert("Num Children", topCell.numChildrenInstantiated, == 5);
    for (int i = 0; i < 100; i++) {
        cellptr = cellptr->getNext();
    }
    assert("Num Children", topCell.numChildrenInstantiated, == 5);
}

