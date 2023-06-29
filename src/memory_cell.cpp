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
        currChild->setVal(static_cast<num>(str.at(i)));
        currChild = currChild->getNext();
    }
}

MemoryCell& MemoryCell::operator =(const MemoryCell& other) {
    // if we're trying to set a cell to itself, we don't need to do anything
    if (this == &other) return *this;

    setVal(other.getVal());
    // if other doesn't have any instantiated children, we're done
    if (other.numChildrenInstantiated == 0) return *this;
    // otherwise we need to make copies of other's children

    numChildrenInstantiated = other.numChildrenInstantiated; // this isn't true yet, but it will be once we're done
    // make a copy of other's first child so that we have a starting point
    firstChild = new MemoryCell(*other.firstChild);
    firstChild->parent = this; // make sure to set the parent

    // iterate forwards over other's children, starting with the child after the first
    MemoryCell* otherCurrChild = nullptr;
    MemoryCell* thisCurrChild = nullptr;
    MemoryCell* thisPrevChild = firstChild;
    for (otherCurrChild = other.firstChild->nextSibling;
         otherCurrChild != nullptr && otherCurrChild != other.firstChild;
         otherCurrChild = otherCurrChild->nextSibling) {
        thisCurrChild = new MemoryCell(*otherCurrChild); // create a copy of otherCurrChild
        thisPrevChild->linkNext(thisCurrChild);
        thisPrevChild = thisCurrChild;
    }
    // if we looped all the way around, attach the end to the start and exit
    if (otherCurrChild == other.firstChild) {
        thisPrevChild->linkNext(firstChild);
        return *this;
    }
    // otherwise iterate backwards over other's children
    MemoryCell* thisNextChild = firstChild;
    for (otherCurrChild = other.firstChild->prevSibling;
         otherCurrChild != nullptr;
         otherCurrChild = otherCurrChild->prevSibling) {
        thisCurrChild = new MemoryCell(*otherCurrChild); // create a copy of otherCurrChild
        thisNextChild->linkPrev(thisCurrChild);
        thisNextChild = thisCurrChild;
    }

    return *this;
}

MemoryCell& MemoryCell::operator =(MemoryCell&& other) {
    // if we're trying to move a cell to itself, we don't need to do anything
    if (this == &other) return *this;

    setVal(other.getVal());
    // if other doesn't have any instantiated children, we're done
    if (other.numChildrenInstantiated == 0) return *this;
    // otherwise we need to move other's children

    numChildrenInstantiated = other.numChildrenInstantiated; // this isn't true yet, but it will be once we're done
    firstChild = other.firstChild;
    firstChild->parent = this;
    // null out other's child pointer to prevent it from deallocating the children when it's destroyed
    other.firstChild = nullptr;
    other.numChildrenInstantiated = 0;

    // iterate forwards over other's children, starting with the child after the first
    MemoryCell* currChild = nullptr;
    for (currChild = firstChild->nextSibling;
         currChild != nullptr && currChild != firstChild;
         currChild = currChild->nextSibling) {
        currChild->parent = this;
    }
    // if we looped all the way around, then we're done
    if (currChild == firstChild) return *this;
    // otherwise iterate backwards over other's children
    for (currChild = firstChild->prevSibling;
         currChild != nullptr;
         currChild = currChild->prevSibling) {
        currChild->parent = this;
    }

    return *this;
}

MemoryCell::~MemoryCell() {
    reset();
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
        firstChild->linkNext(firstChild);
    }
    return firstChild;
}

MemoryCell* MemoryCell::getPrev() {
    // if we already have a previous sibling, we can just return it
    if (prevSibling != nullptr) return prevSibling;
    // otherwise we need to allocate and link a new sibling
    MemoryCell* newSibling = new MemoryCell(0);
    this->linkPrev(newSibling);
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
        newSibling->linkPrev(lastSibling);
    }

    return prevSibling;
}

MemoryCell* MemoryCell::getNext() {
    // see MemoryCell::prev() for an explanation of how this works
    if (nextSibling != nullptr) return nextSibling;

    MemoryCell* newSibling = new MemoryCell(0);
    this->linkNext(newSibling);

    //if (parent == nullptr) return nextSibling; // TODO: decide what to do in this case

    parent->numChildrenInstantiated++;
    if (parent->isFull()) {
        MemoryCell* firstSibling = nullptr;
        for (firstSibling = this;
             firstSibling->prevSibling != nullptr;
             firstSibling = firstSibling->prevSibling) {}
        newSibling->linkNext(firstSibling);
    }

    return nextSibling;
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

void MemoryCell::makeFirst() {
    getParent()->firstChild = this;
}

MemoryCell* MemoryCell::insertBefore(num _value) {
    MemoryCell* newCell = new MemoryCell(_value);
    MemoryCell* prevCell = getPrev();
    prevCell->linkNext(newCell);
    this->linkPrev(newCell);
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

MemoryCell* MemoryCell::deleteBefore() {
    MemoryCell* prevCell = getPrev();
    MemoryCell* nextCell = getNext();
    prevCell->linkNext(nextCell);
    parent->value--;
    parent->numChildrenInstantiated--;
    if (parent->firstChild == this)
        parent->firstChild = prevCell;
    delete this; // TODO: this isn't great practice
    return prevCell;
}

MemoryCell* MemoryCell::deleteAfter() {
    MemoryCell* prevCell = getPrev();
    MemoryCell* nextCell = getNext();
    prevCell->linkNext(nextCell);
    parent->value--;
    parent->numChildrenInstantiated--;
    if (parent->firstChild == this)
        parent->firstChild = nextCell;
    delete this; // TODO: this isn't great practice
    return nextCell;
}

void MemoryCell::insertChild(MemoryCell* newChild) {
    if (value == 0) {
        this->firstChild = newChild;
        newChild->parent = this;
        newChild->linkNext(newChild);
    } else {
        newChild->parent = this;
        MemoryCell* lastChild = firstChild->getPrev();
        firstChild->linkPrev(newChild);
        lastChild->linkNext(newChild);
    }
    value++;
    numChildrenInstantiated++;
}

inline void MemoryCell::linkNext(MemoryCell* next) {
    this->nextSibling = next;
    next->prevSibling = this;
    next->parent = this->parent;
}

inline void MemoryCell::linkPrev(MemoryCell* prev) {
    this->prevSibling = prev;
    prev->nextSibling = this;
    prev->parent = this->parent;
}

