/**
  *  \file u/t_async_operationlist.cpp
  *  \brief Test for afl::async::OperationList
  */

#include "afl/async/operationlist.hpp"

#include "u/t_async.hpp"
#include "afl/async/operation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/controller.hpp"

/** Test all operations with the base class. */
void
TestAsyncOperationList::testBase()
{
    afl::async::OperationList<afl::async::Operation> list;
    afl::async::Operation op1, op2, op3;
    afl::async::Controller ctl;

    // Nothing there yet
    TS_ASSERT(list.extractFront() == 0);
    TS_ASSERT(list.front() == 0);
    TS_ASSERT(list.empty());
    TS_ASSERT(list.extractByController(&ctl) == 0);
    TS_ASSERT(!list.remove(&op1));

    // Move two items through the list
    list.pushBack(&op1);
    list.pushBack(&op2);
    TS_ASSERT(!list.empty());
    TS_ASSERT(list.front() == &op1);
    TS_ASSERT(list.extractFront() == &op1);
    TS_ASSERT(!list.empty());
    TS_ASSERT(list.front() == &op2);
    TS_ASSERT(list.extractFront() == &op2);
    TS_ASSERT(list.empty());
    TS_ASSERT(list.front() == 0);
    TS_ASSERT(list.extractFront() == 0);

    // Now with remove()
    list.pushBack(&op1);
    list.pushBack(&op2);
    TS_ASSERT(!list.empty());
    TS_ASSERT(list.front() == &op1);

    TS_ASSERT(list.remove(&op2));
    TS_ASSERT(!list.remove(&op2));

    TS_ASSERT(!list.empty());
    TS_ASSERT(list.front() == &op1);
    TS_ASSERT(list.extractFront() == &op1);
    TS_ASSERT(list.empty());
    TS_ASSERT(list.front() == 0);
    TS_ASSERT(list.extractFront() == 0);

    // Now with controller
    op1.setController(&ctl);
    op3.setController(&ctl);
    list.pushBack(&op1);
    list.pushBack(&op2);
    list.pushBack(&op3);
    TS_ASSERT(list.extractByController(&ctl) == &op1);
    TS_ASSERT(list.extractByController(&ctl) == &op3);
    TS_ASSERT(list.extractByController(&ctl) == 0);

    TS_ASSERT(!list.empty());
    TS_ASSERT(list.front() == &op2);
    TS_ASSERT(list.extractFront() == &op2);
    TS_ASSERT(list.empty());
    TS_ASSERT(list.front() == 0);
    TS_ASSERT(list.extractFront() == 0);
}

/** Test all operations with a derived class. */
void
TestAsyncOperationList::testDerived()
{
    afl::async::OperationList<afl::async::SendOperation> list;
    afl::async::SendOperation op1;
    afl::async::Controller ctl;

    // Nothing there yet
    TS_ASSERT(list.extractFront() == 0);
    TS_ASSERT(list.front() == 0);
    TS_ASSERT(list.empty());
    TS_ASSERT(list.extractByController(&ctl) == 0);
    TS_ASSERT(!list.remove(&op1));

    // Walk element through the list
    list.pushBack(&op1);
    
    TS_ASSERT(!list.empty());
    TS_ASSERT(list.front() == &op1);
    TS_ASSERT(list.extractFront() == &op1);
    TS_ASSERT(list.empty());
    TS_ASSERT(list.front() == 0);
    TS_ASSERT(list.extractFront() == 0);
}
