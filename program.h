// program.h

#pragma once

#include <memory>
#include <istream>
#include <string>
#include "definitions.h"
#include "program_state.h"
#include "instructions/instructions.h"
#include "tokenizer.h"

namespace spherehorn {

// Parse the program in the input buffer into Instructions and MemoryCells. Pass these back to the
// caller by mutating the state and instr parameters.
// The input stream will be read from, and therefore should not be used after the function has
// finished execution.
using cell_ptr = std::unique_ptr<MemoryCell>;
void parse(ProgramState& state, cell_ptr& topCell, instr_ptr& instr, std::istream& input);

class Program {
private:
    ProgramState state_;
    cell_ptr memory_;
    instr_ptr instrs_;
    Tokenizer tokens_;
    bool isParseError_ = false;
public:
    Program(std::istream& input);
    void run();
    constexpr bool isParseError() const { return isParseError_; }
private:
    MemoryCell* parseMemory();
    inline MemoryCell* parseLiteral();
    instr_ptr parseInstructionBlock();
    inline instr_ptr parseInstruction();
    inline instr_ptr parseMemorySetter(const Token& code);
    inline instr_ptr parseNullaryInstruction(const Token& code);
    inline instr_ptr parseUnaryInstruction(const Token& code);
    arg_ptr parseArgument();
    Condition parseCondition();
    inline num parseNumericLiteral();
    inline num parseNumber();
    unsigned char parseChar();
    std::string parseString();
    inline bool parseBool();
    constexpr char parseEscape(char ch);
    inline num parseInitialAccumulator();
    inline num parseInitialConditional();
};

}

