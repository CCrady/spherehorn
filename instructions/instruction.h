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
            if (!isConditional || state.condRegister) return breakInstr;
            return nextInstr;
        }
    };

    class ActiveInstruction : public Instruction {
    protected:
        virtual void action(ProgramState& state) = 0;
    public:
        ActiveInstruction(bool _isConditional) : Instruction(_isConditional) {}
        Instruction* call(ProgramState& state) {
            if (!isConditional || state.condRegister) action(state);
            return nextInstr;
        }
    };
}

}

