#!/usr/bin/env sh

# build.sh
# A shell script to help compile Spherehorn

# Change this line to change which compiler to use:
compiler=g++
# Uncomment this line to use clang instead of g++:
#compiler=clang


#################

if [ -z "$(which $compiler)" ]; then
    echo "ERROR: $compiler is not installed! Please install it and try again."
    exit 1
elif [ ! -d "src" ]; then
    echo "ERROR: No src/ directory! Please run build.sh from the directory it's in."
    exit 1
fi

echo "Compiling with $compiler..."
if $compiler -std=c++20 -O2 -flto \
    src/arguments.cpp \
    src/memory_cell.cpp \
    src/tokenizer.cpp \
    src/program.cpp \
    src/instruction_block.cpp \
    src/instructions/nullary.cpp \
    src/instructions/unary.cpp \
    src/instructions/set_memory.cpp \
    src/main.cpp \
    -o spherehorn
then
    echo "Compilation was successfull! Try running the command:"
    echo "    ./spherehorn examples/hello_world.spherehorn"
    exit 0
else
    echo "Compilation failed! See README.md for help on how to compile Spherehorn, or file"
    echo "an issue at: github.com/CCrady/spherehorn/issues"
    exit 1
fi

