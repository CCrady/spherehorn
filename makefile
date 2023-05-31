# makefile
CXX := g++
CXXFLAGS := -g3 -Wall -Wextra

OBJECTS := arguments.o program_state.o memory_cell.o instructions/nullary.o instructions/unary.o
EXECUTABLES := unit_tests/unit_tests.exe spherehorn.exe

# Primary commands:
build: spherehorn.exe ;

test: unit_tests/unit_tests.exe
	./unit_tests/unit_tests.exe

clean:
	rm $(OBJECTS) $(EXECUTABLES) 2> /dev/null || true

.PHONY: build test clean

# Compile all object files
$(OBJECTS): %.o: %.cpp

# Compile and link all executables
$(EXECUTABLES): %.exe: %.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $< -o $@

