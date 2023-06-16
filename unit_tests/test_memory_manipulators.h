// test_memory_manipulators.h

#include "../program_state.h"
#include "../memory_cell.h"
#include "../instructions/instructions.h"
#include "unit_tests.h"

void testMemoryManipulatorInstructions() {
    startGroup("Testing memory manipulation instructions");

    ProgramState state;
    MemoryCell cell;

    {
        name = "Memory Up";
        cell.setVal(1);
        MemoryCell& childCell1 = *cell.getChild();
        childCell1.setVal(1);
        MemoryCell& grandchildCell1 = *childCell1.getChild();
        resetState(state, grandchildCell1);
        Instructions::MemoryUp memup (Condition::ALWAYS);
        assertOkay(memup);
        assert(state.memoryPtr, == &childCell1);
        assertExit(memup);
    }

    {
        name = "Memory Down";
        resetState(state, cell);
        cell.setVal(1);
        MemoryCell& childCell2 = *cell.getChild();
        Instructions::MemoryDown memdown (Condition::ALWAYS);
        assertOkay(memdown);
        assert(state.memoryPtr, == &childCell2);
    }

    {
        name = "Memory Prev";
        cell.setVal(6);
        MemoryCell& mainChild = *cell.getChild();
        resetState(state, mainChild);
        MemoryCell& prevChild = *mainChild.getPrev();
        Instructions::MemoryPrev memprev (Condition::ALWAYS);
        assertOkay(memprev);
        assert(state.memoryPtr, == &prevChild);

        name = "Memory Next";
        resetState(state, mainChild);
        MemoryCell& nextChild = *mainChild.getNext();
        Instructions::MemoryNext memnext (Condition::ALWAYS);
        assertOkay(memnext);
        assert(state.memoryPtr, == &nextChild);

        name = "Memory Back";
        resetState(state, mainChild);
        Instructions::MemoryBack memback1 (Condition::ALWAYS, createConstArg(1));
        assertOkay(memback1);
        assert(state.memoryPtr, == &prevChild);
        resetState(state, nextChild);
        Instructions::MemoryBack memback2 (Condition::ALWAYS, createConstArg(2));
        assertOkay(memback2);
        assert(state.memoryPtr, == &prevChild);
        resetState(state, prevChild);
        Instructions::MemoryBack memback4 (Condition::ALWAYS, createConstArg(4));
        assertOkay(memback4);
        assert(state.memoryPtr, == &nextChild);
        resetState(state, mainChild);
        Instructions::MemoryBack memback6 (Condition::ALWAYS, createConstArg(6));
        assertOkay(memback6);
        assert(state.memoryPtr, == &mainChild);

        name = "Memory Forward";
        resetState(state, mainChild);
        Instructions::MemoryForward memfwd1 (Condition::ALWAYS, createConstArg(1));
        assertOkay(memfwd1);
        assert(state.memoryPtr, == &nextChild);
        resetState(state, prevChild);
        Instructions::MemoryForward memfwd2 (Condition::ALWAYS, createConstArg(2));
        assertOkay(memfwd2);
        assert(state.memoryPtr, == &nextChild);
        resetState(state, nextChild);
        Instructions::MemoryForward memfwd4 (Condition::ALWAYS, createConstArg(4));
        assertOkay(memfwd4);
        assert(state.memoryPtr, == &prevChild);
        resetState(state, mainChild);
        Instructions::MemoryForward memfwd6 (Condition::ALWAYS, createConstArg(6));
        assertOkay(memfwd6);
        assert(state.memoryPtr, == &mainChild);

        name = "Memory Restart";
        resetState(state, mainChild);
        Instructions::MemoryRestart memrestart (Condition::ALWAYS);
        assertOkay(memrestart);
        assert(state.memoryPtr, == &mainChild);
        resetState(state, nextChild);
        assertOkay(memrestart);
        assert(state.memoryPtr, == &mainChild);
        resetState(state, prevChild);
        assertOkay(memrestart);
        assert(state.memoryPtr, == &mainChild);

        name = "Memory Rotate";
        resetState(state, mainChild);
        Instructions::MemoryRotate memrotate (Condition::ALWAYS);
        assertOkay(memrotate);
        assert(cell.getChild(), == &mainChild);
        resetState(state, nextChild);
        assertOkay(memrotate);
        assert(cell.getChild(), == &nextChild);
        resetState(state, prevChild);
        assertOkay(memrotate);
        assert(cell.getChild(), == &prevChild);
        resetState(state, prevChild);
        assertOkay(memrotate);
        assert(cell.getChild(), == &prevChild);
    }

    {
        name = "Insert Before/After (short loop)";
        Instructions::InsertBefore insbefore (Condition::ALWAYS);
        Instructions::InsertAfter  insafter  (Condition::ALWAYS);
        cell.setVal(1);
        MemoryCell* firstChild = cell.getChild();
        resetState(state, *firstChild);
        firstChild->setVal(5);
        assertOkay(insbefore);
        MemoryCell* prevChild = state.memoryPtr;
        assert(prevChild->getNext(), == firstChild);
        assert(prevChild->getPrev(), == firstChild);
        assert(firstChild->getNext(), == prevChild);
        assert(firstChild->getPrev(), == prevChild);
        assert(prevChild, != firstChild);
        assert(prevChild->getVal(), == 0);
        assert(prevChild->getParent(), == &cell);
        assert(cell.getChild(), == firstChild);
        assert(cell.getVal(), == 2);
        assert(cell.numChildrenInstantiated, == 2);

        resetState(state, *firstChild);
        firstChild->setVal(5);
        assertOkay(insafter);
        MemoryCell* nextChild = state.memoryPtr;
        assert(nextChild->getNext(), == prevChild);
        assert(nextChild->getPrev(), == firstChild);
        assert(firstChild->getNext(), == nextChild);
        assert(prevChild->getPrev(), == nextChild);
        assert(nextChild, != prevChild);
        assert(nextChild, != firstChild);
        assert(nextChild->getVal(), == 0);
        assert(nextChild->getParent(), == &cell);
        assert(cell.getChild(), == firstChild);
        assert(cell.getVal(), == 3);
        assert(cell.numChildrenInstantiated, == 3);

        name = "Insert Before/After (long loop)";
        cell.setVal(10);
        firstChild = cell.getChild();
        resetState(state, *firstChild);
        firstChild->setVal(5);
        assertOkay(insbefore);
        prevChild = state.memoryPtr;
        assert(prevChild->getNext(), == firstChild);
        assert(firstChild->getPrev(), == prevChild);
        assert(prevChild, != firstChild);
        assert(prevChild->getPrev(), != firstChild);
        assert(cell.getVal(), == 11);

        resetState(state, *firstChild);
        firstChild->setVal(5);
        assertOkay(insafter);
        nextChild = state.memoryPtr;
        assert(nextChild->getPrev(), == firstChild);
        assert(firstChild->getNext(), == nextChild);
        assert(nextChild, != firstChild);
        assert(nextChild, != prevChild);
        assert(nextChild->getNext(), != firstChild);
        assert(cell.getVal(), == 12);
    }

    {
        name = "Delete Before/After (long loop)";
        Instructions::DeleteBefore delbefore (Condition::ALWAYS);
        Instructions::DeleteAfter  delafter  (Condition::ALWAYS);
        cell.setVal(10);
        MemoryCell* firstChild = cell.getChild();
        MemoryCell* prevChild1 = firstChild->getPrev();
        MemoryCell* prevChild2 = prevChild1->getPrev();
        resetState(state, *prevChild1);
        assertOkay(delbefore);
        assert(state.memoryPtr, == prevChild2);
        assert(prevChild2->getNext(), == firstChild);
        assert(firstChild->getPrev(), == prevChild2);
        assert(cell.getChild(), == firstChild);
        assert(cell.getVal(), == 9);

        MemoryCell* prevChild3 = prevChild2->getPrev();
        resetState(state, *prevChild2);
        assertOkay(delafter);
        assert(state.memoryPtr, == firstChild);
        assert(prevChild3->getNext(), == firstChild);
        assert(firstChild->getPrev(), == prevChild3);
        assert(cell.getChild(), == firstChild);
        assert(cell.getVal(), == 8);

        MemoryCell* nextChild1 = firstChild->getNext();
        resetState(state, *firstChild);
        assertOkay(delbefore);
        assert(state.memoryPtr, == prevChild3);
        assert(prevChild3->getNext(), == nextChild1);
        assert(nextChild1->getPrev(), == prevChild3);
        assert(cell.getChild(), == prevChild3);
        assert(cell.getVal(), == 7);

        resetState(state, *nextChild1);
        assertOkay(delafter);
        MemoryCell* nextChild2 = state.memoryPtr;
        assert(prevChild3->getNext(), == nextChild2);
        assert(nextChild2->getPrev(), == prevChild3);
        assert(cell.getChild(), == prevChild3);
        assert(cell.getVal(), == 6);

        name = "Delete Before/After (short loop)";
        cell.setVal(3);
        MemoryCell* child0 = cell.getChild();
        MemoryCell* child1 = child0->getNext();
        resetState(state, *child0);
        assertOkay(delbefore);
        MemoryCell* child2 = state.memoryPtr;
        assert(child1->getNext(), == child2);
        assert(child1->getPrev(), == child2);
        assert(child2->getNext(), == child1);
        assert(child2->getPrev(), == child1);
        assert(cell.getChild(), == child2);
        assert(cell.getVal(), == 2);

        resetState(state, *child1);
        assertOkay(delafter);
        assert(state.memoryPtr, == child2);
        assert(child2->getNext(), == child2);
        assert(child2->getPrev(), == child2);
        assert(cell.getChild(), == child2);
        assert(cell.getVal(), == 1);
        assert(cell.numChildrenInstantiated, == 1);

        resetState(state, *child2);
        assertExit(delafter);
        assert(state.memoryPtr, == &cell);
        assert(cell.getVal(), == 0);
        assert(cell.numChildrenInstantiated, == 0);

        cell.setVal(1);
        MemoryCell* delChild = cell.getChild();
        delChild->setVal(1);
        MemoryCell* delGrandchild = delChild->getChild();
        delGrandchild->setVal(1);
        resetState(state, *delGrandchild);
        assertOkay(delbefore);
        assert(state.memoryPtr, == delChild);
        assert(delChild->getVal(), == 0);
        assert(delChild->numChildrenInstantiated, == 0);
    }

    {
        name = "Memory setter";
        resetState(state, cell);
        MemoryCell setCell (4);
        MemoryCell& setCellChild = *setCell.getChild();
        setCellChild.setVal(5);
        Instructions::SetMemory setmem (Condition::ALWAYS, setCell);
        assertOkay(setmem);
        assertCopies(cell, setCell);
        MemoryCell& childCell3 = *cell.getChild();
        resetState(state, childCell3);
        childCell3.getChild()->setVal(2);
        assertOkay(setmem);
        assertCopies(childCell3, setCell);
        assert(childCell3.getParent(), == &cell);
    }

    endGroup();
}

