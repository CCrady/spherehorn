// memory_cell.h

#pragma once

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
    MemoryCell(num _value) : value(_value) {}
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
    bool isTop();
};

}
