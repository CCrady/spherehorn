// test_memory_manipulators.h

#include "../program_state.h"
#include "../memory_cell.h"
#include "../instructions/instructions.h"
#include "unit_tests.h"

void testMemoryManipulatorInstructions() {
    startGroup("Testing memory manipulation instructions");

    ProgramState state;
    MemoryCell cell;

    name = "Memory Up";
    cell.setVal(1);
    MemoryCell& childCell1 = *cell.getChild();
    childCell1.setVal(1);
    MemoryCell& grandchildCell1 = *childCell1.getChild();
    resetState(state, grandchildCell1);
    Instructions::MemoryUp memup (Condition::ALWAYS);
    assertOkay(memup);
    assert(state.memoryPtr, == &childCell1);
    assertExit(memup);

    name = "Memory Down";
    resetState(state, cell);
    cell.setVal(1);
    MemoryCell& childCell2 = *cell.getChild();
    Instructions::MemoryDown memdown (Condition::ALWAYS);
    assertOkay(memdown);
    assert(state.memoryPtr, == &childCell2);

    name = "Memory Prev";
    cell.setVal(6);
    MemoryCell& mainChild = *cell.getChild();
    resetState(state, mainChild);
    MemoryCell& prevChild = *mainChild.getPrev();
    Instructions::MemoryPrev memprev (Condition::ALWAYS);
    assertOkay(memprev);
    assert(state.memoryPtr, == &prevChild);

    name = "Memory Next";
    resetState(state, mainChild);
    MemoryCell& nextChild = *mainChild.getNext();
    Instructions::MemoryNext memnext (Condition::ALWAYS);
    assertOkay(memnext);
    assert(state.memoryPtr, == &nextChild);

    name = "Memory Back";
    resetState(state, mainChild);
    Instructions::MemoryBack memback1 (Condition::ALWAYS, createConstArg(1));
    assertOkay(memback1);
    assert(state.memoryPtr, == &prevChild);
    resetState(state, nextChild);
    Instructions::MemoryBack memback2 (Condition::ALWAYS, createConstArg(2));
    assertOkay(memback2);
    assert(state.memoryPtr, == &prevChild);
    resetState(state, prevChild);
    Instructions::MemoryBack memback4 (Condition::ALWAYS, createConstArg(4));
    assertOkay(memback4);
    assert(state.memoryPtr, == &nextChild);
    resetState(state, mainChild);
    Instructions::MemoryBack memback6 (Condition::ALWAYS, createConstArg(6));
    assertOkay(memback6);
    assert(state.memoryPtr, == &mainChild);

    name = "Memory Forward";
    resetState(state, mainChild);
    Instructions::MemoryForward memfwd1 (Condition::ALWAYS, createConstArg(1));
    assertOkay(memfwd1);
    assert(state.memoryPtr, == &nextChild);
    resetState(state, prevChild);
    Instructions::MemoryForward memfwd2 (Condition::ALWAYS, createConstArg(2));
    assertOkay(memfwd2);
    assert(state.memoryPtr, == &nextChild);
    resetState(state, nextChild);
    Instructions::MemoryForward memfwd4 (Condition::ALWAYS, createConstArg(4));
    assertOkay(memfwd4);
    assert(state.memoryPtr, == &prevChild);
    resetState(state, mainChild);
    Instructions::MemoryForward memfwd6 (Condition::ALWAYS, createConstArg(6));
    assertOkay(memfwd6);
    assert(state.memoryPtr, == &mainChild);

    name = "Memory Restart";
    resetState(state, mainChild);
    Instructions::MemoryRestart memrestart (Condition::ALWAYS);
    assertOkay(memrestart);
    assert(state.memoryPtr, == &mainChild);
    resetState(state, nextChild);
    assertOkay(memrestart);
    assert(state.memoryPtr, == &mainChild);
    resetState(state, prevChild);
    assertOkay(memrestart);
    assert(state.memoryPtr, == &mainChild);

    name = "Memory Rotate";
    resetState(state, mainChild);
    Instructions::MemoryRotate memrotate (Condition::ALWAYS);
    assertOkay(memrotate);
    assert(cell.getChild(), == &mainChild);
    resetState(state, nextChild);
    assertOkay(memrotate);
    assert(cell.getChild(), == &nextChild);
    resetState(state, prevChild);
    assertOkay(memrotate);
    assert(cell.getChild(), == &prevChild);
    resetState(state, prevChild);
    assertOkay(memrotate);
    assert(cell.getChild(), == &prevChild);

    name = "Memory setter";
    resetState(state, cell);
    MemoryCell setCell (4);
    MemoryCell& setCellChild = *setCell.getChild();
    setCellChild.setVal(5);
    Instructions::SetMemory setmem (Condition::ALWAYS, setCell);
    assertOkay(setmem);
    assertCopies(cell, setCell);
    MemoryCell& childCell3 = *cell.getChild();
    resetState(state, childCell3);
    childCell3.getChild()->setVal(2);
    assertOkay(setmem);
    assertCopies(childCell3, setCell);
    assert(childCell3.getParent(), == &cell);

    endGroup();
}

