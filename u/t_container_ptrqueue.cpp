/**
  *  \file u/t_container_ptrqueue.cpp
  *  \brief Test for afl::container::PtrQueue
  */

#include "afl/container/ptrqueue.hpp"

#include "u/t_container.hpp"

namespace {
    class Obj {
        Obj(const Obj&);
        Obj& operator=(const Obj&);
     public:
        int n;
        static int live;
        Obj(int n)
            : n(n) { ++live; }
        ~Obj()
            { --live; }
    };
    int Obj::live = 0;
}

/** Simple test. */
void
TestContainerPtrQueue::testIt()
{
    // Blank vector
    const Obj* nullObj = 0;
    afl::container::PtrQueue<Obj> pq;
    TS_ASSERT(pq.empty());
    TS_ASSERT_EQUALS(pq.size(), 0U);
    TS_ASSERT_EQUALS(Obj::live, 0);
    TS_ASSERT_EQUALS(pq.front(), nullObj);
    TS_ASSERT_EQUALS(pq.back(), nullObj);

    // Add three objects
    pq.pushBackNew(new Obj(1));
    pq.pushBackNew(new Obj(2));
    pq.pushBackNew(new Obj(3));   // 1 2 3
    TS_ASSERT(!pq.empty());
    TS_ASSERT_EQUALS(pq.size(), 3U);
    TS_ASSERT_EQUALS(Obj::live, 3);

    // Verify content
    TS_ASSERT(pq.front() != 0);
    TS_ASSERT_EQUALS(pq.front()->n, 1);
    TS_ASSERT(pq.back() != 0);
    TS_ASSERT_EQUALS(pq.back()->n, 3);

    // Fetch element
    Obj* ele = pq.extractFront();
    TS_ASSERT(ele != 0);
    TS_ASSERT_EQUALS(ele->n, 1);
    TS_ASSERT_EQUALS(Obj::live, 3);
    delete ele;

    // Drop elements
    TS_ASSERT_EQUALS(Obj::live, 2);
    pq.popFront();
    TS_ASSERT_EQUALS(Obj::live, 1);

    pq.clear();
    TS_ASSERT_EQUALS(Obj::live, 0);
}

/** Test behaviour with const members. */
void
TestContainerPtrQueue::testConst()
{
    afl::container::PtrQueue<const int> q;
    q.pushBackNew(new int(99));
    TS_ASSERT_EQUALS(q.size(), 1U);
    TS_ASSERT(q.front() != 0);
    TS_ASSERT_EQUALS(*q.front(), 99);

    const int* p = q.extractFront();
    TS_ASSERT(p);
    TS_ASSERT_EQUALS(*p, 99);
    delete p;

    TS_ASSERT_EQUALS(q.size(), 0U);
    TS_ASSERT(q.front() == 0);
    TS_ASSERT(q.extractFront() == 0);
}

/** Test pushFrontNew/pushBackNew. */
void
TestContainerPtrQueue::testPush()
{
    afl::container::PtrQueue<int> testee;
    testee.pushBackNew(new int(1));        // 1
    testee.pushFrontNew(new int(2));       // 2:1
    testee.pushBackNew(new int(3));        // 2:1:3
    testee.pushFrontNew(new int(4));       // 4:2:1:3
    testee.pushBackNew(new int(5));        // 4:2:1:3:5
    testee.pushBackNew(new int(6));        // 4:2:1:3:5:6
    testee.pushFrontNew(new int(7));       // 7:4:2:1:3:5:6
    testee.pushFrontNew(new int(8));       // 8:7:4:2:1:3:5:6

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 8);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 7);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 4);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 2);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 1);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 3);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 5);
    testee.popFront();

    TS_ASSERT(testee.front() != 0);
    TS_ASSERT_EQUALS(*testee.front(), 6);
    testee.popFront();

    TS_ASSERT(testee.front() == 0);
    TS_ASSERT(testee.empty());
}

