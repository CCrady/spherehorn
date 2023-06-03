// arguments.h

#pragma once

#include "definitions.h"
#include "program_state.h"

namespace spherehorn {

namespace Arguments {
    class Argument {
    protected:
        static ProgramState* state;
    public:
        virtual num get() = 0;
        static void setStatePtr(ProgramState* _state) { state = _state; }
    };

    class Constant : public Argument {
    private:
        num value = 0;
    public:
        Constant(num _value) : value(_value) {}
        num get();
    };

    class Accumulator : public Argument {
    public:
        Accumulator() {}
        num get();
    };

    class MemoryCell : public Argument {
    public:
        MemoryCell() {}
        num get();
    };
}

}

