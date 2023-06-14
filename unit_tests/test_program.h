// test_program.h

#include <sstream>
#include <fstream>
#include <stdexcept>
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;

void testProgram() {
    startGroup("Testing program execution");

    const char* fileName = nullptr;

    name = "Repeat running";
    stringstream str1 ("{break} (0)");
    Program prog1 (str1);
    assert(prog1.run(), == Status::EXIT);
    bool isException = false;
    try {
        prog1.run();
    } catch (runtime_error& e) {
        isException = true;
    }
    assert(isException,);

    name = "Hello World";
    fileName = "examples/hello_world.spherehorn";
    ifstream helloWorldInput (fileName);
    if (helloWorldInput.is_open()) {
        toCin.str("");
        fromCout.str("");
        const char* helloWorldOutput = "Hello, World!\n";
        Program helloWorldProg (helloWorldInput);
        helloWorldInput.close();
        assert(helloWorldProg.isParseError(), == false);
        assert(helloWorldProg.run(), == Status::EXIT);
        assert(fromCout.str(), == helloWorldOutput);
    } else {
        displayNotOpenError();
    }

    name = "Fizzbuzz";
    fileName = "examples/fizzbuzz.spherehorn";
    ifstream fizzbuzzInput (fileName);
    if (fizzbuzzInput.is_open()) {
        toCin.str("3\n4\n15\n");
        fromCout.str("");
        const char* fizzbuzzOutput = "1\n2\nfizz\nbuzz\n5\nfizz\n7\nbuzz\nfizz\n10\n11\nfizzbuzz\n13\n14\nfizz\n";
        Program fizzbuzzProg (fizzbuzzInput);
        fizzbuzzInput.close();
        assert(fizzbuzzProg.isParseError(), == false);
        assert(fizzbuzzProg.run(), == Status::EXIT);
        assert(fromCout.str(), == fizzbuzzOutput);
    } else {
        displayNotOpenError();
    }

    endGroup();
}

