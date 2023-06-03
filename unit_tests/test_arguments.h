// test_arguments.h

#include "../arguments.h"
#include "unit_tests.h"
using namespace spherehorn;

void testArguments() {
    startGroup("Testing arguments");

    ProgramState state;
    resetState(state);
    Arguments::Argument::setStatePtr(&state);

    name = "Constant";
    Arguments::Constant constant (3);
    assert(constant.get(), == 3);

    name = "Accumulator";
    Arguments::Accumulator accumulator;
    assert(accumulator.get(), == 10);
    state.accRegister = 20;
    assert(accumulator.get(), == 20);

    MemoryCell cell (5);
    state.memoryPtr = &cell;

    name = "Memory Cell";
    Arguments::MemoryCell memoryCell;
    assert(memoryCell.get(), == 5);
    cell.setVal(7);
    assert(memoryCell.get(), == 7);

    endGroup();
}
