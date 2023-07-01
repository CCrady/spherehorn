// test_io.h

#pragma once

#include "../src/program_state.h"
#include "../src/memory_cell.h"
#include "../src/instructions/instructions.h"
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testIOInstructions() {
    startGroup("Testing I/O instructions");

    ProgramState state;
    MemoryCell cell (0);

    name = "Input Char";
    resetState(state, cell);
    Instructions::InputChar chin(Condition::ALWAYS);
    toCin.str("AB");
    assertOkay(chin);
    assert(cell.getVal(), == 'A');
    assert(cin.get(), == 'B');
    toCin.str("\n\tX");
    assertOkay(chin);
    assert(cell.getVal(), == '\n');
    assert(cin.get(), == '\t');
    assert(cin.get(), == 'X');

    name = "Output Char";
    resetState(state, cell);
    cell.setVal(static_cast<num>('A'));
    Instructions::OutputChar chout(Condition::ALWAYS);
    fromCout.str("");
    assertOkay(chout);
    assert(fromCout.str(), == "A");
    cell.setVal(static_cast<num>('\n'));
    fromCout.str("");
    assertOkay(chout);
    assert(fromCout.str(), == "\n");
    cell.setVal(999999); // not valid ASCII
    assertAbort(chout);

    name = "Input Num";
    resetState(state, cell);
    Instructions::InputNum numin(Condition::ALWAYS);
    toCin.str("345\n");
    assertOkay(numin);
    assert(cell.getVal(), == 345);
    assert(cin.get(), == '\n');

    name = "Output Num";
    fromCout.str("");
    Instructions::OutputNum numout(Condition::ALWAYS);
    assertOkay(numout);
    assert(fromCout.str(), == "345");

    name = "Input String";
    resetState(state, cell);
    Instructions::InputString strin(Condition::ALWAYS);
    toCin.str("Foo Bar Baz!\nX");
    assertOkay(strin);
    assert(cell.getVal(), == 12);
    assert(cell.getChild()->getVal(), == 'F');
    assert(cin.get(), == 'X');

    name = "Output String";
    fromCout.str("");
    Instructions::OutputString strout(Condition::ALWAYS);
    assertOkay(strout);
    assert(fromCout.str(), == "Foo Bar Baz!");

    endGroup();
}

