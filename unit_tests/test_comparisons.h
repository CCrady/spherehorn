// test_comparisons.h

#include "../program_state.h"
#include "../instructions/instructions.h"
#include "unit_tests.h"

void testComparisonInstructions() {
    startGroup("Testing numerical comparisons");

    ProgramState state;

    name = "Greater than";
    resetState(state);
    Instructions::Greater gt9 (Condition::ALWAYS, createConstArg(9));
    assertOkay(gt9);
    assertCondEq(true);
    resetState(state);
    Instructions::Greater gt10 (Condition::ALWAYS, createConstArg(10));
    assertOkay(gt10);
    assertCondEq(false);
    resetState(state);
    Instructions::Greater gt11 (Condition::ALWAYS, createConstArg(11));
    assertOkay(gt11);
    assertCondEq(false);

    name = "Equal to";
    resetState(state);
    Instructions::Equal eq9 (Condition::ALWAYS, createConstArg(9));
    assertOkay(eq9);
    assertCondEq(false);
    resetState(state);
    Instructions::Equal eq10 (Condition::ALWAYS, createConstArg(10));
    assertOkay(eq10);
    assertCondEq(true);
    resetState(state);
    Instructions::Equal eq11 (Condition::ALWAYS, createConstArg(11));
    assertOkay(eq11);
    assertCondEq(false);

    name = "Less than";
    resetState(state);
    Instructions::Less lt9 (Condition::ALWAYS, createConstArg(9));
    assertOkay(lt9);
    assertCondEq(false);
    resetState(state);
    Instructions::Less lt10 (Condition::ALWAYS, createConstArg(10));
    assertOkay(lt10);
    assertCondEq(false);
    resetState(state);
    Instructions::Less lt11 (Condition::ALWAYS, createConstArg(11));
    assertOkay(lt11);
    assertCondEq(true);

    name = "Greater than or equal to";
    resetState(state);
    Instructions::GreaterOrEqual ge9 (Condition::ALWAYS, createConstArg(9));
    assertOkay(ge9);
    assertCondEq(true);
    resetState(state);
    Instructions::GreaterOrEqual ge10 (Condition::ALWAYS, createConstArg(10));
    assertOkay(ge10);
    assertCondEq(true);
    resetState(state);
    Instructions::GreaterOrEqual ge11 (Condition::ALWAYS, createConstArg(11));
    assertOkay(ge11);
    assertCondEq(false);

    name = "Less than or equal to";
    resetState(state);
    Instructions::LessOrEqual le9 (Condition::ALWAYS, createConstArg(9));
    assertOkay(le9);
    assertCondEq(false);
    resetState(state);
    Instructions::LessOrEqual le10 (Condition::ALWAYS, createConstArg(10));
    assertOkay(le10);
    assertCondEq(true);
    resetState(state);
    Instructions::LessOrEqual le11 (Condition::ALWAYS, createConstArg(11));
    assertOkay(le11);
    assertCondEq(true);

    name = "Not equal to";
    resetState(state);
    Instructions::NotEqual ne9 (Condition::ALWAYS, createConstArg(9));
    assertOkay(ne9);
    assertCondEq(true);
    resetState(state);
    Instructions::NotEqual ne10 (Condition::ALWAYS, createConstArg(10));
    assertOkay(ne10);
    assertCondEq(false);
    resetState(state);
    Instructions::NotEqual ne11 (Condition::ALWAYS, createConstArg(11));
    assertOkay(ne11);
    assertCondEq(true);

    endGroup();
}
