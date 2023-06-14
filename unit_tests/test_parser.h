// test_parser.h

#include <sstream>
#define private public
#include "../program.h"
#undef private
#include "../memory_cell.h"
#include "unit_tests.h"
using namespace spherehorn;

void testParser() {
    startGroup("Testing the parser");

    name = "Memory block";
    stringstream str1 (
        "("
        "    0xdeadbeef"
        "    '\\n'"
        "    \"FOO\""
        "    ("
        "        4"
        "        0b1010"
        "    )"
        ")"
        ""
        "{"
        "    break;"
        "}"
    );
    Program prog1 (str1);
    assert(prog1.isParseError(), == false);
    MemoryCell parent;
    MemoryCell* child1 = new MemoryCell(0xdeadbeef);
    MemoryCell* child2 = new MemoryCell('\n');
    MemoryCell* child3 = new MemoryCell("FOO");
    MemoryCell* child4 = new MemoryCell();
    MemoryCell* grandchild1 = new MemoryCell(4);
    MemoryCell* grandchild2 = new MemoryCell(0b1010);
    child4->insertChild(grandchild1);
    child4->insertChild(grandchild2);
    parent.insertChild(child1);
    parent.insertChild(child2);
    parent.insertChild(child3);
    parent.insertChild(child4);
    assertCopies(*prog1.memory_, parent);

    name = "Instruction block";
    stringstream str2 (
        "{ !"
        "    numout"
        "    A m"
        "    ++"
        "    .a"
        "    >= 20; ^?"
        "}"
        ""
        "("
        "    0"
        ")"
    );
    Program prog2 (str2);
    assert(prog2.isParseError(), == false);
    prog2.run();
    assert(fromCout.str(), == "012345678910111213141516171819");

    endGroup();
}

