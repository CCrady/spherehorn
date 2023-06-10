// test_memory_cell.h

#pragma once

#include <string>
#include <utility>
#include "unit_tests.h"
using namespace spherehorn;
using namespace std;


void testMemoryCell() {
    startGroup("Testing the MemoryCell class");

    name = "getVal";
    MemoryCell topCell (5);
    assert(topCell.getVal(), == 5);

    name = "getChild";
    MemoryCell* childCell = topCell.getChild();
    assert(childCell->getVal(), == 0);
    assert(topCell.numChildrenInstantiated, == 1);

    name = "setVal";
    childCell->setVal(3);
    assert(childCell->getVal(), == 3);

    name = "getChild";
    MemoryCell* grandchildCell = childCell->getChild();
    assert(grandchildCell->getVal(), == 0);

    name = "getNext";
    MemoryCell* nextCell = childCell->getNext();
    assert(nextCell->getVal(), == 0);
    assert(topCell.numChildrenInstantiated, == 2);

    name = "Looping";
    MemoryCell* cellptr = childCell;
    for (int i = 0; i < 4; i++) {
        cellptr = cellptr->getPrev();
    }
    assert(cellptr, == nextCell);
    assert(topCell.numChildrenInstantiated, == 5);
    cellptr = cellptr->getPrev();
    assert(cellptr, == childCell);
    assert(topCell.numChildrenInstantiated, == 5);
    for (int i = 0; i < 100; i++) {
        cellptr = cellptr->getNext();
    }
    assert(topCell.numChildrenInstantiated, == 5);

    name = "Shift multiple positions";
    assert(childCell->shiftBack(5), == childCell);
    assert(childCell->shiftForward(10), == childCell);
    assert(childCell->shiftBack(6), == childCell->getPrev());
    assert(childCell->shiftBack(4), == childCell->getNext());
    assert(childCell->shiftForward(2), == childCell->getNext()->getNext());

    name = "Insert before/after";
    MemoryCell insertParent (5);
    MemoryCell& firstChild = *insertParent.getChild();
    MemoryCell& prevChild = *firstChild.getPrev();
    MemoryCell& newPrevChild = *firstChild.insertBefore(3);
    assert(insertParent.getVal(), == 6);
    assert(newPrevChild.getVal(), == 3);
    assert(firstChild.getPrev(), == &newPrevChild);
    assert(newPrevChild.getNext(), == &firstChild);
    assert(newPrevChild.getPrev(), == &prevChild);
    assert(prevChild.getNext(), == &newPrevChild);
    MemoryCell& newNextChild = *firstChild.insertAfter();
    assert(firstChild.getNext(), == &newNextChild);
    assert(newNextChild.getPrev(), == &firstChild);

    name = "Constructor from string";
    string fromStr = "ABC";
    MemoryCell cellABC = MemoryCell(fromStr);
    assert(cellABC.getVal(), == 3);
    MemoryCell* childA = cellABC.getChild();
    assert(childA->getVal(), == 'A');
    MemoryCell* childB = childA->getNext();
    assert(childB->getVal(), == 'B');
    MemoryCell* childC = childB->getNext();
    assert(childC->getVal(), == 'C');

    name = "Insert child";
    MemoryCell* insertionParent = new MemoryCell(0);
    MemoryCell* child1 = new MemoryCell(1);
    MemoryCell* child2 = new MemoryCell(2);
    MemoryCell* child3 = new MemoryCell(3);
    insertionParent->insertChild(child1);
    assert(insertionParent->getVal(), == 1);
    assert(insertionParent->getChild(), == child1);
    assert(child1->getParent(), == insertionParent);
    assert(child1->getNext(), == child1);
    assert(child1->getPrev(), == child1);
    insertionParent->insertChild(child2);
    assert(insertionParent->getVal(), == 2);
    assert(insertionParent->getChild(), == child1);
    assert(child2->getParent(), == insertionParent);
    assert(child1->getNext(), == child2);
    assert(child1->getPrev(), == child2);
    assert(child2->getNext(), == child1);
    assert(child2->getPrev(), == child1);
    insertionParent->insertChild(child3);
    assert(insertionParent->getVal(), == 3);
    assert(child1->getPrev(), == child3);
    assert(child3->getNext(), == child1);
    assert(child3->getPrev(), == child2);
    assert(child2->getNext(), == child3);
    delete insertionParent;

    // setup for copy constructor/assignment tests
    MemoryCell fromParent (2);
    MemoryCell& fromChild1 = *fromParent.getChild();
    fromChild1.setVal(3);
    [[maybe_unused]] MemoryCell& fromChild2 = *fromChild1.getNext();
    MemoryCell& fromGrandchild1 = *fromChild1.getChild();
    fromGrandchild1.setVal(4);
    MemoryCell& fromGrandchild2 = *fromGrandchild1.getNext();
    fromGrandchild2.setVal(5);
    MemoryCell& fromGreatGrandchild1 = *fromGrandchild1.getChild();
    fromGreatGrandchild1.setVal(6);

    name = "Copy constructor (incomplete loop)";
    MemoryCell toChildA1 (fromChild1);
    assert(toChildA1.parent, == nullptr);
    assert(toChildA1.prevSibling, == nullptr);
    assert(toChildA1.nextSibling, == nullptr);
    assertCopies(fromChild1, toChildA1);

    name = "Copy constructor (complete loop)";
    [[maybe_unused]] MemoryCell& fromGrandchild3 = *fromGrandchild1.getPrev();
    MemoryCell toChildB1 (fromChild1);
    assertCopies(fromChild1, toChildB1);

    name = "Copy constructor (independence)";
    MemoryCell& toGrandchildA1 = *toChildA1.getChild();
    MemoryCell& toGrandchildA2 = *toGrandchildA1.getNext();
    MemoryCell& toGrandchildB1 = *toChildB1.getChild();
    MemoryCell& toGrandchildB2 = *toGrandchildB1.getNext();
    fromGrandchild2.setVal(7);
    assert(toGrandchildA2.getVal(), == 5);
    assert(toGrandchildB2.getVal(), == 5);
    toGrandchildA1.setVal(8);
    assert(fromGrandchild1.getVal(), == 4);
    toChildA1.setVal(9);
    assert(fromChild1.getVal(), == 3);
    MemoryCell& toGreatGrandchildB1 = *toGrandchildB1.getChild();
    toGreatGrandchildB1.setVal(1);
    assert(fromGreatGrandchild1.getVal(), == 6);

    name = "Copy assignment (overwriting)";
    toGrandchildB2 = fromChild1;
    assertCopies(fromChild1, toGrandchildB2);

    name = "Copy assignment (from self)";
    toGrandchildB2 = toGrandchildB2;
    assertCopies(fromChild1, toGrandchildB2);

    name = "Move constructor";
    MemoryCell moveChild1 (std::move(toChildB1));
    assert(moveChild1.parent, == nullptr);
    assert(moveChild1.prevSibling, == nullptr);
    assert(moveChild1.nextSibling, == nullptr);
    assert(moveChild1.getChild(), == &toGrandchildB1);
    assert(toGrandchildB1.getParent(), == &moveChild1);
    assert(toGrandchildB2.getParent(), == &moveChild1);

    endGroup();
}

