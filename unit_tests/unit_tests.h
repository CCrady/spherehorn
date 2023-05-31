// unit_tests.h

#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include "../program_state.h"
#include "../memory_cell.h"
#include "../arguments.h"
using namespace spherehorn;
using namespace std;


// these will be defined by the various test_*.h files
void testMemoryCell();
void testMathInstructions();
void testSetterInstructions();
void testIOInstructions();

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

