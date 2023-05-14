// arguments.h

#pragma once

#include "definitions.h"
#include "program_state.h"

namespace spherehorn {

namespace Arguments {
    class Argument {
    public:
        virtual num get() = 0;
    };

    class Constant : public Argument {
    private:
        num value = 0;
    public:
        Constant(num _value) : value(_value) {}
        num get();
    };

    class Accumulator : public Argument {
    private:
        ProgramState* state;
    public:
        Accumulator(ProgramState* _state) : state(_state) {}
        num get();
    };

    class MemoryCell : public Argument {
    private:
        ProgramState* state;
    public:
        MemoryCell(ProgramState* _state) : state(_state) {}
        num get();
    };
}

}

