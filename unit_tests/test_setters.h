// test_setters.h

#pragma once

#include "../memory_cell.h"
#include "../instructions/instructions.h"
#include "../arguments.h"
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testSetterInstructions() {
    startGroup("Testing setter instructions");

    ProgramState state;
    arg_ptr arg;

    name = "Set Accumulator";
    resetState(state);
    arg = createConstArg(3);
    Instructions::SetAccumulator setacc(false, arg);
    assertOkay(setacc);
    assertAccEq(3);

    name = "Set Conditional";
    resetState(state);
    arg = createConstArg(3);
    Instructions::SetConditional setcond(false, arg);
    assertOkay(setcond);
    assertCondEq(true);

    name = "Set Memory Value";
    resetState(state);
    arg = createConstArg(3);
    MemoryCell cell(1);
    state.memoryPtr = &cell;
    Instructions::SetMemoryVal setval(false, arg);
    assertOkay(setval);
    assert(cell.getVal(), == 3);
    cell.getChild();
    assertOkay(setval);
    assert(cell.numChildrenInstantiated, == 0);

    endGroup();
}

