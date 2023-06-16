// program.cpp

#include <iostream>
#include <string>
#include <istream>
#include <sstream>
#include <charconv>
#include <vector>
#include <memory>
#include <stdexcept>
#include "definitions.h"
#include "program_state.h"
#include "instruction_container.h"
#include "instruction_block.h"
#include "instructions/instructions.h"
#include "tokenizer.h"
#include "program.h"
using namespace spherehorn;
using std::string;


// Each helper function that consumes tokens is responsible for leaving the input token stream in a
// position where the next token is the start of the next syntactic unit.
// The function that discovers a syntax error is responsible for printing a message to cerr and
// setting isParseError_ to true.

Status Program::run() {
    if (isParseError_) throw std::runtime_error("attempted to run a program with a parse error");
    if (hasBeenRun_) throw std::runtime_error("attempted to re-run a program");
    hasBeenRun_ = true;
    Status exit_status = instrs_->run(state_);
    return exit_status == Status::ABORT ? Status::ABORT : Status::EXIT;
}

Program::Program(std::istream& input) : tokens_(input) {
    for (Token token = tokens_.peek();
         !tokens_.isEnd();
         token = tokens_.peek()) {
        if (token.str == "{" && !instrs_) {
            instrs_ = std::move(parseInstructionBlock());
        } else if (token.str == "(" && !memory_) {
            memory_.reset(parseMemory());
        } else if (token.str == "a:") {
            state_.accRegister = parseInitialAccumulator();
        } else if (token.str == "c:") {
            state_.condRegister = parseInitialConditional();
        } else {
            std::cerr << "Parse error: invalid root token `" << token.str << "` "
                         "(line " << tokens_.line() << ")" << std::endl;
            isParseError_ = true;
            tokens_.discard();
        }
    }

    if (memory_) state_.memoryPtr = memory_->getChild();
    Arguments::Argument::setStatePtr(&state_);
}

MemoryCell* Program::parseMemory() {
    // Rules for parsing memory:
    // ensure that we're starting with a '(' token
    // loop:
    //   if we come across a '(' token, recurse (start a new memory cell)
    //   if we come across a ')' token, return the memory cell we built
    //   if we come across any other token, parse it as a literal
    if (tokens_.next().str != "(") throw std::runtime_error("first token of memory block is not '('");

    MemoryCell* thisCell = new MemoryCell();

    for (Token token = tokens_.peek();
         token.str != ")" && token.type != Token::END;
         token = tokens_.peek()) {
        MemoryCell* newCell = nullptr;
        if (token.type == Token::GROUPING && token.str == "(") {
            newCell = parseMemory();
        } else if (token.isLiteral()) {
            newCell = parseLiteral();
        } else {
            std::cerr << "Parse error: invalid memory token `" << token.str << "` "
                         "(line " << tokens_.line() << ")\n"
                         " -> Hint: only literal values may appear in a memory block" << std::endl;
            isParseError_ = true;
            tokens_.discard();
        }
        if (!newCell) continue;
        thisCell->insertChild(newCell);
    }
    tokens_.discard();

    if (tokens_.isEnd()) {
        std::cerr << "Parse error: memory block is not closed\n"
                     " -> Hint: did you forget a ')'?" << std::endl;
        isParseError_ = true;
    }

    if (isParseError_) {
        delete thisCell;
        return nullptr;
    } else {
        return thisCell;
    }
}

inline MemoryCell* Program::parseLiteral() {
    Token token = tokens_.peek();
    switch (token.type) {
    case Token::NUMBER:
        return new MemoryCell(parseNumber());
    case Token::CHAR:
        return new MemoryCell(parseChar());
    case Token::STRING:
        return new MemoryCell(parseString());
    case Token::BOOL:
        return new MemoryCell(parseBool());
    default:
        throw std::runtime_error("attempted to parse non-literal value");
    }
}

inline num Program::parseNumericLiteral() {
    Token token = tokens_.peek();
    switch (token.type) {
    case Token::NUMBER:
        return parseNumber();
    case Token::CHAR:
        return parseChar();
    case Token::BOOL:
        return parseBool();
    default:
        throw std::runtime_error("attempted to parse non-numeric-literal value");
    }
}

instr_ptr Program::parseInstructionBlock() {
    // Rules for parsing an instruction block:
    // ensure that we're starting with a '{' token
    // if the second token is a terminator, consume it and set the condition accordingly
    // loop:
    //   if we come across a '{' token, recurse (start a new instruction block)
    //   if we come across a '}' token, return the block we built
    //   if we come across any other token, assume that it's the start of an instruction
    if (tokens_.next().str != "{") throw std::runtime_error("first token of block is not '{'");

    Condition condition = parseCondition();
    InstructionBlock* thisBlock = new InstructionBlock(condition);

    for (Token token = tokens_.peek();
         token.str != "}" && token.type != Token::END;
         token = tokens_.peek()) {
        instr_ptr newInstr = token.str == "{" ?
                             parseInstructionBlock() :
                             parseInstruction();
        if (!newInstr) continue;
        thisBlock->insertInstr(newInstr);
    }
    tokens_.discard();

    if (tokens_.isEnd()) {
        std::cerr << "Parse error: instruction block is not closed\n"
                     " -> Hint: did you forget a '}'?" << std::endl;
        isParseError_ = true;
    }

    if (isParseError_) {
        delete thisBlock;
        return instr_ptr();
    } else {
        return instr_ptr(thisBlock);
    }
}

inline instr_ptr Program::parseInstruction() {
    Token code = tokens_.next();
    if (!code.isInstruction()) {
        std::cerr << "Parse error: invalid instruction code `" << code.str << "` "
                     "(line " << tokens_.line() << ")" << std::endl;
        if (code.str == "(" || code.isLiteral() || code.isArgument())
            std::cerr << " -> Hint: did you mean to use '.'?" << std::endl;
        isParseError_ = true;
        return instr_ptr();
    }

    if (code.type == Token::MEMSET) return parseMemorySetter(code);
    bool isUnary = tokens_.peek().isArgument();
    if (isUnary) return parseUnaryInstruction(code);
    return parseNullaryInstruction(code);
}

inline instr_ptr Program::parseMemorySetter(const Token& code) {
    if (code.type != Token::MEMSET) throw std::runtime_error("code of memory setter is not of type Token::MEMSET");

    Token value = tokens_.peek();
    if (value.isArgument()) {
        arg_ptr arg = parseArgument();
        Condition condition = parseCondition();
        return instr_ptr(new Instructions::SetMemoryVal(condition, arg));
    } else if (value.type == Token::STRING) {
        MemoryCell cell (parseString());
        Condition condition = parseCondition();
        return instr_ptr(new Instructions::SetMemory(condition, std::move(cell)));
    } else if (value.str == "(") {
        MemoryCell cell (*parseMemory());
        Condition condition = parseCondition();
        return instr_ptr(new Instructions::SetMemory(condition, std::move(cell)));
    } else {
        std::cerr << "Parse error: invalid memory value for `.` instruction "
                     "(line " << tokens_.line() << ")" << std::endl;
        tokens_.discard();
        return instr_ptr();
    }
}

inline instr_ptr Program::parseNullaryInstruction(const Token& code) {
    if (code.type != Token::INSTRUCTION) throw std::runtime_error("code of instruction is not of type Token::INSTRUCTION");

    Condition condition = parseCondition();
    instr_ptr instr;
    if (code.str == "break") {
        instr.reset(new Instructions::Break(condition));
    } else if (code.str == "++") {
        instr.reset(new Instructions::Increment(condition));
    } else if (code.str == "--") {
        instr.reset(new Instructions::Decrement(condition));
    } else if (code.str == "not") {
        instr.reset(new Instructions::Invert(condition));
    } else if (code.str == "chin") {
        instr.reset(new Instructions::InputChar(condition));
    } else if (code.str == "numin") {
        instr.reset(new Instructions::InputNum(condition));
    } else if (code.str == "strin") {
        instr.reset(new Instructions::InputString(condition));
    } else if (code.str == "chout") {
        instr.reset(new Instructions::OutputChar(condition));
    } else if (code.str == "numout") {
        instr.reset(new Instructions::OutputNum(condition));
    } else if (code.str == "strout") {
        instr.reset(new Instructions::OutputString(condition));
    } else if (code.str == "^") {
        instr.reset(new Instructions::MemoryUp(condition));
    } else if (code.str == "v") {
        instr.reset(new Instructions::MemoryDown(condition));
    } else if (code.str == "<") {
        instr.reset(new Instructions::MemoryPrev(condition));
    } else if (code.str == ">") {
        instr.reset(new Instructions::MemoryNext(condition));
    } else if (code.str == "R") {
        instr.reset(new Instructions::MemoryRestart(condition));
    } else if (code.str == "rot") {
        instr.reset(new Instructions::MemoryRotate(condition));
    } else if (code.str == "<+") {
        instr.reset(new Instructions::InsertBefore(condition));
    } else if (code.str == "+>") {
        instr.reset(new Instructions::InsertAfter(condition));
    } else if (code.str == "<-") {
        instr.reset(new Instructions::DeleteBefore(condition));
    } else if (code.str == "->") {
        instr.reset(new Instructions::DeleteAfter(condition));
    } else {
        std::cerr << "Parse error: unrecognized nullary instruction `" << code.str << "` "
                     "(line " << tokens_.line() << ")" << std::endl;
    }
    return instr;
}

inline instr_ptr Program::parseUnaryInstruction(const Token& code) {
    if (code.type != Token::INSTRUCTION) throw std::runtime_error("code of instruction is not of type Token::INSTRUCTION");

    arg_ptr arg = parseArgument();
    if (!arg) return instr_ptr();
    Condition condition = parseCondition();
    instr_ptr instr;
    if (code.str == "A") {
        instr.reset(new Instructions::SetAccumulator(condition, arg));
    } else if (code.str == "C") {
        instr.reset(new Instructions::SetConditional(condition, arg));
    } else if (code.str == "+") {
        instr.reset(new Instructions::Add(condition, arg));
    } else if (code.str == "-") {
        instr.reset(new Instructions::Subtract(condition, arg));
    } else if (code.str == "r-") {
        instr.reset(new Instructions::ReverseSubtract(condition, arg));
    } else if (code.str == "*") {
        instr.reset(new Instructions::Multiply(condition, arg));
    } else if (code.str == "/") {
        instr.reset(new Instructions::Divide(condition, arg));
    } else if (code.str == "r/") {
        instr.reset(new Instructions::ReverseDivide(condition, arg));
    } else if (code.str == "%") {
        instr.reset(new Instructions::Modulo(condition, arg));
    } else if (code.str == "r%") {
        instr.reset(new Instructions::ReverseModulo(condition, arg));
    } else if (code.str == "and") {
        instr.reset(new Instructions::And(condition, arg));
    } else if (code.str == "or") {
        instr.reset(new Instructions::Or(condition, arg));
    } else if (code.str == "xor") {
        instr.reset(new Instructions::Xor(condition, arg));
    } else if (code.str == ">>") {
        instr.reset(new Instructions::Greater(condition, arg));
    } else if (code.str == "=") {
        instr.reset(new Instructions::Equal(condition, arg));
    } else if (code.str == "<<") {
        instr.reset(new Instructions::Less(condition, arg));
    } else if (code.str == ">=") {
        instr.reset(new Instructions::GreaterOrEqual(condition, arg));
    } else if (code.str == "<=") {
        instr.reset(new Instructions::LessOrEqual(condition, arg));
    } else if (code.str == "/=") {
        instr.reset(new Instructions::NotEqual(condition, arg));
    } else if (code.str == "<") {
        instr.reset(new Instructions::MemoryBack(condition, arg));
    } else if (code.str == ">") {
        instr.reset(new Instructions::MemoryForward(condition, arg));
    } else {
        std::cerr << "Parse error: unrecognized unary instruction `" << code.str << "` "
                     "(line " << tokens_.line() << ")" << std::endl;
    }
    return instr;
}

arg_ptr Program::parseArgument() {
    const Token arg = tokens_.peek();
    arg_ptr result;
    if (!arg.isArgument()) {
        std::cerr << "Parse error: invalid argument `" << arg.str << "` "
                     "(line " << tokens_.line() << ")" << std::endl;
    } else if (arg.type == Token::NUMBER) {
        result.reset(new Arguments::Constant(parseNumber()));
    } else if (arg.type == Token::CHAR) {
        result.reset(new Arguments::Constant(parseChar()));
    } else if (arg.type == Token::BOOL) {
        result.reset(new Arguments::Constant(parseBool()));
    } else if (arg.type == Token::VARIABLE && arg.str == "a") {
        result.reset(new Arguments::Accumulator());
        tokens_.discard();
    } else if (arg.type == Token::VARIABLE && arg.str == "m") {
        result.reset(new Arguments::MemoryCell());
        tokens_.discard();
    }
    return result;
}

Condition Program::parseCondition() {
    const Token token = tokens_.peek();
    if (token.type != Token::TERMINATOR)
        return Condition::ALWAYS;
    tokens_.discard();
    if (token.str == ";") {
        return Condition::ALWAYS;
    } else if (token.str == "?") {
        return Condition::WHEN_TRUE;
    } else if (token.str == "!") {
        return Condition::WHEN_FALSE;
    } else {
        throw std::runtime_error("token of type TERMINATOR is not ';', '?', or '!'");
    }
}

inline num Program::parseNumber() {
    const Token token = tokens_.next();
    int base = 10;
    bool skipFirstTwo = false;
    if (token.str.size() > 2) {
        switch (token.str.at(1)) {
        case 'b':
            base = 2;
            skipFirstTwo = true;
            break;
        case 'o':
            base = 8;
            skipFirstTwo = true;
            break;
        case 'd':
            base = 10;
            skipFirstTwo = true;
            break;
        case 'x':
            base = 16;
            skipFirstTwo = true;
            break;
        }
    }
    const char* stringBegin = token.str.data();
    const char* stringEnd = stringBegin + token.str.size();
    if (skipFirstTwo) stringBegin += 2;
    num result = 0;
    // TODO: parse error if the number is ill-formed
    std::from_chars(stringBegin, stringEnd, result, base);
    return result;
}

unsigned char Program::parseChar() {
    const Token token = tokens_.next();
    if (token.type != Token::CHAR) throw std::runtime_error("token is not of type CHAR");
    if (token.str.back() != '\'') {
        isParseError_ = true;
        std::cerr << "Parse error: character literal is not closed (line " << tokens_.line() << ")\n"
                     " -> Hint: did you forget a `\'`?" << std::endl;
        return 0;
    }
    if (token.str.size() < 3) {
        isParseError_ = true;
        std::cerr << "Parse error: character literal is too short (line " << tokens_.line() << ")" << std::endl;
        return 0;
    }

    // first character is an apostrophe
    unsigned char firstCh = token.str.at(1);
    if (firstCh != '\\') return firstCh;
    unsigned char secondCh = token.str.at(2);
    return parseEscape(secondCh);
}

string Program::parseString() {
    const Token token = tokens_.next();
    if (token.type != Token::STRING) throw std::runtime_error("token is not of type STRING");
    if (token.str.back() != '\"') {
        isParseError_ = true;
        std::cerr << "Parse error: string literal is not closed (line " << tokens_.line() << ")\n"
                     " -> Hint: did you forget a `\"`?" << std::endl;
        return "";
    }

    std::stringstream outStr;
    // first and last characters are quotation marks
    for (unsigned int i = 1; i < token.str.size() - 1; i++) {
        if (token.str.at(i) == '\\') {
            i++;
            outStr.put(parseEscape(token.str.at(i)));
        } else {
            outStr.put(token.str.at(i));
        }
    }
    return outStr.str();
}

inline bool Program::parseBool() {
    const Token token = tokens_.next();
    if (token.type != Token::BOOL) throw std::runtime_error("token is not of type BOOL");
    if (token.str != "T" && token.str != "F") throw std::runtime_error("BOOL token is not 'T' or 'F'");
    return token.str == "T";
}

constexpr char Program::parseEscape(char ch) {
    switch (ch) {
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case ';':
            return ';';
        case '?':
            return '?';
        case ' ':
        case 's':
            return ' ';
        case '0':
            return '\0';
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'e':
            return '\u001b';
        case 'f':
            return '\f';
        case 'n':
        case '\n':
            return '\n';
        case 'r':
            return 'r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        // TODO: better solution
        default:
            return ch;
    }
}

inline num Program::parseInitialAccumulator() {
    if (tokens_.next().str != "a:") throw std::runtime_error("first token of initial accumulator is not 'a:'");
    return parseNumericLiteral();
}

inline num Program::parseInitialConditional() {
    if (tokens_.next().str != "c:") throw std::runtime_error("first token of initial accumulator is not 'c:'");
    return parseNumericLiteral();
}

