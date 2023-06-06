// unit_tests.cpp

#include "test_memory_cell.h"
#include "test_arguments.h"
#include "test_math.h"
#include "test_setters.h"
#include "test_io.h"
#include "test_comparisons.h"
#include "test_memory_manipulators.h"
#include "test_control_flow.h"
#include "test_tokenizer.h"
#include "unit_tests.h"


int main() {
    replaceStdio();
    testMemoryCell();
    testArguments();
    testMathInstructions();
    testSetterInstructions();
    testIOInstructions();
    testComparisonInstructions();
    testMemoryManipulatorInstructions();
    testControlFlow();
    testTokenizer();
    return 0;
}

