// tokenizer.cpp

#include <string>
#include <istream>
#include <sstream>
#include <limits>
#include <stdexcept>
#include "tokenizer.h"
using namespace spherehorn;


namespace {
    constexpr bool isGrouping(char ch) {
        return ch == '{' || ch == '}' || ch == '(' || ch == ')';
    }

    constexpr bool isInstructionTerminator(char ch) {
        return ch == ';' || ch == '?' || ch == '!';
    }

    constexpr bool isWordTerminator(char ch) {
        return isspace(ch) || isGrouping(ch) || isInstructionTerminator(ch) || ch == '#';
    }

    constexpr bool isStringTerminator(char ch) {
        return ch == '\"' || ch == '\n';
    }

    constexpr bool isCharTerminator(char ch) {
        return ch == '\'' || ch == '\n';
    }

    constexpr bool isVariableCh(char ch) {
        return ch == 'a' || ch == 'm';
    }

    constexpr bool isBoolCh(char ch) {
        return ch == 'T' || ch == 'F';
    }
}

Token Tokenizer::next() {
    setUpcoming();
    isUpcomingCurrent_ = false;
    return upcoming_;
}

Token Tokenizer::peek() {
    setUpcoming();
    return upcoming_;
}

void Tokenizer::discard() {
    if (!isUpcomingCurrent_) throw std::runtime_error("upcoming_ is not current");
    isUpcomingCurrent_ = false;
}

// how to determine the token's type:
// first character:
//   case ;, ?, !, {, }, (, ): special type corresponding to the particular character
//   case digit: number literal
//   case ': char literal
//   case ": string literal
//   case .: memory setter
//   case a, m, and next character is a word terminator: variable
//   case T, F, and next character is a word terminator: bool
//   else: instruction
// TODO: refactor this to be less spaghetti
void Tokenizer::setUpcoming() {
    // if we've already set upcoming_ to the right value, we don't need to do anything
    if (isUpcomingCurrent_) return;

    char ch = 0;
    do {
        ch = getCh();
        if (ch == '#') ignoreComment();
    } while (isspace(ch) || ch == '#');
    // ch is now the first character of the first unprocessed token
    // figure out the type of the token
    if (isEnd()) {
        upcoming_.type = Token::END;
    } else if (ch == '.') {
        upcoming_.type = Token::MEMSET;
    } else if (isInstructionTerminator(ch)) {
        upcoming_.type = Token::TERMINATOR;
    } else if (isGrouping(ch)) {
        upcoming_.type = Token::GROUPING;
    } else if (isdigit(ch)) {
        upcoming_.type = Token::NUMBER;
    } else if (ch == '\'') {
        upcoming_.type = Token::CHAR;
    } else if (ch == '\"') {
        upcoming_.type = Token::STRING;
    } else if (isVariableCh(ch) && isWordTerminator(input_.peek())) {
        upcoming_.type = Token::VARIABLE;
    } else if (isBoolCh(ch) && isWordTerminator(input_.peek())) {
        upcoming_.type = Token::BOOL;
    } else {
        upcoming_.type = Token::INSTRUCTION;
    }

    // based on the token type we figured out, find the token string
    str_.str("");
    switch (upcoming_.type) {
        case Token::MEMSET:
        case Token::TERMINATOR:
        case Token::GROUPING:
        case Token::VARIABLE:
        case Token::BOOL:
            str_.put(ch);
            break;
        case Token::NUMBER:
        case Token::INSTRUCTION:
            str_.put(ch);
            while (!isEnd() && !isWordTerminator(input_.peek())) {
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
            } while (!isCharTerminator(ch) && !isEnd());
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
            } while (!isStringTerminator(ch) && !isEnd());
            break;
        case Token::END:
            break;
    }
    upcoming_.str = str_.str();

    isUpcomingCurrent_ = true; // we just set upcoming, so make sure to store that fact in isUpcomingCurrent_
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

