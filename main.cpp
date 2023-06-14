// main.cpp

#include <fstream>
#include <iostream>
#include "program.h"

const int EX_USAGE = 64;
const int EX_NOINPUT = 66;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " FILE" << std::endl;
        return EX_USAGE;
    }

    std::ifstream input (argv[1]);
    if (!input.is_open()) {
        std::cerr << "File error: file " << argv[1] << " could not be opened" << std::endl;
        return EX_NOINPUT;
    }

    spherehorn::Program program (input);
    input.close(); // we can close the file as soon as we're done parsing it
    if (program.isParseError()) {
        std::cerr << "Program was not run, as there were one or more parse errors." << std::endl;
        return 1;
    }

    program.run();
}

