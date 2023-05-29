// instruction.h

/* Class hierarchy:

  Instruction
   /       \
Break     ActiveInstruction
           /             \
      Nullary           Unary

Individual isntruction classes branch off of Nullary and Unary.
*/


#pragma once

#include <iostream>
#include "../definitions.h"
#include "../program_state.h"

namespace spherehorn {

namespace Instructions {
    class Instruction {
    protected:
        bool isConditional = false;
        Instruction* nextInstr = nullptr;
    public:
        Instruction(bool _isConditional) : isConditional(_isConditional) {}
        void setNextInstr(Instruction* _nextInstr) { nextInstr = _nextInstr; } // TODO: use smart pointers
        virtual Instruction* call(ProgramState& state) = 0;
    };

    class Break : public Instruction {
    private:
        Instruction* breakInstr;
    public:
        Break(bool _isConditional) : Instruction(_isConditional) {}
        void setBreakInstr(Instruction* _breakInstr) { breakInstr = _breakInstr; } // TODO: use smart pointers

        Instruction* call(ProgramState& state) {
            if (isConditional && !state.condRegister) return nextInstr;
            return breakInstr;
        }
    };

    // the value returned by a call to .action(), indicating whether the instruction call went
    // okay, terminated the program gracefully, or encountered an error.
    enum Result {
        OKAY,
        EXIT,
        ABORT,
    };

    class ActiveInstruction : public Instruction {
    protected:
        virtual Result action(ProgramState& state) = 0;
    public:
        ActiveInstruction(bool _isConditional) : Instruction(_isConditional) {}
        // Call/run the instruction, modifying the program's state. Return a pointer to the next
        // instruction to be executed.
        Instruction* call(ProgramState& state) {
            if (isConditional && !state.condRegister) return nextInstr;

            Result result = action(state);
            switch (result) {
            case ABORT:
                std::cerr << "Error!" << std::endl; // TODO: make a more informative error message
                [[fallthrough]];
            case EXIT:
                return nullptr;
            default:
                return nextInstr;
            }
        }
    };
}

}

