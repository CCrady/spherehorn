// tokenizer.cpp

#include <istream>
#include <sstream>
#include <limits>
#include "tokenizer.h"
using namespace spherehorn;


namespace {
    constexpr bool isKeywordTerminator(char ch) {
        return isspace(ch) || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ';' || ch == '?' || ch == '#';
    }
}

// how to determine the token's type:
// first character:
//   case ;, ?, {, }, (, ): special type corresponding to the particular character
//   case digit: number literal
//   case ': char literal
//   case ": string literal
//   default: keyword
// TODO: refactor this to be less spaghetti
Token Tokenizer::next() {
    char ch = 0;
    do {
        ch = getCh();
        if (ch == '#') ignoreComment();
    } while (isspace(ch) || ch == '#');
    // ch is now the first character of the first unprocessed token
    // figure out the type of the token
    Token::TokenType type = Token::END;
    if (isEnd()) {
        type = Token::END;
    } else if (ch == ';') {
        type = Token::TERMINATOR_NORMAL;
    } else if (ch == '?') {
        type = Token::TERMINATOR_CONDITIONAL;
    } else if (ch == '{') {
        type = Token::START_BLOCK;
    } else if (ch == '}') {
        type = Token::END_BLOCK;
    } else if (ch == '(') {
        type = Token::START_MEMORY;
    } else if (ch == ')') {
        type = Token::END_MEMORY;
    } else if (isdigit(ch)) {
        type = Token::NUMBER;
    } else if (ch == '\'') {
        type = Token::CHAR;
    } else if (ch == '\"') {
        type = Token::STRING;
    } else {
        type = Token::KEYWORD;
    }

    str_.str("");
    switch (type) {
        case Token::TERMINATOR_NORMAL:
        case Token::TERMINATOR_CONDITIONAL:
        case Token::START_BLOCK:
        case Token::END_BLOCK:
        case Token::START_MEMORY:
        case Token::END_MEMORY:
            str_.put(ch);
            break;
        case Token::NUMBER:
        case Token::KEYWORD:
            str_.put(ch);
            while (!isKeywordTerminator(input_.peek())) {
                str_.put(getCh());
            }
            break;
        case Token::CHAR:
            str_.put(ch);
            do {
                ch = getCh();
                str_.put(ch);
                // if we come across a backslash, we want to put the following character in even if it's an apostrophe
                if (ch == '\\') {
                    str_.put(getCh());
                }
            } while (ch != '\'');
            break;
        case Token::STRING:
            str_.put(ch);
            do {
                ch = getCh();
                str_.put(ch);
                // if we come across a backslash, we want to put the following character in even if it's a quote mark
                if (ch == '\\') {
                    str_.put(getCh());
                }
            } while (ch != '\"');
            break;
        case Token::END:
            break;
    }

    return Token(type, str_.str());
}

char Tokenizer::getCh() {
    char gotten = input_.get();
    if (gotten == '\n') line_++;
    return gotten;
}

void Tokenizer::ignoreComment() {
    // discard all characters until the next newline
    input_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    line_++;
}

