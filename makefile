# makefile
CXX := g++
CXXFLAGS := -g3 -Wall -Wextra

OBJECTS := arguments.o memory_cell.o tokenizer.o instruction_block.o instructions/nullary.o instructions/unary.o instructions/set_memory.o

# Primary commands:
#build: spherehorn.exe ;

test: unit_tests/unit_tests.exe
	./unit_tests/unit_tests.exe

clean:
	rm $(OBJECTS) $(EXECUTABLES) 2> /dev/null || true

.PHONY: build test clean

# Compile all object files
$(OBJECTS): %.o: %.cpp

# Compile and link the executables

TESTS := $(wildcard unit_tests/*)
unit_tests/unit_tests.exe: $(OBJECTS) $(TESTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) unit_tests/unit_tests.cpp -o $@

