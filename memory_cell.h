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
    bool isTop();
};

}
