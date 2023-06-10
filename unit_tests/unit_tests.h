// unit_tests.h

#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
// this is terrible but I need to test MemoryCell's internals
#define private public
#include "../memory_cell.h"
#undef private
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

arg_ptr createConstArg(num x) {
    return arg_ptr(new Arguments::Constant(x));
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

#define assertCopies(lhs, rhs) \
    { \
        MemoryCell& lhsVal = lhs; \
        MemoryCell& rhsVal = rhs; \
        numTests++; \
        if (areCellsCopies(lhsVal, rhsVal)) { \
            numPassed++; \
        } else { \
            rout << "\u001b[91m  FAILED\u001b[0m " << setw(20) << name << ":  areCellsCopies(" #lhs ", " #rhs ")\n" \
                    "  > " #lhs " = " << lhsVal << "\n" \
                    "  > " #rhs " = " << rhsVal << endl; \
        } \
    }

bool areCellsCopies(const spherehorn::MemoryCell& lhs, const spherehorn::MemoryCell& rhs) {
    // if lhs and rhs have different values or numbers of children, return false
    if (lhs.getVal() != rhs.getVal() || lhs.numChildrenInstantiated != rhs.numChildrenInstantiated)
        return false;
    // if lhs and rhs refer to the same object, return false
    if (&lhs == &rhs)
        return false;
    // if lhs and rhs have the same value and no instantiated children, return true
    if (lhs.getVal() == rhs.getVal() && lhs.numChildrenInstantiated == 0 && rhs.numChildrenInstantiated == 0)
        return true;

    // check whether the first children are copies
    spherehorn::MemoryCell* currLhsChild = lhs.firstChild;
    spherehorn::MemoryCell* currRhsChild = rhs.firstChild;
    // recursive call
    if (!areCellsCopies(*currLhsChild, *currRhsChild)) return false;
    // ensure parents are set correctly
    if (currLhsChild->getParent() != &lhs || currRhsChild->getParent() != &rhs) return false;
    // iterate forwards over lhs and rhs's children, starting from the second child
    for (currLhsChild = lhs.firstChild->nextSibling, currRhsChild = rhs.firstChild->nextSibling;
         // currLhsChild & currRhsChild aren't null and we haven't looped around
         currLhsChild && currRhsChild && currLhsChild != lhs.firstChild;
         currLhsChild = currLhsChild->nextSibling, currRhsChild = currRhsChild->nextSibling) {
        // recursive call
        if (!areCellsCopies(*currLhsChild, *currRhsChild)) return false;
        // ensure parents are set correctly
        if (currLhsChild->getParent() != &lhs || currRhsChild->getParent() != &rhs) return false;
    }
    // if one of them is null and the other isn't, return false
    if ((currLhsChild && !currRhsChild) || (!currLhsChild && currRhsChild)) return false;
    // iterate backwards over lhs and rhs's children, starting from the last child
    for (currLhsChild = lhs.firstChild->prevSibling, currRhsChild = rhs.firstChild->prevSibling;
         // currLhsChild & currRhsChild aren't null and we haven't looped around
         currLhsChild && currRhsChild && currLhsChild != lhs.firstChild;
         currLhsChild = currLhsChild->prevSibling, currRhsChild = currRhsChild->prevSibling) {
        // recursive call
        if (!areCellsCopies(*currLhsChild, *currRhsChild)) return false;
        // ensure parents are set correctly
        if (currLhsChild->getParent() != &lhs || currRhsChild->getParent() != &rhs) return false;
    }
    // if one of them is null and the other isn't, return false
    if ((currLhsChild && !currRhsChild) || (!currLhsChild && currRhsChild)) return false;
    // if we haven't returned by now, lhs and rhs must be equal
    return true;
}

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

std::ostream& operator<<(std::ostream& out, spherehorn::MemoryCell cell) {
    out << cell.getVal() << " @ " << &cell;
    // if we haven't instantiated any children, then we're done
    if (cell.numChildrenInstantiated == 0) return out;

    out << ": ( ";
    spherehorn::MemoryCell* firstChild = cell.getChild();
    out << *firstChild << " ";
    for (spherehorn::MemoryCell* currChild = firstChild->getNext();
         currChild != firstChild;
         currChild = currChild->getNext()) {
        out << *currChild << " ";
    }
    out << ")";
    return out;
}

