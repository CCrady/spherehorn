// program.h

#pragma once

#include <memory>
#include <istream>
#include <sstream>
#include <string>
#include "definitions.h"
#include "program_state.h"
#include "instructions/instructions.h"
#include "tokenizer.h"

namespace spherehorn {

using cell_ptr = std::unique_ptr<MemoryCell>;

class Program {
private:
    ProgramState state_;
    cell_ptr memory_;
    instr_ptr instrs_;
    Tokenizer tokens_;
    bool isParseError_ = false;
    bool hasBeenRun_ = false;
public:
    Program(std::istream&& input);
    Status run();
    constexpr bool isParseError() const { return isParseError_; }
private:
    // For instructions:
    instr_ptr parseInstructionBlock();
    instr_ptr parseInstruction();
    instr_ptr parseMemorySetter(const Token& code);
    instr_ptr parseNullaryInstruction(const Token& code);
    instr_ptr parseUnaryInstruction(const Token& code);
    arg_ptr parseArgument();
    Condition parseCondition();
    // For literals:
    MemoryCell* parseLiteralAsMemory();
    MemoryCell* parseMemoryBlock();
    MemoryCell* parseStringLiteral();
    void parseStringToken(std::stringstream& str, const Token& token);
    num parseLiteralAsNumber();
    num parseIntegerLiteral();
    num parseCharLiteral();
    char parseChar(const char*& start);
    bool parseBoolLiteral();
    // For initial program state:
    num parseInitialAccumulator();
    num parseInitialConditional();
};

}

