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

    name = "Conditionals (?)";
    resetState(state);
    state.condRegister = false;
    Instructions::Increment inc1(Condition::WHEN_TRUE);
    inc1.run(state);
    assert(state.accRegister, == 10);
    state.condRegister = true;
    inc1.run(state);
    assert(state.accRegister, == 11);

    name = "Conditionals (!)";
    resetState(state);
    state.condRegister = false;
    Instructions::Increment inc2(Condition::WHEN_FALSE);
    inc2.run(state);
    assert(state.accRegister, == 11);
    state.condRegister = true;
    inc2.run(state);
    assert(state.accRegister, == 11);

    name = "Break";
    resetState(state);
    state.condRegister = false;
    Instructions::Break brk(Condition::WHEN_TRUE);
    assertOkay(brk);
    state.condRegister = true;
    assert(brk.run(state), == Status::BREAK);

    name = "Blocks";
    resetState(state);
    state.condRegister = false;
    auto dec1 = instr_ptr(new Instructions::Decrement(Condition::WHEN_TRUE));
    auto sub1 = instr_ptr(new Instructions::Subtract(Condition::ALWAYS, createConstArg(2)));
    auto brk1 = instr_ptr(new Instructions::Break(Condition::WHEN_TRUE));
    auto inv1 = instr_ptr(new Instructions::Invert(Condition::ALWAYS));
    InstructionBlock block (Condition::WHEN_FALSE);
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

