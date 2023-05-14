# makefile
CXX := g++
CXXFLAGS := -g3 -Wall -Wextra

OBJECTS := arguments.o program_state.o memory_cell.o instructions/nullary.o instructions/unary.o
EXECUTABLES := unit_tests spherehorn

# Primary commands:
build: spherehorn ;

test: unit_tests
	./unit_tests

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLES)

.PHONY: build test clean

# Compile all object files
$(OBJECTS): %.o: %.cpp

# Compile and link all executables
$(EXECUTABLES): %: %.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $< -o $@

