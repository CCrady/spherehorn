// unit_tests.cpp

#include "test_memory_cell.h"
#include "test_math.h"
#include "test_setters.h"
#include "test_io.h"
#include "unit_tests.h"


int main() {
    replaceStdio();
    testMemoryCell();
    testMathInstructions();
    testSetterInstructions();
    testIOInstructions();
    return 0;
}

