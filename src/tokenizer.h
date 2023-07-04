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
// special characters are '{', '}', '(', ')', ';', '?', '!', '.', '&'
// comments are started with a '#' character which isn't part of a string or character literal
struct Token {
    enum TokenType {
        KEYWORD,
        SET_MEMORY,
        VARIABLE,
        TERMINATOR,
        CODE_BLOCK,
        MEMORY_BLOCK,
        STRING,
        CHAR,
        INTEGER,
        BOOL,
        CONCAT,
        END,
    };
    TokenType type;
    std::string str;
    Token(TokenType _type = END, std::string _str = ""): type(_type), str(_str) {}
    constexpr bool isNumericLiteral() const {
        return type == CHAR || type == INTEGER || type == BOOL;
    }
    constexpr bool isArgument() const {
        return type == VARIABLE || isNumericLiteral();
    }
    constexpr bool isMemoryLiteral() const {
        return type == STRING || (type == MEMORY_BLOCK && str == "(") || isNumericLiteral();
    }
    constexpr bool isInstruction() const {
        return type == KEYWORD || type == SET_MEMORY;
    }
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

