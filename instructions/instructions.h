// instructions.h
// This file is a convenient wrapper for all the instruction header files. Code which needs to use
// the instruction subclasses (e.g. the code for the parser) can simply include this file instead of
// each instruction header file individually.

#include "instruction.h"
#include "nullary.h"
#include "unary.h"
#include "set_memory.h"

