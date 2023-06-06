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
        "a bc !!ASDF " // keyword
        "2 0b101 0o7 0d96 0xdeadbeef " // number
        "( { ) } " // grouping
        "; ? " // terminator
        "'d' '\\n' '\\'' '\"' ' ' " // character
        "\"\" \"hi there!!\" \"f\" " // string
    );
    Tokenizer tokens1 (str1);
    // keywords
    token = tokens1.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "a");
    token = tokens1.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "bc");
    token = tokens1.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "!!ASDF");
    // numbers
    token = tokens1.next();
    assert(token.type, == Token::NUMBER);
    assert(token.str, == "2");
    token = tokens1.next();
    assert(token.type, == Token::NUMBER);
    assert(token.str, == "0b101");
    token = tokens1.next();
    assert(token.type, == Token::NUMBER);
    assert(token.str, == "0o7");
    token = tokens1.next();
    assert(token.type, == Token::NUMBER);
    assert(token.str, == "0d96");
    token = tokens1.next();
    assert(token.type, == Token::NUMBER);
    assert(token.str, == "0xdeadbeef");
    // grouping
    token = tokens1.next();
    assert(token.type, == Token::START_MEMORY);
    assert(token.str, == "(");
    token = tokens1.next();
    assert(token.type, == Token::START_BLOCK);
    assert(token.str, == "{");
    token = tokens1.next();
    assert(token.type, == Token::END_MEMORY);
    assert(token.str, == ")");
    token = tokens1.next();
    assert(token.type, == Token::END_BLOCK);
    assert(token.str, == "}");
    // terminators
    token = tokens1.next();
    assert(token.type, == Token::TERMINATOR_NORMAL);
    assert(token.str, == ";");
    token = tokens1.next();
    assert(token.type, == Token::TERMINATOR_CONDITIONAL);
    assert(token.str, == "?");
    // characters
    token = tokens1.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "'d'");
    token = tokens1.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "'\\n'");
    token = tokens1.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "'\\''");
    token = tokens1.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "'\"'");
    token = tokens1.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "' '");
    // strings
    token = tokens1.next();
    assert(token.type, == Token::STRING);
    assert(token.str, == "\"\"");
    token = tokens1.next();
    assert(token.type, == Token::STRING);
    assert(token.str, == "\"hi there!!\"");
    token = tokens1.next();
    assert(token.type, == Token::STRING);
    assert(token.str, == "\"f\"");
    // EOF
    token = tokens1.next();
    assert(token.type, == Token::END);
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
        "\" efg #\" H # comment 7\n"
        "'#' ;?#comment 8"
    );
    Tokenizer tokens2 (str2);
    assert(tokens2.line(), == 1);
    token = tokens2.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "A");
    assert(tokens2.line(), == 2);
    token = tokens2.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "B");
    assert(tokens2.line(), == 3);
    token = tokens2.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "C");
    assert(tokens2.line(), == 6);
    token = tokens2.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "D");
    assert(tokens2.line(), == 6);
    token = tokens2.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "'\\n'");
    assert(tokens2.line(), == 7);
    token = tokens2.next();
    assert(token.type, == Token::STRING);
    assert(token.str, == "\"\"");
    assert(tokens2.line(), == 8);
    token = tokens2.next();
    assert(token.type, == Token::STRING);
    assert(token.str, == "\" efg #\"");
    assert(tokens2.line(), == 9);
    token = tokens2.next();
    assert(token.type, == Token::KEYWORD);
    assert(token.str, == "H");
    assert(tokens2.line(), == 9);
    token = tokens2.next();
    assert(token.type, == Token::CHAR);
    assert(token.str, == "'#'");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assert(token.type, == Token::TERMINATOR_NORMAL);
    assert(token.str, == ";");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assert(token.type, == Token::TERMINATOR_CONDITIONAL);
    assert(token.str, == "?");
    assert(tokens2.line(), == 10);
    token = tokens2.next();
    assert(token.type, == Token::END);

    endGroup();
}

