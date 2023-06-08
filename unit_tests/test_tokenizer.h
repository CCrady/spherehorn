// test_tokenizer.h

#include <sstream>
#include "../tokenizer.h"
#include "unit_tests.h"
using namespace spherehorn;

void testTokenizer() {
    startGroup("Testing the tokenizer");

    Token token;

    name = "Basic token categories";
    std::stringstream str1 (
        "as bc **ASDF " // instruction
        ". " // memory setter
        "a m " // variable
        "2 0b101 0o7 0d96 0xdeadbeef " // number
        "( { ) } " // grouping
        "; ? ! " // terminator
        "'d' '\\n' '\\'' '\"' ' ' " // character
        "\"\" \"hi there!!\" \"f\" " // string
    );
    Tokenizer tokens1 (str1);
    // instructions
    token = tokens1.next();
    assertTokenEq(INSTRUCTION, "as");
    token = tokens1.next();
    assertTokenEq(INSTRUCTION, "bc");
    token = tokens1.next();
    assertTokenEq(INSTRUCTION, "**ASDF");
    // memory setter
    token = tokens1.next();
    assertTokenEq(MEMSET, ".");
    // variables
    token = tokens1.next();
    assertTokenEq(VARIABLE, "a");
    token = tokens1.next();
    assertTokenEq(VARIABLE, "m");
    // numbers
    token = tokens1.next();
    assertTokenEq(NUMBER, "2");
    token = tokens1.next();
    assertTokenEq(NUMBER, "0b101");
    token = tokens1.next();
    assertTokenEq(NUMBER, "0o7");
    token = tokens1.next();
    assertTokenEq(NUMBER, "0d96");
    token = tokens1.next();
    assertTokenEq(NUMBER, "0xdeadbeef");
    // grouping
    token = tokens1.next();
    assertTokenEq(GROUPING, "(");
    token = tokens1.next();
    assertTokenEq(GROUPING, "{");
    token = tokens1.next();
    assertTokenEq(GROUPING, ")");
    token = tokens1.next();
    assertTokenEq(GROUPING, "}");
    // terminators
    token = tokens1.next();
    assertTokenEq(TERMINATOR, ";");
    token = tokens1.next();
    assertTokenEq(TERMINATOR, "?");
    token = tokens1.next();
    assertTokenEq(TERMINATOR, "!");
    // characters
    token = tokens1.next();
    assertTokenEq(CHAR, "'d'");
    token = tokens1.next();
    assertTokenEq(CHAR, "'\\n'");
    token = tokens1.next();
    assertTokenEq(CHAR, "'\\''");
    token = tokens1.next();
    assertTokenEq(CHAR, "'\"'");
    token = tokens1.next();
    assertTokenEq(CHAR, "' '");
    // strings
    token = tokens1.next();
    assertTokenEq(STRING, "\"\"");
    token = tokens1.next();
    assertTokenEq(STRING, "\"hi there!!\"");
    token = tokens1.next();
    assertTokenEq(STRING, "\"f\"");
    // EOF
    token = tokens1.next();
    assertTokenEq(END, "");
    assert(tokens1.isEnd(),);

    name = "Whitespace and comments";
    std::stringstream str2 (
        "# comment 1  \n"
        "\tA # comment 2\n"
        "B#comment 3\n"
        " \n"
        "# comment 4\n"
        "  C D   \n"
        "'\\n'#comment 5\n"
        "\"\"# comment 6\n"
        "\" str #\" E # comment 7\n"
        "'#' ;.}(?#comment 8"
    );
    Tokenizer tokens2 (str2);
    assert(tokens2.line(), == 1);
    token = tokens2.next();
    assertTokenEq(INSTRUCTION, "A");
    assert(tokens2.line(), == 2);
    token = tokens2.next();
    assertTokenEq(INSTRUCTION, "B");
    assert(tokens2.line(), == 3);
    token = tokens2.next();
    assertTokenEq(INSTRUCTION, "C");
    assert(tokens2.line(), == 6);
    token = tokens2.next();
    assertTokenEq(INSTRUCTION, "D");
    assert(tokens2.line(), == 6);
    token = tokens2.next();
    assertTokenEq(CHAR, "'\\n'");
    assert(tokens2.line(), == 7);
    token = tokens2.next();
    assertTokenEq(STRING, "\"\"");
    assert(tokens2.line(), == 8);
    token = tokens2.next();
    assertTokenEq(STRING, "\" str #\"");
    assert(tokens2.line(), == 9);
    token = tokens2.next();
    assertTokenEq(INSTRUCTION, "E");
    assert(tokens2.line(), == 9);
    token = tokens2.next();
    assertTokenEq(CHAR, "'#'");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(TERMINATOR, ";");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(MEMSET, ".");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(GROUPING, "}");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(GROUPING, "(");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(TERMINATOR, "?");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(END, "");

    name = ".next() and .peek()";
    std::stringstream str3 (
        "A B ;C;"
    );
    Tokenizer tokens3 (str3);
    token = tokens3.peek();
    assertTokenEq(INSTRUCTION, "A");
    token = tokens3.peek();
    assertTokenEq(INSTRUCTION, "A");
    token = tokens3.next();
    assertTokenEq(INSTRUCTION, "A");
    token = tokens3.peek();
    assertTokenEq(INSTRUCTION, "B");
    token = tokens3.next();
    assertTokenEq(INSTRUCTION, "B");
    token = tokens3.peek();
    assertTokenEq(TERMINATOR, ";");
    token = tokens3.next();
    assertTokenEq(TERMINATOR, ";");
    token = tokens3.peek();
    assertTokenEq(INSTRUCTION, "C");
    token = tokens3.next();
    assertTokenEq(INSTRUCTION, "C");
    token = tokens3.peek();
    assertTokenEq(TERMINATOR, ";");
    token = tokens3.next();
    assertTokenEq(TERMINATOR, ";");
    token = tokens3.peek();
    assertTokenEq(END, "");
    token = tokens3.next();
    assertTokenEq(END, "");
    token = tokens3.next();
    assertTokenEq(END, "");

    endGroup();
}
