// memory_cell.cpp

#include <string>
#include "definitions.h"
#include "memory_cell.h"
using namespace spherehorn;
using std::string;


MemoryCell::MemoryCell(const string& str) : value(str.size()) {
    if (str.size() == 0) return;

    MemoryCell* currChild = getChild();
    for (unsigned int i = 0; i < str.size(); i++) {
        currChild->setVal(str.at(i));
        currChild = currChild->getNext();
    }
}

MemoryCell& MemoryCell::operator =(const MemoryCell& other) {
    // if we're trying to set a cell to itself, we don't need to do anything
    if (this == &other) return *this;

    reset();
    value = other.value;
    // if other doesn't have any instantiated children, we're done
    if (other.numChildrenInstantiated == 0) return *this;
    // otherwise we need to make copies of other's children

    numChildrenInstantiated = other.numChildrenInstantiated; // this isn't true yet, but it will be once we're done
    // make a copy of other's first child so that we have a starting point
    firstChild = new MemoryCell(*other.firstChild);

    // iterate forwards over other's children, starting with the child after the first
    MemoryCell* otherCurrChild = nullptr;
    MemoryCell* thisCurrChild = nullptr;
    MemoryCell* thisPrevChild = firstChild;
    for (otherCurrChild = other.firstChild->nextSibling;
         otherCurrChild != nullptr && otherCurrChild != other.firstChild;
         otherCurrChild = otherCurrChild->nextSibling) {
        thisCurrChild = new MemoryCell(*otherCurrChild); // create a copy of otherCurrChild
        thisPrevChild->nextSibling = thisCurrChild; // link thisCurrChild up with thisPrevChild
        thisCurrChild->prevSibling = thisPrevChild;
        thisPrevChild = thisCurrChild;
    }
    // if we looped all the way around, attach the end to the start and exit
    if (otherCurrChild == other.firstChild) {
        thisPrevChild->nextSibling = firstChild;
        firstChild->prevSibling = thisPrevChild;
        return *this;
    }
    // otherwise iterate backwards over other's children
    MemoryCell* thisNextChild = firstChild;
    for (otherCurrChild = other.firstChild->prevSibling;
         otherCurrChild != nullptr;
         otherCurrChild = otherCurrChild->prevSibling) {
        thisCurrChild = new MemoryCell(*otherCurrChild); // create a copy of otherCurrChild
        thisNextChild->prevSibling = thisCurrChild; // link thisCurrChild up with thisNextChild
        thisCurrChild->nextSibling = thisNextChild;
        thisNextChild = thisCurrChild;
    }

    return *this;
}

MemoryCell::~MemoryCell() {
    reset();
}

num MemoryCell::getVal() {
    return value;
}

void MemoryCell::setVal(num _value) {
    reset();
    value = _value;
}

void MemoryCell::reset() {
    // store this for later
    bool wasFull = isFull();
    // once this function is finished, this cell will have no instantiated children
    numChildrenInstantiated = 0;
    // if this cell has no children, then we don't need to do anything
    if (firstChild == nullptr) return;
    // if this cell has a single child, then destroy it and return
    if (value == 1) {
        delete firstChild;
        firstChild = nullptr;
        return;
    }
    // otherwise, destroy all of this node's instantiated children

    // split up the link from the last child to the first child, if it exists
    MemoryCell* lastChild = firstChild->prevSibling;
    if (lastChild != nullptr) {
        lastChild->nextSibling = nullptr;
    }
    // iterate forwards through the loop, starting from the first child
    MemoryCell* next = nullptr;
    for (MemoryCell* curr = firstChild; curr != nullptr; curr = next) {
        next = curr->nextSibling;
        delete curr;
    }
    // we've deleted the first child, so we should null out this cell's firstChild pointer
    firstChild = nullptr;
    // if the loop was full when we started, then we've gone all the way around, so we're done
    if (wasFull) return;
    // otherwise, we need to do the same thing but working backwards from lastChild
    MemoryCell* prev = nullptr;
    for (MemoryCell* curr = lastChild; curr != nullptr; curr = prev) {
        prev = curr->prevSibling;
        delete curr;
    }
    // the node's instantiated children will always be a contiguous segment of the full loop, so
    // now we can be sure that there are no more children to delete
}

MemoryCell* MemoryCell::getChild() {
    // TODO: if this memory cell's value is 0, trying to get its child is an error
    // if we already have a child, we can just return it
    if (firstChild != nullptr) return firstChild;
    // otherwise we need to allocate a new child
    firstChild = new MemoryCell(0);
    firstChild->parent = this;
    numChildrenInstantiated = 1;
    // if we're only going to have a single child, then link that child to itself in a loop
    if (value == 1) {
        firstChild->prevSibling = firstChild;
        firstChild->nextSibling = firstChild;
    }
    return firstChild;
}

MemoryCell* MemoryCell::getPrev() {
    // if we already have a previous sibling, we can just return it
    if (prevSibling != nullptr) return prevSibling;
    // otherwise we need to allocate a new sibling
    MemoryCell* newSibling = new MemoryCell(0);
    newSibling->parent = this->parent;
    this->prevSibling = newSibling;
    newSibling->nextSibling = this;
    // if this cell doesn't have a parent, i.e. it's a top-level cell, then we're done
    // (this shouldn't ever happen, but just in case)
    //if (parent == nullptr) return prevSibling; // TODO: decide what to do in this case
    // otherwise, we have to inform the parent that we've instantiated a new child
    parent->numChildrenInstantiated++;
    // if the parent is now full, we need to connect the two ends of the loop together
    if (parent->isFull()) {
        MemoryCell* lastSibling = nullptr;
        for (lastSibling = this;
             lastSibling->nextSibling != nullptr;
             lastSibling = lastSibling->nextSibling) {}
        // lastSibling is now the last sibling in the instantiated siblings, and newSibling is the
        // first
        newSibling->prevSibling = lastSibling;
        lastSibling->nextSibling = newSibling;
    }

    return prevSibling;
}

MemoryCell* MemoryCell::getNext() {
    // see MemoryCell::prev() for an explanation of how this works
    if (nextSibling != nullptr) return nextSibling;

    MemoryCell* newSibling = new MemoryCell(0);
    newSibling->parent = this->parent;
    this->nextSibling = newSibling;
    newSibling->prevSibling = this;

    //if (parent == nullptr) return nextSibling; // TODO: decide what to do in this case

    parent->numChildrenInstantiated++;
    if (parent->isFull()) {
        MemoryCell* firstSibling = nullptr;
        for (firstSibling = this;
             firstSibling->prevSibling != nullptr;
             firstSibling = firstSibling->prevSibling) {}
        newSibling->nextSibling = firstSibling;
        firstSibling->prevSibling = newSibling;
    }

    return nextSibling;
}

MemoryCell* MemoryCell::getParent() {
    return parent;
}

MemoryCell* MemoryCell::shiftBack(num n) {
    // if n is larger than the size of the loop, we can accomplish the same thing in less than n
    // calls to getPrev()
    num numOps = n % parent->value;
    MemoryCell* curr = this;
    for (num i = 0; i < numOps; i++) {
        curr = curr->getPrev();
    }
    return curr;
}

MemoryCell* MemoryCell::shiftForward(num n) {
    // see MemoryCell::shiftBack(num n) for an explanation of how this works
    num numOps = n % parent->value;
    MemoryCell* curr = this;
    for (num i = 0; i < numOps; i++) {
        curr = curr->getNext();
    }
    return curr;
}

MemoryCell* MemoryCell::insertBefore(num _value) {
    MemoryCell* newCell = new MemoryCell(_value);
    MemoryCell* prevCell = getPrev();
    prevCell->nextSibling = newCell;
    this->prevSibling = newCell;
    newCell->nextSibling = this;
    newCell->prevSibling = prevCell;
    // make sure to update the parent
    parent->value++;
    parent->numChildrenInstantiated++;
    return newCell;
}

MemoryCell* MemoryCell::insertAfter(num _value) {
    MemoryCell* nextCell = getNext();
    MemoryCell* newCell = nextCell->insertBefore(_value);
    return newCell;
}

void MemoryCell::insertChild(MemoryCell* newChild) {
    if (value == 0) {
        this->firstChild = newChild;
        newChild->parent = this;
        newChild->nextSibling = newChild;
        newChild->prevSibling = newChild;
    } else {
        newChild->parent = this;
        MemoryCell* lastChild = firstChild->getPrev();
        lastChild->nextSibling = newChild;
        newChild->prevSibling = lastChild;
        firstChild->prevSibling = newChild;
        newChild->nextSibling = firstChild;
    }
    value++;
    numChildrenInstantiated++;
}

bool MemoryCell::isTop() {
    return parent == nullptr;
}

