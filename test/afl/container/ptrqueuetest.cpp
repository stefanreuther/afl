/**
  *  \file test/afl/container/ptrqueuetest.cpp
  *  \brief Test for afl::container::PtrQueue
  */

#include "afl/container/ptrqueue.hpp"
#include "afl/test/testrunner.hpp"

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
AFL_TEST("afl.container.PtrQueue", a)
{
    // Blank vector
    const Obj* nullObj = 0;
    afl::container::PtrQueue<Obj> pq;
    a.check("01. empty", pq.empty());
    a.checkEqual("02. size", pq.size(), 0U);
    a.checkEqual("03. live", Obj::live, 0);
    a.checkEqual("04. front", pq.front(), nullObj);
    a.checkEqual("05. back", pq.back(), nullObj);

    // Add three objects
    pq.pushBackNew(new Obj(1));
    pq.pushBackNew(new Obj(2));
    pq.pushBackNew(new Obj(3));   // 1 2 3
    a.check("11. empty", !pq.empty());
    a.checkEqual("12. size", pq.size(), 3U);
    a.checkEqual("13. live", Obj::live, 3);

    // Verify content
    a.check("21. front", pq.front() != 0);
    a.checkEqual("22. value", pq.front()->n, 1);
    a.check("23. back", pq.back() != 0);
    a.checkEqual("24. value", pq.back()->n, 3);

    // Fetch element
    Obj* ele = pq.extractFront();
    a.check("31. extractFront", ele != 0);
    a.checkEqual("32. value", ele->n, 1);
    a.checkEqual("33. live", Obj::live, 3);
    delete ele;

    // Drop elements
    a.checkEqual("41. live", Obj::live, 2);
    pq.popFront();
    a.checkEqual("42. live", Obj::live, 1);

    pq.clear();
    a.checkEqual("43. live", Obj::live, 0);
}

/** Test behaviour with const members. */
AFL_TEST("afl.container.PtrQueue:const", a)
{
    afl::container::PtrQueue<const int> q;
    q.pushBackNew(new int(99));
    a.checkEqual("01. size", q.size(), 1U);
    a.check("02. front", q.front() != 0);
    a.checkEqual("03. front", *q.front(), 99);

    const int* p = q.extractFront();
    a.check("11. extractFront", p);
    a.checkEqual("12. value", *p, 99);
    delete p;

    a.checkEqual("21. size", q.size(), 0U);
    a.check("22. front", q.front() == 0);
    a.check("23. extractFront", q.extractFront() == 0);
}

/** Test pushFrontNew/pushBackNew. */
AFL_TEST("afl.container.PtrQueue:push", a)
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

    a.check("01. front", testee.front() != 0);
    a.checkEqual("02. value", *testee.front(), 8);
    testee.popFront();

    a.check("11. front", testee.front() != 0);
    a.checkEqual("12. value", *testee.front(), 7);
    testee.popFront();

    a.check("21. front", testee.front() != 0);
    a.checkEqual("22. value", *testee.front(), 4);
    testee.popFront();

    a.check("31. front", testee.front() != 0);
    a.checkEqual("32. value", *testee.front(), 2);
    testee.popFront();

    a.check("41. front", testee.front() != 0);
    a.checkEqual("42. value", *testee.front(), 1);
    testee.popFront();

    a.check("51. front", testee.front() != 0);
    a.checkEqual("52. value", *testee.front(), 3);
    testee.popFront();

    a.check("61. front", testee.front() != 0);
    a.checkEqual("62. value", *testee.front(), 5);
    testee.popFront();

    a.check("71. front", testee.front() != 0);
    a.checkEqual("72. value", *testee.front(), 6);
    testee.popFront();

    a.check("81. front", testee.front() == 0);
    a.check("82. empty", testee.empty());
}
