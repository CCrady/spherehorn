// test_math.h

#pragma once

#include "../program_state.h"
#include "../arguments.h"
#include "../instructions/instructions.h"
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testMathInstructions() {
    startGroup("Testing math instructions");

    ProgramState state;

    name = "Increment";
    resetState(state);
    Instructions::Increment inc(false);
    inc.call(state);
    assertAccEq(11);

    name = "Decrement";
    resetState(state);
    Instructions::Decrement dec(false);
    dec.setNextInstr(&inc);
    assert(dec.call(state), == &inc);
    assertAccEq(9);
    resetState(state);
    state.accRegister = 0;
    assertAbort(dec);

    name = "Invert";
    resetState(state);
    Instructions::Invert inv(false);
    inv.call(state);
    assertCondEq(true);
    inv.call(state);
    assertCondEq(false);

    Arguments::Argument* arg;

    name = "Add";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Add add(false, arg);
    add.call(state);
    assertAccEq(13);

    name = "Subtract";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Subtract sub1(false, arg);
    sub1.setNextInstr(&inc);
    assert(sub1.call(state), == &inc);
    assertAccEq(7);
    resetState(state);
    arg = createConstArg(11);
    Instructions::Subtract sub2(false, arg);
    sub2.setNextInstr(&inc);
    assertAbort(sub2);

    name = "Reverse Subtract";
    resetState(state);
    arg = createConstArg(15);
    Instructions::ReverseSubtract rsub1(false, arg);
    rsub1.setNextInstr(&inc);
    assert(rsub1.call(state), == &inc);
    assertAccEq(5);
    resetState(state);
    arg = createConstArg(3);
    Instructions::ReverseSubtract rsub2(false, arg);
    rsub2.setNextInstr(&inc);
    assertAbort(rsub2);

    name = "Multiply";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Multiply mul(false, arg);
    mul.call(state);
    assertAccEq(30);

    name = "Divide";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Divide div1(false, arg);
    div1.setNextInstr(&inc);
    assert(div1.call(state), == &inc);
    assertAccEq(3);
    resetState(state);
    arg = createConstArg(0);
    Instructions::Divide div2(false, arg);
    div2.setNextInstr(&inc);
    assertAbort(div2);

    name = "Reverse Divide";
    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseDivide rdiv1(false, arg);
    rdiv1.setNextInstr(&inc);
    assert(rdiv1.call(state), == &inc);
    assertAccEq(2);
    resetState(state);
    state.accRegister = 0;
    arg = createConstArg(3);
    Instructions::ReverseDivide rdiv2(false, arg);
    rdiv2.setNextInstr(&inc);
    assertAbort(rdiv2);

    name = "Modulo";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Modulo mod1(false, arg);
    mod1.setNextInstr(&inc);
    assert(mod1.call(state), == &inc);
    assertAccEq(1);
    resetState(state);
    arg = createConstArg(0);
    Instructions::Modulo mod2(false, arg);
    mod2.setNextInstr(&inc);
    assertAbort(mod2);

    name = "Reverse Modulo";
    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseModulo rmod1(false, arg);
    rmod1.setNextInstr(&inc);
    assert(rmod1.call(state), == &inc);
    assertAccEq(6);
    resetState(state);
    state.accRegister = 0;
    arg = createConstArg(3);
    Instructions::ReverseModulo rmod2(false, arg);
    rmod2.setNextInstr(&inc);
    assertAbort(rmod2);

    name = "And";
    resetState(state);
    state.condRegister = true;
    arg = createConstArg(3);
    Instructions::And and1(false, arg);
    and1.call(state);
    assertCondEq(true);
    arg = createConstArg(0);
    Instructions::And and2(false, arg);
    and2.call(state);
    assertCondEq(false);

    name = "Or";
    resetState(state);
    arg = createConstArg(0);
    Instructions::Or or1(false, arg);
    or1.call(state);
    assertCondEq(false);
    arg = createConstArg(3);
    Instructions::Or or2(false, arg);
    or2.call(state);
    assertCondEq(true);

    name = "Xor";
    resetState(state);
    arg = createConstArg(1);
    Instructions::Xor xor1(false, arg);
    xor1.call(state);
    assertCondEq(true);
    xor1.call(state);
    assertCondEq(false);

    endGroup();
}

