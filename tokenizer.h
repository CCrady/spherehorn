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
    constexpr bool isNumericLiteral() const;
    constexpr bool isArgument() const;
    constexpr bool isLiteral() const;
    constexpr bool isInstruction() const;
};

class Tokenizer {
private:
    std::istream& input_;
    std::stringstream str_;
    int line_ = 1;
    Token upcoming_;
    bool isUpcomingCurrent_ = false;
public:
    Tokenizer(std::istream& input) : input_(input) {}
    Tokenizer(std::istream&& input) : input_(input) {}
    Token next();
    Token peek();
    void discard();
    bool isEnd() const { return input_.eof(); }
    constexpr int line() const { return line_; }
private:
    char getCh();
    void ignoreComment();
    void setUpcoming();
};

}

