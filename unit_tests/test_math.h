// test_math.h

#pragma once

#include "../src/program_state.h"
#include "../src/arguments.h"
#include "../src/instructions/instructions.h"
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testMathInstructions() {
    startGroup("Testing math instructions");

    ProgramState state;

    name = "Increment";
    resetState(state);
    Instructions::Increment inc(Condition::ALWAYS);
    assertOkay(inc);
    assertAccEq(11);

    name = "Decrement";
    resetState(state);
    Instructions::Decrement dec(Condition::ALWAYS);
    assertOkay(dec);
    assertAccEq(9);
    resetState(state);
    state.accRegister = 0;
    assertAbort(dec);

    name = "Invert";
    resetState(state);
    Instructions::Invert inv(Condition::ALWAYS);
    assertOkay(inv);
    assertCondEq(true);
    assertOkay(inv);
    assertCondEq(false);

    arg_ptr arg;

    name = "Add";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Add add(Condition::ALWAYS, arg);
    assertOkay(add);
    assertAccEq(13);

    name = "Subtract";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Subtract sub1(Condition::ALWAYS, arg);
    assertOkay(sub1);
    assertAccEq(7);
    resetState(state);
    arg = createConstArg(11);
    Instructions::Subtract sub2(Condition::ALWAYS, arg);
    assertAbort(sub2);

    name = "Reverse Subtract";
    resetState(state);
    arg = createConstArg(15);
    Instructions::ReverseSubtract rsub1(Condition::ALWAYS, arg);
    assertOkay(rsub1);
    assertAccEq(5);
    resetState(state);
    arg = createConstArg(3);
    Instructions::ReverseSubtract rsub2(Condition::ALWAYS, arg);
    assertAbort(rsub2);

    name = "Multiply";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Multiply mul(Condition::ALWAYS, arg);
    assertOkay(mul);
    assertAccEq(30);

    name = "Divide";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Divide div1(Condition::ALWAYS, arg);
    assertOkay(div1);
    assertAccEq(3);
    resetState(state);
    arg = createConstArg(0);
    Instructions::Divide div2(Condition::ALWAYS, arg);
    assertAbort(div2);

    name = "Reverse Divide";
    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseDivide rdiv1(Condition::ALWAYS, arg);
    assertOkay(rdiv1);
    assertAccEq(2);
    resetState(state);
    state.accRegister = 0;
    arg = createConstArg(3);
    Instructions::ReverseDivide rdiv2(Condition::ALWAYS, arg);
    assertAbort(rdiv2);

    name = "Modulo";
    resetState(state);
    arg = createConstArg(3);
    Instructions::Modulo mod1(Condition::ALWAYS, arg);
    assertOkay(mod1);
    assertAccEq(1);
    resetState(state);
    arg = createConstArg(0);
    Instructions::Modulo mod2(Condition::ALWAYS, arg);
    assertAbort(mod2);

    name = "Reverse Modulo";
    resetState(state);
    arg = createConstArg(26);
    Instructions::ReverseModulo rmod1(Condition::ALWAYS, arg);
    assertOkay(rmod1);
    assertAccEq(6);
    resetState(state);
    state.accRegister = 0;
    arg = createConstArg(3);
    Instructions::ReverseModulo rmod2(Condition::ALWAYS, arg);
    assertAbort(rmod2);

    name = "And";
    resetState(state);
    state.condRegister = true;
    arg = createConstArg(3);
    Instructions::And and1(Condition::ALWAYS, arg);
    assertOkay(and1);
    assertCondEq(true);
    arg = createConstArg(0);
    Instructions::And and2(Condition::ALWAYS, arg);
    assertOkay(and2);
    assertCondEq(false);

    name = "Or";
    resetState(state);
    arg = createConstArg(0);
    Instructions::Or or1(Condition::ALWAYS, arg);
    assertOkay(or1);
    assertCondEq(false);
    arg = createConstArg(3);
    Instructions::Or or2(Condition::ALWAYS, arg);
    assertOkay(or2);
    assertCondEq(true);

    name = "Xor";
    resetState(state);
    arg = createConstArg(1);
    Instructions::Xor xor1(Condition::ALWAYS, arg);
    assertOkay(xor1);
    assertCondEq(true);
    assertOkay(xor1);
    assertCondEq(false);

    endGroup();
}

