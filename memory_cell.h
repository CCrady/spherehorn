// memory_cell.h

#pragma once

#include <utility>
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

    constexpr bool isFull() const { return numChildrenInstantiated == value; }

public:
    MemoryCell(num _value = 0) : value(_value) {}
    // Construct a memory cell whose contents are the string
    MemoryCell(const std::string& str);
    // Copy assignment. Copies children and value, but doesn't change parent or siblings
    MemoryCell& operator =(const MemoryCell& other);
    MemoryCell(const MemoryCell& other) { *this = other; } // copy constructor can simply use =
    // Move assignment. Copies value and moves children, but doesn't change parent or siblings
    MemoryCell& operator =(MemoryCell&& other);
    MemoryCell(MemoryCell&& other) { *this = std::move(other); } // move constructor can simply use =
    ~MemoryCell();
    constexpr num getVal() const { return value; }
    void setVal(num _value);
    void reset();
    MemoryCell* getChild();
    MemoryCell* getPrev();
    MemoryCell* getNext();
    constexpr MemoryCell* getParent() const { return parent; }
    MemoryCell* shiftBack(num n);
    MemoryCell* shiftForward(num n);
    void makeFirst();
    // Construct a new memory cell with the given value and insert it just before/after this cell.
    // Return a pointer to the new cell.
    MemoryCell* insertBefore(num _value = 0);
    MemoryCell* insertAfter(num _value = 0);
    // Insert child as the last child of this memory cell
    void insertChild(MemoryCell* child);
    // Recursively make copies of all of other's children and set them as children of this
    void copyChildren(MemoryCell& other);
    constexpr bool isTop() const { return parent == nullptr; }

private:
    // link the memory cell as this's next/previous sibling
    inline void linkNext(MemoryCell* next);
    inline void linkPrev(MemoryCell* prev);
};

}
