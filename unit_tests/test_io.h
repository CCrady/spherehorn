// test_io.h

#pragma once

#include "../program_state.h"
#include "../memory_cell.h"
#include "../instructions/instructions.h"
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testIOInstructions() {
    startGroup("Testing I/O instructions");

    ProgramState state;
    MemoryCell cell (0);

    name = "Input Char";
    resetState(state, cell);
    Instructions::InputChar chin(false);
    toCin.str("AB");
    chin.run(state);
    assert(cell.getVal(), == 'A');
    assert(cin.get(), == 'B');

    name = "Output Char";
    fromCout.str("");
    Instructions::OutputChar chout(false);
    assertOkay(chout);
    assert(fromCout.str(), == "A");
    cell.setVal(999999); // not valid ASCII
    assertAbort(chout);

    name = "Input Num";
    resetState(state, cell);
    Instructions::InputNum numin(false);
    toCin.str("345\n");
    numin.run(state);
    assert(cell.getVal(), == 345);
    assert(cin.get(), == '\n');

    name = "Output Num";
    fromCout.str("");
    Instructions::OutputNum numout(false);
    numout.run(state);
    assert(fromCout.str(), == "345");

    name = "Input String";
    resetState(state, cell);
    Instructions::InputString strin(false);
    toCin.str("Foo Bar Baz!\nX");
    strin.run(state);
    assert(cell.getVal(), == 12);
    assert(cell.getChild()->getVal(), == 'F');
    assert(cin.get(), == 'X');

    name = "Output String";
    fromCout.str("");
    Instructions::OutputString strout(false);
    strout.run(state);
    assert(fromCout.str(), == "Foo Bar Baz!");

    endGroup();
}

