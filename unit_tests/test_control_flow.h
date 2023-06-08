// test_control_flow.h

#pragma once

#include <memory>
#include "../program_state.h"
#include "../memory_cell.h"
#include "../instruction_block.h"
#include "../instructions/instructions.h"
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testControlFlow() {
    startGroup("Test control flow");

    ProgramState state;

    name = "Conditionals";
    resetState(state);
    state.condRegister = false;
    Instructions::Increment inc(true);
    inc.run(state);
    assert(state.accRegister, == 10);
    state.condRegister = true;
    inc.run(state);
    assert(state.accRegister, == 11);

    name = "Break";
    resetState(state);
    state.condRegister = false;
    Instructions::Break brk(true);
    assertOkay(brk);
    state.condRegister = true;
    assert(brk.run(state), == Status::BREAK);

    name = "Blocks";
    resetState(state);
    state.condRegister = false;
    auto dec1 = instr_ptr(new Instructions::Decrement(true));
    auto sub1 = instr_ptr(new Instructions::Subtract(false, createConstArg(2)));
    auto brk1 = instr_ptr(new Instructions::Break(true));
    auto inv1 = instr_ptr(new Instructions::Invert(false));
    InstructionBlock block;
    block.insertInstr(dec1);
    block.insertInstr(sub1);
    block.insertInstr(brk1);
    block.insertInstr(inv1);
    // block is now:
    // {
    //     dec?
    //     sub 2;
    //     break?
    //     invert;
    // }
    assertOkay(block);
    assert(state.condRegister, == true);
    assert(state.accRegister, == 5);

    endGroup();
}

