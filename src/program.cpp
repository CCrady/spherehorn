// program.cpp

#include <iostream>
#include <string>
#include <istream>
#include <sstream>
#include <charconv>
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

Program::Program(std::istream&& input) : tokens_(std::move(input)) {
    bool seenInstructionBlock = false;
    bool seenInitialMemory = false;
    bool seenInitialAccumulator = false;
    bool seenInitialConditional = false;

    for (Token token = tokens_.peek();
         !tokens_.isEnd();
         token = tokens_.peek()) {
        if (token.str == "{") {
            if (seenInstructionBlock) {
                std::cerr << "Parse error: more than one instruction block at top-level scope "
                             "(line " << tokens_.line() << ")" << std::endl;
                isParseError_ = true;
            }
            instrs_ = std::move(parseInstructionBlock());
            seenInstructionBlock = true;
        } else if (token.isMemoryLiteral()) {
            if (seenInitialMemory) {
                std::cerr << "Parse error: more than one memory literal at top-level scope "
                             "(line " << tokens_.line() << ")" << std::endl;
                isParseError_ = true;
            }
            memory_.reset(parseLiteralAsMemory());
            seenInitialMemory = true;
        } else if (token.str == "a:") {
            if (seenInitialAccumulator) {
                std::cerr << "Parse error: more than one initial accumulator value "
                             "(line " << tokens_.line() << ")" << std::endl;
                isParseError_ = true;
            }
            state_.accRegister = parseInitialAccumulator();
            seenInitialAccumulator = true;
        } else if (token.str == "c:") {
            if (seenInitialConditional) {
                std::cerr << "Parse error: more than one initial conditional value "
                             "(line " << tokens_.line() << ")" << std::endl;
                isParseError_ = true;
            }
            state_.condRegister = parseInitialConditional();
            seenInitialConditional = true;
        } else {
            std::cerr << "Parse error: invalid token at top-level scope `" << token.str << "` "
                         "(line " << tokens_.line() << ")" << std::endl;
            isParseError_ = true;
            tokens_.discard();
        }
    }

    if (memory_) state_.memoryPtr = memory_->getChild();
    Arguments::Argument::setStatePtr(&state_);
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
        if (code.isMemoryLiteral() || code.isArgument())
            std::cerr << " -> Hint: did you mean to use '.'?" << std::endl;
        isParseError_ = true;
        return instr_ptr();
    }

    if (code.type == Token::SET_MEMORY) return parseMemorySetter(code);
    bool isUnary = tokens_.peek().isArgument();
    if (isUnary) return parseUnaryInstruction(code);
    return parseNullaryInstruction(code);
}

instr_ptr Program::parseMemorySetter(const Token& code) {
    if (code.type != Token::SET_MEMORY) throw std::runtime_error("code of memory setter is not of type Token::SET_MEMORY");

    Token value = tokens_.peek();
    if (value.isArgument()) {
        arg_ptr arg = parseArgument();
        Condition condition = parseCondition();
        return instr_ptr(new Instructions::SetMemoryVal(condition, arg));
    } else if (value.isMemoryLiteral()) {
        MemoryCell* cell = parseLiteralAsMemory();
        Condition condition = parseCondition();
        instr_ptr result (new Instructions::SetMemory(condition, std::move(*cell)));
        delete cell; // TODO: this is kind of kludgy
        return result;
    } else {
        std::cerr << "Parse error: invalid memory value for `.` instruction "
                     "(line " << tokens_.line() << ")" << std::endl;
        tokens_.discard();
        return instr_ptr();
    }
}

instr_ptr Program::parseNullaryInstruction(const Token& code) {
    if (code.type != Token::KEYWORD) throw std::runtime_error("code of instruction is not of type Token::KEYWORD");

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
        isParseError_ = true;
    }
    return instr;
}

instr_ptr Program::parseUnaryInstruction(const Token& code) {
    if (code.type != Token::KEYWORD) throw std::runtime_error("code of instruction is not of type Token::KEYWORD");

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
    } else if (arg.type == Token::INTEGER) {
        result.reset(new Arguments::Constant(parseIntegerLiteral()));
    } else if (arg.type == Token::CHAR) {
        result.reset(new Arguments::Constant(parseCharLiteral()));
    } else if (arg.type == Token::BOOL) {
        result.reset(new Arguments::Constant(parseBoolLiteral()));
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

MemoryCell* Program::parseLiteralAsMemory() {
    Token token = tokens_.peek();
    if (!token.isMemoryLiteral()) throw std::runtime_error("attempted to parse non-literal value");
    switch (token.type) {
    case Token::INTEGER:
        return new MemoryCell(parseIntegerLiteral());
    case Token::CHAR:
        return new MemoryCell(parseCharLiteral());
    case Token::BOOL:
        return new MemoryCell(parseBoolLiteral());
    case Token::STRING:
        return parseStringLiteral();
    case Token::MEMORY_BLOCK:
        return parseMemoryBlock();
    default:
        throw std::runtime_error("invalid token memory literal token type");
    }
}

MemoryCell* Program::parseMemoryBlock() {
    // Rules for parsing memory:
    // ensure that we're starting with a '(' token
    // loop:
    //   if we come across a ')' token, return the memory cell we built
    //   if we come across any other token, parse it as a literal
    if (tokens_.next().str != "(") throw std::runtime_error("first token of memory block is not '('");

    MemoryCell* result = new MemoryCell();

    for (Token token = tokens_.peek();
         token.str != ")" && token.type != Token::END;
         token = tokens_.peek()) {
        if (token.isMemoryLiteral()) {
            MemoryCell* newCell = parseLiteralAsMemory();
            result->insertChild(newCell);
        } else {
            std::cerr << "Parse error: invalid memory token `" << token.str << "` "
                         "(line " << tokens_.line() << ")\n"
                         " -> Hint: only literal values may appear in a memory block" << std::endl;
            isParseError_ = true;
            tokens_.discard();
        }
    }
    tokens_.discard();

    if (tokens_.isEnd()) {
        std::cerr << "Parse error: memory block is not closed\n"
                     " -> Hint: did you forget a ')'?" << std::endl;
        isParseError_ = true;
    }
    // if we're not going to use thisCell, we may as well free its children now
    if (isParseError_) result->reset();

    return result;
}

num Program::parseLiteralAsNumber() {
    Token token = tokens_.peek();
    switch (token.type) {
    case Token::INTEGER:
        return parseIntegerLiteral();
    case Token::CHAR:
        return parseCharLiteral();
    case Token::BOOL:
        return parseBoolLiteral();
    default:
        throw std::runtime_error("attempted to parse non-numeric-literal value");
    }
}

num Program::parseIntegerLiteral() {
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
    const char* const stringEnd = stringBegin + token.str.size();
    if (skipFirstTwo) stringBegin += 2;
    num value = 0;
    auto result = std::from_chars(stringBegin, stringEnd, value, base);
    if (result.ptr != stringEnd) {
        std::cerr << "Parse error: invalid integer literal `" << token.str << "` "
                     "(line " << tokens_.line() << ")" << std::endl;
        isParseError_ = true;
    }
    return value;
}

num Program::parseCharLiteral() {
    const Token token = tokens_.next();
    if (token.type != Token::CHAR) throw std::runtime_error("token is not of type CHAR");
    if (token.str.back() != '\'') {
        isParseError_ = true;
        std::cerr << "Parse error: character literal `" << token.str << "` is not closed (line " << tokens_.line() << ")\n"
                     " -> Hint: did you forget a `'`?" << std::endl;
        return 0;
    }
    if (token.str.size() < 3) {
        isParseError_ = true;
        std::cerr << "Parse error: character literal `" << token.str << "`is empty (line " << tokens_.line() << ")" << std::endl;
        return 0;
    }

    // first and last characters are apostrophes
    const char* const charBegin = token.str.data() + 1;
    const char* const charEnd = token.str.data() + token.str.size() - 1;
    const char* charPtr = charBegin;
    char result = parseChar(charPtr);

    if (charPtr != charEnd) {
        isParseError_ = true;
        std::cerr << "Parse error: character literal `" << token.str << "` contains more than one character "
                     "(line " << tokens_.line() << ")" << std::endl;
        return 0;
    }

    return static_cast<num>(result);
}

MemoryCell* Program::parseStringLiteral() {
    if (tokens_.peek().type != Token::STRING) throw std::runtime_error("token is not of type STRING (parseStringLiteral)");

    std::stringstream str;
    while (true) {
        const Token token = tokens_.peek();
        if (token.type != Token::STRING) {
            isParseError_ = true;
            std::cerr << "Parse error: expected a string literal, but instead got `" << token.str << "` "
                         "(line " << tokens_.line() << ")\n";
            if (token.isMemoryLiteral()) {
                std::cerr << " -> Hint: `&` can only be used to concatenate string literals" << std::endl;
            } else {
                std::cerr << " -> Hint: did you use an unnecessary `&`?" << std::endl;
            }
            break;
        }
        tokens_.discard();

        parseStringToken(str, token);
        if (tokens_.peek().type == Token::CONCAT) {
            tokens_.discard();
        } else {
            break;
        }
    }

    return new MemoryCell(str.str());
}

inline void Program::parseStringToken(std::stringstream& str, const Token& token) {
    if (token.type != Token::STRING) throw std::runtime_error("token is not of type STRING (parseStringToken)");
    if (token.str.back() != '\"') {
        isParseError_ = true;
        std::cerr << "Parse error: string literal `" << token.str << "` is not closed (line " << tokens_.line() << ")\n"
                     " -> Hint: did you forget a `\"`?" << std::endl;
        return;
    }
    // first and last characters are quotation marks
    const char* const stringBegin = token.str.data() + 1;
    const char* const stringEnd = token.str.data() + token.str.size() - 1;

    for (const char* stringPos = stringBegin; stringPos != stringEnd;) {
        str.put(parseChar(stringPos));
    }
}

char Program::parseChar(const char*& charPtr) {
    char initialChar = *charPtr;
    charPtr++;

    if (initialChar != '\\') return initialChar;
    // else initialChar == '\\'

    char escapeChar = *charPtr;
    charPtr++;

    switch (escapeChar) {
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
        return '\r';
    case 't':
        return '\t';
    case 'v':
        return '\v';
    case 'x': {
        num value = 0;
        const char* const numBegin = charPtr;
        const char* const numEnd = charPtr + 2;
        auto result = std::from_chars(numBegin, numEnd, value, 16);
        if (result.ptr != numEnd) {
            std::cerr << "Parse error: invalid character escape sequence `\\" << escapeChar << *numBegin << *(numBegin + 1) << "` "
                         "(line " << tokens_.line() << ")\n"
                         " -> Hint: ascii escape sequences must be of the form `\\xHH`, where H are hexadecimal digits" << std::endl;
            isParseError_ = true;
        } else {
            // Only increment the character pointer if the escape was valid. This prevents charPtr
            // from going beyond the end of the string if the escape is malformed.
            charPtr += 2;
        }
        return value;
    }
    default:
        std::cerr << "Parse error: invalid character escape sequence `\\" << escapeChar << "` "
                     "(line " << tokens_.line() << ")" << std::endl;
        isParseError_ = true;
        return 0;
    }
}

bool Program::parseBoolLiteral() {
    const Token token = tokens_.next();
    if (token.type != Token::BOOL) throw std::runtime_error("token is not of type BOOL");
    if (token.str != "T" && token.str != "F") throw std::runtime_error("BOOL token is not 'T' or 'F'");
    return token.str == "T";
}

num Program::parseInitialAccumulator() {
    if (tokens_.next().str != "a:") throw std::runtime_error("first token of initial accumulator is not 'a:'");
    return parseLiteralAsNumber();
}

num Program::parseInitialConditional() {
    if (tokens_.next().str != "c:") throw std::runtime_error("first token of initial accumulator is not 'c:'");
    return parseLiteralAsNumber();
}

