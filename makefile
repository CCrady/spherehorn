# makefile
CXX := g++

# files and directories
OBJECTS := arguments.o memory_cell.o tokenizer.o program.o instruction_block.o instructions/nullary.o instructions/unary.o instructions/set_memory.o
SRCDIR := src
BUILDDIR := build_objs
TESTDIR := test_objs

# compiler flags
CXXVERSION := -std=c++20
WARNINGS := -Wall -Wextra -Wpedantic -Wcast-qual -Wcast-align=strict -Wctor-dtor-privacy -Winit-self -Wuninitialized -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=4 -Wundef -Winline -Wstack-protector -Wzero-as-null-pointer-constant -Wuseless-cast
BUILDFLAGS := $(CXXVERSION) $(WARNINGS) -o3 -flto
TESTFLAGS := $(CXXVERSION) $(WARNINGS) -g3 -fsanitize=address -fstack-protector-all

# Primary commands:
build: spherehorn ;

test: unit_tests/unit_tests
	./unit_tests/unit_tests

clean:
	rm -r $(BUILDDIR) $(TESTDIR) unit_tests/unit_tests 2> /dev/null || true

.PHONY: build test clean

# Compile all object files
BUILDOBJECTS := $(addprefix $(BUILDDIR)/, $(OBJECTS))
TESTOBJECTS := $(addprefix $(TESTDIR)/, $(OBJECTS))

$(BUILDOBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(BUILDFLAGS) -c $< -o $@

$(TESTOBJECTS): $(TESTDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(TESTFLAGS) -c $< -o $@

# Compile and link the executables

TESTS := $(wildcard unit_tests/*.h)
unit_tests/unit_tests: unit_tests/unit_tests.cpp $(TESTOBJECTS) $(TESTS)
	$(CXX) $(TESTFLAGS) $(TESTOBJECTS) $< -o $@

spherehorn: $(SRCDIR)/main.cpp $(BUILDOBJECTS)
	$(CXX) $(BUILDFLAGS) $(BUILDOBJECTS) $< -o $@

