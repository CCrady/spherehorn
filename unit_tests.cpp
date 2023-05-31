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

void replaceStdio();
void testMemoryCell();
void testMathInstructions();
void testSetterInstructions();
void testIOInstructions();

int main() {
    replaceStdio();
    testMemoryCell();
    testMathInstructions();
    testSetterInstructions();
    testIOInstructions();
    return 0;
}


// real STDIN/STDOUT buffers, so that when we redirect cin/cout we can still do I/O
istream rin (cin.rdbuf());
ostream rout (cout.rdbuf());
ostream rerr (cerr.rdbuf());
// set up fake versions of cin, cout, cerr
stringstream toCin;
stringstream fromCout;
stringstream fromCerr;
void replaceStdio() {
    cin.rdbuf(toCin.rdbuf());
    cout.rdbuf(fromCout.rdbuf());
    cerr.rdbuf(fromCerr.rdbuf());
}

#define startGroup(header) \
    rout << "### " header << endl; \
    int numTests = 0; \
    int numPassed = 0; \
    string name;

#define assert(var, test) \
    { \
    auto val = var; \
    numTests++; \
    if (val test) { \
        numPassed++; \
    } else { \
        rout << "\u001b[91m  FAILED\u001b[0m " << setw(20) << name << ":  " #var " " #test "\n" \
                "  > " #var " = " << val << endl; \
    } \
    }

#define assertAccEq(val) \
    assert(state.accRegister, == val) \
    if (state.condRegister != false || state.memoryPtr != nullptr) { \
        rout << "\u001b[91m  FAILED\u001b[0m   state side effects: \n" \
                "  > state.condRegister = " << state.condRegister << "\n" \
                "  > state.memoryPtr = " << state.memoryPtr << endl; \
    }

#define assertCondEq(val) \
    assert(state.condRegister, == val) \
    if (state.accRegister != 10 || state.memoryPtr != nullptr) { \
        rout << "\u001b[91m  FAILED\u001b[0m   state side effects: \n" \
                "  > state.accRegister = " << state.accRegister << "\n" \
                "  > state.memoryPtr = " << state.memoryPtr << endl; \
    }

#define assertAbort(instr) \
    assert(instr.call(state), == nullptr); \
    assert(fromCerr.str(), != "");

#define endGroup() \
    rout << "Passed " \
         << (numPassed == numTests ? "\u001b[92m" : "\u001b[91m") << numPassed << "/" << numTests \
         << "\u001b[0m tests" << endl;

void testMemoryCell() {
    startGroup("Testing the MemoryCell class");

    name = "getVal";
    MemoryCell topCell (5);
    assert(topCell.getVal(), == 5);

    name = "getChild";
    MemoryCell* childCell = topCell.getChild();
    assert(childCell->getVal(), == 0);
    assert(topCell.numChildrenInstantiated, == 1);

    name = "setVal";
    childCell->setVal(3);
    assert(childCell->getVal(), == 3);

    name = "getChild";
    MemoryCell* grandchildCell = childCell->getChild();
    assert(grandchildCell->getVal(), == 0);

    name = "getNext";
    MemoryCell* nextCell = childCell->getNext();
    assert(nextCell->getVal(), == 0);
    assert(topCell.numChildrenInstantiated, == 2);

    name = "Looping";
    MemoryCell* cellptr = childCell;
    for (int i = 0; i < 4; i++) {
        cellptr = cellptr->getPrev();
    }
    assert(cellptr, == nextCell);
    assert(topCell.numChildrenInstantiated, == 5);
    cellptr = cellptr->getPrev();
    assert(cellptr, == childCell);
    assert(topCell.numChildrenInstantiated, == 5);
    for (int i = 0; i < 100; i++) {
        cellptr = cellptr->getNext();
    }
    assert(topCell.numChildrenInstantiated, == 5);

    name = "Shift multiple positions";
    assert(childCell->shiftBack(5), == childCell);
    assert(childCell->shiftForward(10), == childCell);
    assert(childCell->shiftBack(6), == childCell->getPrev());
    assert(childCell->shiftBack(4), == childCell->getNext());
    assert(childCell->shiftForward(2), == childCell->getNext()->getNext());

    name = "Insert";
    MemoryCell insertParent (5);
    MemoryCell& firstChild = *insertParent.getChild();
    MemoryCell& prevChild = *firstChild.getPrev();
    MemoryCell& newPrevChild = *firstChild.insertBefore(3);
    assert(insertParent.getVal(), == 6);
    assert(newPrevChild.getVal(), == 3);
    assert(firstChild.getPrev(), == &newPrevChild);
    assert(newPrevChild.getNext(), == &firstChild);
    assert(newPrevChild.getPrev(), == &prevChild);
    assert(prevChild.getNext(), == &newPrevChild);
    MemoryCell& newNextChild = *firstChild.insertAfter();
    assert(firstChild.getNext(), == &newNextChild);
    assert(newNextChild.getPrev(), == &firstChild);

    name = "Constructor from string";
    string fromStr = "ABC";
    MemoryCell cellABC = MemoryCell(fromStr);
    assert(cellABC.getVal(), == 3);
    MemoryCell* childA = cellABC.getChild();
    assert(childA->getVal(), == 'A');
    MemoryCell* childB = childA->getNext();
    assert(childB->getVal(), == 'B');
    MemoryCell* childC = childB->getNext();
    assert(childC->getVal(), == 'C');

    endGroup();
}

void resetState(ProgramState& state) {
    state.accRegister = 10;
    state.condRegister = false;
    state.memoryPtr = nullptr;
    toCin.str("");
    fromCout.str("");
    fromCerr.str("");
}

void resetState(ProgramState& state, MemoryCell& cell) {
    resetState(state);
    state.memoryPtr = &cell;
    cell.setVal(0);
}

Arguments::Argument* createConstArg(num x) {
    return new Arguments::Constant(x);
}

void testMathInstructions() {
    startGroup("Testing math instructions");

    ProgramState state;

    name = "Increment";
    resetState(state);
    Instructions::Increment inc(false);
    inc.call(state);
    assertAccEq(11);

    name = "Decrement";
    resetState(state);
    Instructions::Decrement dec(false);
    dec.setNextInstr(&inc);
    assert(dec.call(state), == &inc);
    assertAccEq(9);
    resetState(state);
    state.accRegister = 0;
    assertAbort(dec);

    name = "Invert";
    resetState(state);
    Instructions::Invert inv(false);
    inv.call(state);
    assertCondEq(true);
    inv.call(state);
    assertCondEq(false);

    Arguments::Argument* arg;

    name = "Add";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Add add(false, arg);
    add.call(state);
    assertAccEq(13);

    name = "Subtract";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Subtract sub1(false, arg);
    sub1.setNextInstr(&inc);
    assert(sub1.call(state), == &inc);
    assertAccEq(7);
    resetState(state);
    arg = createConstArg(11);
    Instructions::Subtract sub2(false, arg);
    sub2.setNextInstr(&inc);
    assertAbort(sub2);

    name = "Reverse Subtract";
    resetState(state);
    arg = createConstArg(15);
    Instructions::ReverseSubtract rsub1(false, arg);
    rsub1.setNextInstr(&inc);
    assert(rsub1.call(state), == &inc);
    assertAccEq(5);
    resetState(state);
    arg = createConstArg(3);
    Instructions::ReverseSubtract rsub2(false, arg);
    rsub2.setNextInstr(&inc);
    assertAbort(rsub2);

    name = "Multiply";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Multiply mul(false, arg);
    mul.call(state);
    assertAccEq(30);

    name = "Divide";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Divide div1(false, arg);
    div1.setNextInstr(&inc);
    assert(div1.call(state), == &inc);
    assertAccEq(3);
    resetState(state);
    arg = createConstArg(0);
    Instructions::Divide div2(false, arg);
    div2.setNextInstr(&inc);
    assertAbort(div2);

    name = "Reverse Divide";
    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseDivide rdiv1(false, arg);
    rdiv1.setNextInstr(&inc);
    assert(rdiv1.call(state), == &inc);
    assertAccEq(2);
    resetState(state);
    state.accRegister = 0;
    arg = createConstArg(3);
    Instructions::ReverseDivide rdiv2(false, arg);
    rdiv2.setNextInstr(&inc);
    assertAbort(rdiv2);

    name = "Modulo";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Modulo mod1(false, arg);
    mod1.setNextInstr(&inc);
    assert(mod1.call(state), == &inc);
    assertAccEq(1);
    resetState(state);
    arg = createConstArg(0);
    Instructions::Modulo mod2(false, arg);
    mod2.setNextInstr(&inc);
    assertAbort(mod2);

    name = "Reverse Modulo";
    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseModulo rmod1(false, arg);
    rmod1.setNextInstr(&inc);
    assert(rmod1.call(state), == &inc);
    assertAccEq(6);
    resetState(state);
    state.accRegister = 0;
    arg = createConstArg(3);
    Instructions::ReverseModulo rmod2(false, arg);
    rmod2.setNextInstr(&inc);
    assertAbort(rmod2);

    name = "And";
    resetState(state);
    state.condRegister = true;
    arg = createConstArg(3);
    Instructions::And and1(false, arg);
    and1.call(state);
    assertCondEq(true);
    arg = createConstArg(0);
    Instructions::And and2(false, arg);
    and2.call(state);
    assertCondEq(false);

    name = "Or";
    resetState(state);
    arg = createConstArg(0);
    Instructions::Or or1(false, arg);
    or1.call(state);
    assertCondEq(false);
    arg = createConstArg(3);
    Instructions::Or or2(false, arg);
    or2.call(state);
    assertCondEq(true);

    name = "Xor";
    resetState(state);
    arg = createConstArg(1);
    Instructions::Xor xor1(false, arg);
    xor1.call(state);
    assertCondEq(true);
    xor1.call(state);
    assertCondEq(false);

    endGroup();
}

void testSetterInstructions() {
    startGroup("Testing setter instructions");

    ProgramState state;
    Arguments::Argument* arg;

    name = "Set Accumulator";
    resetState(state);
    arg = createConstArg(3);
    Instructions::SetAccumulator setacc(false, arg);
    setacc.call(state);
    assertAccEq(3);

    name = "Set Conditional";
    resetState(state);
    arg = createConstArg(3);
    Instructions::SetConditional setcond(false, arg);
    setcond.call(state);
    assertCondEq(true);

    name = "Set Memory Value";
    resetState(state);
    arg = createConstArg(3);
    MemoryCell cell(1);
    state.memoryPtr = &cell;
    Instructions::SetMemoryVal setval(false, arg);
    setval.call(state);
    assert(cell.getVal(), == 3);
    cell.getChild();
    setval.call(state);
    assert(cell.numChildrenInstantiated, == 0);

    endGroup();
}

void testIOInstructions() {
    startGroup("Testing I/O instructions");

    ProgramState state;
    MemoryCell cell (0);

    name = "Input Char";
    resetState(state, cell);
    Instructions::InputChar chin(false);
    toCin.str("AB");
    chin.call(state);
    assert(cell.getVal(), == 'A');
    assert(cin.get(), == 'B');

    name = "Output Char";
    fromCout.str("");
    Instructions::OutputChar chout(false);
    chout.setNextInstr(&chin);
    assert(chout.call(state), == &chin);
    assert(fromCout.str(), == "A");
    cell.setVal(999999); // not valid ASCII
    assertAbort(chout);

    name = "Input Num";
    resetState(state, cell);
    Instructions::InputNum numin(false);
    toCin.str("345\n");
    numin.call(state);
    assert(cell.getVal(), == 345);
    assert(cin.get(), == '\n');

    name = "Output Num";
    fromCout.str("");
    Instructions::OutputNum numout(false);
    numout.call(state);
    assert(fromCout.str(), == "345");

    name = "Input String";
    resetState(state, cell);
    Instructions::InputString strin(false);
    toCin.str("Foo Bar Baz!\nX");
    strin.call(state);
    assert(cell.getVal(), == 12);
    assert(cell.getChild()->getVal(), == 'F');
    assert(cin.get(), == 'X');

    name = "Output String";
    fromCout.str("");
    Instructions::OutputString strout(false);
    strout.call(state);
    assert(fromCout.str(), == "Foo Bar Baz!");

    endGroup();
}
