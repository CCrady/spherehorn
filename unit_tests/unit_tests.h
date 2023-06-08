// unit_tests.h

#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include "../program_state.h"
#include "../memory_cell.h"
#include "../arguments.h"
#include "../instruction_container.h"
#include "../tokenizer.h"
using namespace spherehorn;
using namespace std;


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

#define assertOkay(instr) \
    assert(instr.run(state), == spherehorn::Status::OKAY); \
    assert(fromCerr.str(), == "");

#define assertExit(instr) \
    assert(instr.run(state), == spherehorn::Status::EXIT); \
    assert(fromCerr.str(), == "");

#define assertAbort(instr) \
    assert(instr.run(state), == spherehorn::Status::ABORT); \
    assert(fromCerr.str(), != "");

#define assertAccEq(val) \
    assert(state.accRegister, == val); \
    if (state.condRegister != false || state.memoryPtr != nullptr) { \
        rout << "\u001b[91m  FAILED\u001b[0m   state side effects: \n" \
                "  > state.condRegister = " << state.condRegister << "\n" \
                "  > state.memoryPtr = " << state.memoryPtr << endl; \
    }

#define assertCondEq(val) \
    assert(state.condRegister, == val); \
    if (state.accRegister != 10 || state.memoryPtr != nullptr) { \
        rout << "\u001b[91m  FAILED\u001b[0m   state side effects: \n" \
                "  > state.accRegister = " << state.accRegister << "\n" \
                "  > state.memoryPtr = " << state.memoryPtr << endl; \
    }

#define assertTokenEq(ttype, tstring) \
    assert(token.type, == Token::ttype); \
    assert(token.str, == tstring);

#define endGroup() \
    rout << "Passed " \
         << (numPassed == numTests ? "\u001b[92m" : "\u001b[91m") << numPassed << "/" << numTests \
         << "\u001b[0m tests" << endl;

#define printEnumCase(val) \
    case spherehorn::val: \
        out << #val; \
        break;

std::ostream& operator <<(std::ostream& out, spherehorn::Status status) {
    switch (status) {
        printEnumCase(Status::OKAY);
        printEnumCase(Status::BREAK);
        printEnumCase(Status::EXIT);
        printEnumCase(Status::ABORT);
    }
    return out;
}

std::ostream& operator <<(std::ostream& out, spherehorn::Token::TokenType type) {
    switch (type) {
        printEnumCase(Token::INSTRUCTION);
        printEnumCase(Token::MEMSET);
        printEnumCase(Token::VARIABLE);
        printEnumCase(Token::TERMINATOR);
        printEnumCase(Token::GROUPING);
        printEnumCase(Token::NUMBER);
        printEnumCase(Token::STRING);
        printEnumCase(Token::CHAR);
        printEnumCase(Token::BOOL);
        printEnumCase(Token::END);
    }
    return out;
}

#undef printEnumCase

std::ostream& operator<<(std::ostream& out, const spherehorn::ProgramState& state) {
    out << "ProgramState @ " << &state << " {\n"
           "    accRegister = " << state.accRegister << "\n"
           "    condRegister = " << state.condRegister << "\n"
           "    memoryPtr = " << state.memoryPtr << "\n"
           "}" << std::endl;
    return out;
}

