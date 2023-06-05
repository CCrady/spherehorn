// test_memory_manipulators.h

#include "../program_state.h"
#include "../memory_cell.h"
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
    Instructions::MemoryUp memup (false);
    assertOkay(memup);
    assert(state.memoryPtr, == &childCell1);
    assertExit(memup);

    name = "Memory Down";
    resetState(state, cell);
    cell.setVal(1);
    MemoryCell& childCell2 = *cell.getChild();
    Instructions::MemoryDown memdown (false);
    assertOkay(memdown);
    assert(state.memoryPtr, == &childCell2);

    // TODO: MemoryPrev, MemoryNext, MemoryBack, MemoryForward

    name = "Memory Prev";
    cell.setVal(6);
    MemoryCell& mainChild = *cell.getChild();
    resetState(state, mainChild);
    MemoryCell& prevChild = *mainChild.getPrev();
    Instructions::MemoryPrev memprev (false);
    assertOkay(memprev);
    assert(state.memoryPtr, == &prevChild);

    name = "Memory Next";
    resetState(state, mainChild);
    MemoryCell& nextChild = *mainChild.getNext();
    Instructions::MemoryNext memnext (false);
    assertOkay(memnext);
    assert(state.memoryPtr, == &nextChild);

    name = "Memory Back";
    resetState(state, mainChild);
    Instructions::MemoryBack memback1 (false, createConstArg(1));
    assertOkay(memback1);
    assert(state.memoryPtr, == &prevChild);
    resetState(state, nextChild);
    Instructions::MemoryBack memback2 (false, createConstArg(2));
    assertOkay(memback2);
    assert(state.memoryPtr, == &prevChild);
    resetState(state, prevChild);
    Instructions::MemoryBack memback4 (false, createConstArg(4));
    assertOkay(memback4);
    assert(state.memoryPtr, == &nextChild);
    resetState(state, mainChild);
    Instructions::MemoryBack memback6 (false, createConstArg(6));
    assertOkay(memback6);
    assert(state.memoryPtr, == &mainChild);

    name = "Memory Forward";
    resetState(state, mainChild);
    Instructions::MemoryForward memfwd1 (false, createConstArg(1));
    assertOkay(memfwd1);
    assert(state.memoryPtr, == &nextChild);
    resetState(state, prevChild);
    Instructions::MemoryForward memfwd2 (false, createConstArg(2));
    assertOkay(memfwd2);
    assert(state.memoryPtr, == &nextChild);
    resetState(state, nextChild);
    Instructions::MemoryForward memfwd4 (false, createConstArg(4));
    assertOkay(memfwd4);
    assert(state.memoryPtr, == &prevChild);
    resetState(state, mainChild);
    Instructions::MemoryForward memfwd6 (false, createConstArg(6));
    assertOkay(memfwd6);
    assert(state.memoryPtr, == &mainChild);

    endGroup();
}

