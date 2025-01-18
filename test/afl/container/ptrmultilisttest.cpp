/**
  *  \file test/afl/container/ptrmultilisttest.cpp
  *  \brief Test for afl::container::PtrMultiList
  */

#include "afl/container/ptrmultilist.hpp"
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

    bool objCompare(Obj* a, Obj* b)
        {
            return a->n < b->n;
        }
}

/** Test general operation sequence. */
AFL_TEST("afl.container.PtrMultiList", a)
{
    a.checkEqual("live initial", Obj::live, 0);

    {
        // Create something
        afl::container::PtrMultiList<Obj> ml;
        a.check("init empty", ml.empty());
        a.checkEqual("init size", ml.size(), 0U);
        Obj* firstObj = new Obj(1);
        Obj* firstAdded = ml.pushBackNew(firstObj);
        a.checkEqual("one ptr", firstObj, firstAdded);
        a.check("one empty", !ml.empty());
        a.checkEqual("one size", ml.size(), 1U);
        ml.pushBackNew(new Obj(2));
        ml.pushBackNew(new Obj(3));
        a.check("three empty", !ml.empty());
        a.checkEqual("three live", Obj::live, 3);
        a.checkEqual("three size", ml.size(), 3U);

        // Regular iteration
        {
            afl::test::Assert aa(a("regular"));
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            aa.check("end 1", i != ml.end());
            aa.check("begin 1", i == ml.begin());
            aa.checkEqual("value 1", (*i)->n, 1);
            ++i;
            aa.check("end 2", i != ml.end());
            aa.check("begin 2", i != ml.begin());
            aa.checkEqual("value 2", (*i)->n, 2);
            ++i;
            aa.check("end 3", i != ml.end());
            aa.check("begin 3", i != ml.begin());
            aa.checkEqual("value 3", (*i)->n, 3);
            ++i;
            aa.check("end 4", i == ml.end());
            aa.check("begin 4", i != ml.begin());
        }

        // Regular iteration with two iterators
        {
            afl::test::Assert aa(a("parallel"));
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            afl::container::PtrMultiList<Obj>::iterator j = ml.begin();
            aa.check("iter 1", i == j);
            aa.checkEqual("i 1", (*i)->n, 1);
            aa.checkEqual("j i ", (*j)->n, 1);
            ++i;
            aa.check("iter 2", i != j);
            aa.checkEqual("i 2", (*i)->n, 2);
            aa.checkEqual("j 2", (*j)->n, 1);
            ++j;
            aa.check("iter 3", i == j);
            aa.checkEqual("i 3", (*i)->n, 2);
            aa.checkEqual("j 3", (*j)->n, 2);
            aa.checkEqual("live", Obj::live, 3);
        }

        // Deletion of second element with both iterators pointing there
        {
            afl::test::Assert aa(a("delete"));
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            afl::container::PtrMultiList<Obj>::iterator j = ml.begin();
            ++i;
            ++j;
            ml.erase(i);
            aa.check("i 1", *i == 0);
            aa.check("j 1", *j == 0);
            ++i;
            ++j;
            aa.checkEqual("i 2", (*i)->n, 3);
            aa.checkEqual("j 2", (*j)->n, 3);
            aa.check("iter", i == j);
            aa.checkEqual("live", Obj::live, 2);
        }

        // List now has two elements. Try deleting the first.
        {
            afl::test::Assert aa(a("delete-first"));
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            afl::container::PtrMultiList<Obj>::iterator j = ml.begin();
            ml.erase(i);
            aa.check("i 1", *i == 0);
            aa.check("j 1", *j == 0);
            ++i;
            ++j;
            aa.checkEqual("i 2", (*i)->n, 3);
            aa.checkEqual("j 2", (*j)->n, 3);
            aa.checkEqual("live", Obj::live, 1);
        }

        // List now has one element. Must be 3.
        {
            afl::test::Assert aa(a("single"));
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            aa.checkEqual("value", (*i)->n, 3);
            aa.checkEqual("size", ml.size(), 1U);
        }

        // Insert one element to test insert()
        {
            afl::test::Assert aa(a("insert"));
            ml.insertNew(ml.begin(), new Obj(9));
            aa.checkEqual("live", Obj::live, 2);
            aa.checkEqual("size", ml.size(), 2U);
            aa.checkEqual("value", ml.front()->n, 9);
        }
    }

    a.checkEqual("live final", Obj::live, 0);
}

/** Test merge operation. */
AFL_TEST("afl.container.PtrMultiList:merge", a)
{
    afl::container::PtrMultiList<Obj> ml;
    ml.pushBackNew(new Obj(1));
    ml.pushBackNew(new Obj(3));
    ml.pushBackNew(new Obj(5));
    ml.pushBackNew(new Obj(7));
    ml.pushBackNew(new Obj(8));
    ml.pushBackNew(new Obj(9));

    afl::container::PtrMultiList<Obj> ml2;
    ml2.pushBackNew(new Obj(2));
    ml2.pushBackNew(new Obj(4));
    ml2.pushBackNew(new Obj(6));
    ml2.pushBackNew(new Obj(10));
    ml2.pushBackNew(new Obj(11));
    ml2.pushBackNew(new Obj(12));

    a.checkEqual("01. live", Obj::live, 12);
    a.checkEqual("02. size", ml.size(), 6U);
    a.checkEqual("03. size", ml2.size(), 6U);

    // Merge
    ml.merge(ml2, objCompare);
    a.checkEqual("11. live", Obj::live, 12);
    a.checkEqual("12. size", ml.size(), 12U);
    a.checkEqual("13. size", ml2.size(), 0U);

    // Verify
    afl::container::PtrMultiList<Obj>::iterator p = ml.begin();
    for (int i = 1; i <= 12; ++i) {
        a.check("21. iter", p != ml.end());
        a.check("22. ptr", *p != 0);
        a.checkEqual("23. value", (*p)->n, i);
        ++p;
    }
    a.check("24. end", p == ml.end());

    // Merge and verify again (pathological case I)
    ml.merge(ml2, objCompare);
    a.checkEqual("31. live", Obj::live, 12);
    a.checkEqual("32. size", ml.size(), 12U);
    a.checkEqual("33. size", ml2.size(), 0U);

    // Verify
    p = ml.begin();
    for (int i = 1; i <= 12; ++i) {
        a.check("41. iter", p != ml.end());
        a.check("42. ptr", *p != 0);
        a.checkEqual("43. value", (*p)->n, i);
        ++p;
    }
    a.check("44. end", p == ml.end());

    // Merge and verify again (pathological case II)
    ml2.merge(ml, objCompare);
    a.checkEqual("51. live", Obj::live, 12);
    a.checkEqual("52. size", ml2.size(), 12U);
    a.checkEqual("53. size", ml.size(), 0U);

    // Verify
    p = ml2.begin();
    for (int i = 1; i <= 12; ++i) {
        a.check("61. iter", p != ml2.end());
        a.check("62. ptr", *p != 0);
        a.checkEqual("63. value", (*p)->n, i);
        ++p;
    }
    a.check("64. end", p == ml2.end());
}

/** Test adding on both sides. */
AFL_TEST("afl.container.PtrMultiList:pushBackNew:pushFrontNew", a)
{
    afl::container::PtrMultiList<Obj> ml;
    Obj* o3 = ml.pushBackNew(new Obj(3));
    Obj* o2 = ml.pushFrontNew(new Obj(2));
    Obj* o4 = ml.pushBackNew(new Obj(4));
    Obj* o1 = ml.pushFrontNew(new Obj(1));
    Obj* o5 = ml.pushBackNew(new Obj(5));

    a.checkEqual("01. live", Obj::live, 5);
    a.checkEqual("02. size", ml.size(), 5U);

    a.checkEqual("11. value", o1->n, 1);
    a.checkEqual("12. value", o2->n, 2);
    a.checkEqual("13. value", o3->n, 3);
    a.checkEqual("14. value", o4->n, 4);
    a.checkEqual("15. value", o5->n, 5);

    // Verify
    afl::container::PtrMultiList<Obj>::iterator p = ml.begin();
    for (int i = 1; i <= 5; ++i) {
        a.check("21. iter", p != ml.end());
        a.check("22. ptr", *p != 0);
        a.checkEqual("23. value", (*p)->n, i);
        ++p;
    }
    a.check("24. end", p == ml.end());
}

/** Test sort operation. */
AFL_TEST("afl.container.PtrMultiList:sort", a)
{
    afl::container::PtrMultiList<Obj> ml;
    ml.pushBackNew(new Obj(6));
    ml.pushBackNew(new Obj(9));
    ml.pushBackNew(new Obj(1));
    ml.pushBackNew(new Obj(4));
    ml.pushBackNew(new Obj(2));
    ml.pushBackNew(new Obj(5));
    ml.pushBackNew(new Obj(7));
    ml.pushBackNew(new Obj(3));
    ml.pushBackNew(new Obj(8));

    a.checkEqual("01. live", Obj::live, 9);
    a.checkEqual("02. size", ml.size(), 9U);

    // Sort
    ml.sort(objCompare);

    // Verify
    afl::container::PtrMultiList<Obj>::iterator p = ml.begin();
    for (int i = 1; i <= 9; ++i) {
        a.check("11. iter", p != ml.end());
        a.check("12. ptr", *p != 0);
        a.checkEqual("13. value", (*p)->n, i);
        ++p;
    }
    a.check("14. end", p == ml.end());
}

/** Test removal of elements. */
AFL_TEST("afl.container.PtrMultiList:remove:empty", a)
{
    // Remove element from empty list. Must not crash.
    afl::container::PtrMultiList<int> testee;
    testee.popFront();
    testee.popFront();
    testee.popFront();
    a.check("1", testee.extractFront() == 0);
    a.check("2", testee.extractFront() == 0);
    a.check("3", testee.extractFront() == 0);
}

AFL_TEST("afl.container.PtrMultiList:remove:single", a)
{
    // Add single element, point iterator at it, remove.
    afl::container::PtrMultiList<int> testee;
    testee.pushBackNew(new int(42));
    afl::container::PtrMultiList<int>::iterator it = testee.begin();
    a.check("01. iter", *it != 0);
    a.checkEqual("02. value", **it, 42);

    testee.popFront();
    a.check("11. ptr", *it == 0);
    a.check("12. extractFront", testee.extractFront() == 0);
}

AFL_TEST("afl.container.PtrMultiList:remove:long-lived", a)
{
    // Iterator that lives longer than the list
    afl::container::PtrMultiList<int>::iterator it;
    {
        afl::container::PtrMultiList<int> testee;
        testee.pushBackNew(new int(42));
        it = testee.begin();
        a.check("01. iter", *it != 0);
        a.checkEqual("02. value", **it, 42);
    }
    a.check("11. ptr", *it == 0);
}

/** Test insert method. */
AFL_TEST("afl.container.PtrMultiList:insert", a)
{
    afl::container::PtrMultiList<int> testee;
    testee.insertNew(testee.begin(), new int(10));       // [10]
    testee.insertNew(testee.end(), new int(15));         // [10,15]
    testee.insertNew(testee.begin(), new int(5));        // [5,10,15]
    testee.insertNew(++testee.begin(), new int(7));      // [5,7,10,15]
    testee.insertNew(testee.end(), new int(17));         // [5,7,10,15,17]

    // Iterate
    // Default-construct the iterator and assign it, to test that.
    afl::container::PtrMultiList<int>::iterator it;

    it = testee.begin();
    a.check("01. it", it != testee.end());
    a.check("02. ptr", *it != 0);
    a.checkEqual("05. value", **it, 5);

    ++it;
    a.check("11. it", it != testee.end());
    a.check("12. ptr", *it != 0);
    a.checkEqual("13. value", **it, 7);

    ++it;
    a.check("21. it", it != testee.end());
    a.check("22. ptr", *it != 0);
    a.checkEqual("23. value", **it, 10);

    it++;   // post-increment for a change...
    a.check("31. it", it != testee.end());
    a.check("32. ptr", *it != 0);
    a.checkEqual("33. value", **it, 15);

    ++it;
    a.check("41. it", it != testee.end());
    a.check("42. ptr", *it != 0);
    a.checkEqual("43. value", **it, 17);

    ++it;
    a.check("51. end", it == testee.end());
}
