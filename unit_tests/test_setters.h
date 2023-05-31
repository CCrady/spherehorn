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

