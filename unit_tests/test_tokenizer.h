// test_tokenizer.h

#include <sstream>
#include <stdexcept>
#include "../src/tokenizer.h"
#include "unit_tests.h"
using namespace spherehorn;

void testTokenizer() {
    startGroup("Testing the tokenizer");

    Token token;

    name = "Basic token categories";
    std::stringstream str1 (
        "as bc **ASDF " // keyword
        ". " // memory setter
        "a m " // variable
        "2 0b101 0o7 0d96 0xdeadbeef " // integer
        "T F" // boolean
        "( { ) } " // grouping
        "; ? ! " // terminator
        "'d' '\\n' '\\'' '\"' ' ' " // character
        "\"\" \"hi there!!\" \"f\" " // string
    );
    Tokenizer tokens1 (str1);
    // keywords
    token = tokens1.next();
    assertTokenEq(KEYWORD, "as");
    token = tokens1.next();
    assertTokenEq(KEYWORD, "bc");
    token = tokens1.next();
    assertTokenEq(KEYWORD, "**ASDF");
    // memory setter
    token = tokens1.next();
    assertTokenEq(SET_MEMORY, ".");
    // variables
    token = tokens1.next();
    assertTokenEq(VARIABLE, "a");
    token = tokens1.next();
    assertTokenEq(VARIABLE, "m");
    // integers
    token = tokens1.next();
    assertTokenEq(INTEGER, "2");
    token = tokens1.next();
    assertTokenEq(INTEGER, "0b101");
    token = tokens1.next();
    assertTokenEq(INTEGER, "0o7");
    token = tokens1.next();
    assertTokenEq(INTEGER, "0d96");
    token = tokens1.next();
    assertTokenEq(INTEGER, "0xdeadbeef");
    // booleans
    token = tokens1.next();
    assertTokenEq(BOOL, "T");
    token = tokens1.next();
    assertTokenEq(BOOL, "F");
    // grouping
    token = tokens1.next();
    assertTokenEq(MEMORY_BLOCK, "(");
    token = tokens1.next();
    assertTokenEq(CODE_BLOCK, "{");
    token = tokens1.next();
    assertTokenEq(MEMORY_BLOCK, ")");
    token = tokens1.next();
    assertTokenEq(CODE_BLOCK, "}");
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
    assertTokenEq(KEYWORD, "A");
    assert(tokens2.line(), == 2);
    token = tokens2.next();
    assertTokenEq(KEYWORD, "B");
    assert(tokens2.line(), == 3);
    token = tokens2.next();
    assertTokenEq(KEYWORD, "C");
    assert(tokens2.line(), == 6);
    token = tokens2.next();
    assertTokenEq(KEYWORD, "D");
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
    assertTokenEq(KEYWORD, "E");
    assert(tokens2.line(), == 9);
    token = tokens2.next();
    assertTokenEq(CHAR, "'#'");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(TERMINATOR, ";");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(SET_MEMORY, ".");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(CODE_BLOCK, "}");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assertTokenEq(MEMORY_BLOCK, "(");
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
    assertTokenEq(KEYWORD, "A");
    token = tokens3.peek();
    assertTokenEq(KEYWORD, "A");
    token = tokens3.next();
    assertTokenEq(KEYWORD, "A");
    token = tokens3.peek();
    assertTokenEq(KEYWORD, "B");
    token = tokens3.next();
    assertTokenEq(KEYWORD, "B");
    token = tokens3.peek();
    assertTokenEq(TERMINATOR, ";");
    token = tokens3.next();
    assertTokenEq(TERMINATOR, ";");
    token = tokens3.peek();
    assertTokenEq(KEYWORD, "C");
    token = tokens3.next();
    assertTokenEq(KEYWORD, "C");
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

    name = ".discard()";
    std::stringstream str4 (
        "A B C D"
    );
    Tokenizer tokens4 (str4);
    token = tokens4.peek();
    assertTokenEq(KEYWORD, "A");
    tokens4.discard();
    token = tokens4.peek();
    assertTokenEq(KEYWORD, "B");
    tokens4.discard();
    bool threwException = false;
    try {
        tokens4.discard();
    } catch (std::runtime_error& e) {
        threwException = true;
    }
    assert(threwException,);
    token = tokens4.next();
    assertTokenEq(KEYWORD, "C");
    threwException = false;
    try {
        tokens4.discard();
    } catch (std::runtime_error& e) {
        threwException = true;
    }
    assert(threwException,);

    endGroup();
}

