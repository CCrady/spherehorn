// memory_cell.h

#pragma once

#include <string>
#include "definitions.h"

namespace spherehorn {

class MemoryCell {
private:
    num value = 0;
    num numChildrenInstantiated = 0;
    MemoryCell* firstChild = nullptr;
    MemoryCell* prevSibling = nullptr;
    MemoryCell* nextSibling = nullptr;
    MemoryCell* parent = nullptr;

    bool isFull() { return numChildrenInstantiated == value; }

public:
    MemoryCell(num _value = 0) : value(_value) {}
    // Construct a memory cell whose contents are the string
    MemoryCell(const std::string& str);
    // Copy assignment. Copies children and value, but doesn't change parent or siblings
    MemoryCell& operator =(const MemoryCell& other);
    MemoryCell(const MemoryCell& other) { *this = other; } // copy constructor can simply use =
    ~MemoryCell();
    num getVal();
    void setVal(num _value);
    void reset();
    MemoryCell* getChild();
    MemoryCell* getPrev();
    MemoryCell* getNext();
    MemoryCell* getParent();
    MemoryCell* shiftBack(num n);
    MemoryCell* shiftForward(num n);
    // Construct a new memory cell with the given value and insert it just before/after this cell.
    // Return a pointer to the new cell.
    MemoryCell* insertBefore(num _value = 0);
    MemoryCell* insertAfter(num _value = 0);
    // Insert child as the last child of this memory cell
    void insertChild(MemoryCell* child);
    // Recursively make copies of all of other's children and set them as children of this
    void copyChildren(MemoryCell& other);
    bool isTop();
};

}
