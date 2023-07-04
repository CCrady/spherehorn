// test_parser.h

#include <utility>
#include <sstream>
#define private public
#include "../src/program.h"
#undef private
#include "../src/memory_cell.h"
#include "unit_tests.h"
using namespace spherehorn;

#define assertParseError(program) \
    assert(program.isParseError(),); \
    assert(fromCerr.str(), .find("Parse error:") == 0);

void testParser() {
    startGroup("Testing the parser");

    {
        name = "Memory block";
        stringstream str (
            "("
            "    0xdeadbeef"
            "    '\\n'"
            "    \"FOO\\x7e\""
            "    \"bar\"&"
            "    \"baz\""
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
        Program prog (std::move(str));
        assert(prog.isParseError(), == false);
        MemoryCell parent;
        MemoryCell* child1 = new MemoryCell(0xdeadbeef);
        MemoryCell* child2 = new MemoryCell('\n');
        MemoryCell* child3 = new MemoryCell("FOO~");
        MemoryCell* child4 = new MemoryCell("barbaz");
        MemoryCell* child5 = new MemoryCell();
        MemoryCell* grandchild1 = new MemoryCell(4);
        MemoryCell* grandchild2 = new MemoryCell(0b1010);
        child5->insertChild(grandchild1);
        child5->insertChild(grandchild2);
        parent.insertChild(child1);
        parent.insertChild(child2);
        parent.insertChild(child3);
        parent.insertChild(child4);
        parent.insertChild(child5);
        assertCopies(*prog.memory_, parent);
    }

    {
        name = "String concatenation";
        stringstream str (
            "("
            "    \"foo\"&"
            "    \" bar\""
            "    &\"baz\""
            ")"
            "{ break }"
        );
        Program prog (std::move(str));
        assert(prog.isParseError(), == false);
        MemoryCell parent;
        MemoryCell* child = new MemoryCell("foo barbaz");
        parent.insertChild(child);
        assertCopies(*prog.memory_, parent);
    }

    {
        name = "Instruction block";
        fromCout.str("");
        stringstream str (
            "{ !"
            "    numout"
            "    A m"
            "    { ?"
            "        + 5"
            "    }"
            "    ++"
            "    .a"
            "    >= 20; ^?"
            "}"
            ""
            "("
            "    0"
            ")"
        );
        Program prog (std::move(str));
        assert(prog.isParseError(), == false);
        prog.run();
        assert(fromCout.str(), == "012345678910111213141516171819");
    }

    {
        name = "Acc/cond initial values";
        stringstream str (
            "a: 'c' "
            "c: 0b1010 "
            "{break} "
            "(0) "
        );
        Program prog (std::move(str));
        assert(prog.isParseError(), == false);
        assert(prog.state_.accRegister, == 'c');
        assert(prog.state_.condRegister, == true);
    }

    {
        name = "Non-block initial memory";
        stringstream str1 (
            "\"foo\"&\"bar\""
            ""
            "{break}"
        );
        Program prog1 (std::move(str1));
        assert(prog1.isParseError(), == false);
        MemoryCell cell1 ("foobar");
        assertCopies(*prog1.memory_, cell1);
        stringstream str2 (
            "0xdeadbeef"
            ""
            "{break}"
        );
        Program prog2 (std::move(str2));
        assert(prog2.isParseError(), == false);
        MemoryCell cell2 (0xdeadbeef);
        cell2.getChild();
        assertCopies(*prog2.memory_, cell2);
    }

    {
        name = "Parse error: unterminated";
        fromCerr.str("");
        stringstream str1 (
            "(0)"
            "{"
            "    break"
        );
        Program prog1 (std::move(str1));
        assertParseError(prog1);
        fromCerr.str("");
        stringstream str2 (
            "{break}"
            "("
            "    0"
        );
        Program prog2 (std::move(str2));
        assertParseError(prog2);
        fromCerr.str("");
        stringstream str3 (
            "{break}"
            "("
            "    \"foo\n"
            ")"
        );
        Program prog3 (std::move(str3));
        assertParseError(prog3);
    }

    {
        name = "Parse error: invalid string";
        fromCerr.str("");
        stringstream str1 (
            "("
            "    \"foo\" &"
            ")"
            "{break}"
        );
        Program prog1 (std::move(str1));
        assertParseError(prog1);
        fromCerr.str("");
        stringstream str2 (
            "("
            "    & \"foo\""
            ")"
            "{break}"
        );
        Program prog2 (std::move(str2));
        assertParseError(prog2);
        fromCerr.str("");
        stringstream str3 (
            "("
            "    \"foo\" & 4"
            ")"
            "{break}"
        );
        Program prog3 (std::move(str3));
        assertParseError(prog3);
        fromCerr.str("");
        stringstream str4 (
            "("
            "    \" \\l \""
            ")"
            "{break}"
        );
        Program prog4 (std::move(str4));
        assertParseError(prog4);
        fromCerr.str("");
        stringstream str5 (
            "("
            "    \"foo\\\""
            ")"
            "{break}"
        );
        Program prog5 (std::move(str5));
        assertParseError(prog5);
        fromCerr.str("");
        stringstream str6 (
            "("
            "    \"\\x8i2\""
            ")"
            "{break}"
        );
        Program prog6 (std::move(str6));
        assertParseError(prog6);
        fromCerr.str("");
        stringstream str7 (
            "("
            "    \"\\x\""
            ")"
            "{break}"
        );
        Program prog7 (std::move(str7));
        assertParseError(prog7);
    }

    endGroup();
}

