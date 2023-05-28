// unit_tests.cpp

// this is terrible but I need to test MemoryCell's internals
#define private public
#include "memory_cell.h"
#undef private

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include "definitions.h"
#include "program_state.h"
#include "arguments.h"
#include "instructions/instructions.h"

using namespace spherehorn;
using namespace std;

// real STDIN/STDOUT buffers, so that when we redirect rin/rout we can still do I/O
istream rin (cin.rdbuf());
ostream rout (cout.rdbuf());

// test instructions that rely only on the program state, not any memory manipulation
void testStateInstructions();
// test the MemoryCell class
void testMemoryCell();
// test instructions that do I/O
void testIOInstructions();

int main() {
    testStateInstructions();
    testMemoryCell();
    testIOInstructions();
    return 0;
}

#define assert(name, var, test) \
    if (var test) { \
        rout << "  PASSED " << setw(20) << name << ":  " #var " " #test << endl; \
    } else { \
        rout << "\u001b[91m! FAILED\u001b[0m " << setw(20) << name << ":  " #var " " #test "\n" \
                #var " = " << var << endl; \
    }

#define assertAccEq(name, val) \
    assert(name, state.accRegister, == val) \
    if (state.condRegister != false || state.memoryPtr != nullptr) { \
        rout << "!\u001b[91m! FAILED\u001b[0m   state side effects: \n" \
                "state.condRegister = " << state.condRegister << "\n" \
                "state.memoryPtr = " << state.memoryPtr << endl; \
    }

void resetState(ProgramState& state) {
    state.accRegister = 10;
    state.condRegister = false;
    state.memoryPtr = nullptr;
}

void resetState(ProgramState& state, MemoryCell& cell) {
    resetState(state);
    state.memoryPtr = &cell;
    cell.setVal(0);
}

Arguments::Argument* createConstArg(num x) {
    return new Arguments::Constant(x);
}

void testStateInstructions() {
    rout << "### Testing instructions relying only on program state" << endl;

    ProgramState state;

    resetState(state);
    Instructions::Increment inc(false);
    inc.call(state);
    assertAccEq("Increment", 11);

    resetState(state);
    Instructions::Decrement dec(false);
    dec.call(state);
    assertAccEq("Decrement", 9);

    resetState(state);
    Instructions::Invert inv(false);
    inv.call(state);
    assert("Invert", state.condRegister, == true);
    inv.call(state);
    assert("Invert", state.condRegister, == false);

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
    rout << "### Testing the MemoryCell class" << endl;

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

void testIOInstructions() {
    rout << "### Testing I/O instructions" << endl;

    stringstream toCin;
    cin.rdbuf(toCin.rdbuf());
    stringstream fromCout;
    cout.rdbuf(fromCout.rdbuf());

    ProgramState state;
    MemoryCell cell (0);

    resetState(state, cell);
    Instructions::InputChar chin(false);
    toCin.str("AB");
    chin.call(state);
    assert("Input Char", cell.getVal(), == 'A');
    assert("Input Char", cin.get(), == 'B');

    fromCout.str("");
    Instructions::OutputChar chout(false);
    chout.call(state);
    assert("Output Char", fromCout.str(), == "A");

    resetState(state, cell);
    Instructions::InputNum numin(false);
    toCin.str("345\n");
    numin.call(state);
    assert("Input Num", cell.getVal(), == 345);
    assert("Input Num", cin.get(), == '\n');

    fromCout.str("");
    Instructions::OutputNum numout(false);
    numout.call(state);
    assert("Output Num", fromCout.str(), == "345");

    resetState(state, cell);
    Instructions::InputString strin(false);
    toCin.str("Foo Bar Baz!\nX");
    strin.call(state);
    assert("Input String", cell.getVal(), == 12);
    assert("Input String", cell.getChild()->getVal(), == 'F');
    assert("Input String", cin.get(), == 'X');

    fromCout.str("");
    Instructions::OutputString strout(false);
    strout.call(state);
    assert("Output String", fromCout.str(), == "Foo Bar Baz!");
}
