// tokenizer.h

#pragma once

#include <string>
#include <istream>
#include <sstream>

namespace spherehorn {

// Tokens:
// - nothing that's part of a comment
// - any continuous run of non-whitespace, non-special characters that's not a string or character literal
// - any string or character literal
// - any special character that's not part of a string or character literal
// special characters are '{', '}', '(', ')', ';', '?', '!'
// comments are started with a '#' character which isn't part of a string or character literal
struct Token {
    enum TokenType {
        INSTRUCTION,
        MEMSET,
        VARIABLE,
        TERMINATOR,
        GROUPING,
        STRING,
        CHAR,
        NUMBER,
        BOOL,
        END,
    };
    TokenType type;
    std::string str;
    Token(TokenType _type = END, std::string _str = ""): type(_type), str(_str) {}
};

class Tokenizer {
private:
    std::istream& input_;
    std::stringstream str_;
    int line_ = 1;
    Token upcoming_;
    bool isUpcomingCurrent_ = false;
public:
    Tokenizer(std::istream& input) : input_(input) {} // TODO: should this use move semantics?
    Token next();
    Token peek();
    bool isEnd() { return input_.eof(); }
    int line() { return line_; }
private:
    char getCh();
    void ignoreComment();
    void setUpcoming();
};

}

