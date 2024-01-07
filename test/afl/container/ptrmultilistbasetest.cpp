/**
  *  \file test/afl/container/ptrmultilistbasetest.cpp
  *  \brief Test for afl::container::PtrMultiListBase
  */

#include "afl/container/ptrmultilistbase.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    void nullDeleter(void* /*p*/)
    { }

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

/** Test basic operations. */
AFL_TEST("afl.container.PtrMultiListBase", as)
{
    // Four elements
    int a = 1, b = 2, c = 3, d = 4;

    // A list with a null deleter. Add our variables.
    afl::container::PtrMultiListBase ml(nullDeleter);
    as.check("init empty", ml.empty());
    as.checkEqual("init size", ml.size(), 0U);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);
    as.check("three empty", !ml.empty());
    as.checkEqual("three size", ml.size(), 3U);

    // Regular iteration
    {
        afl::test::Assert aa(as("regular"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        aa.check("end 1", i != ml.end());
        aa.check("begin 1", i == ml.begin());
        aa.check("value 1", i.get() == &a);
        i.next();
        aa.check("end 2", i != ml.end());
        aa.check("begin 2", i != ml.begin());
        aa.check("value 2", i.get() == &b);
        i.next();
        aa.check("end 3", i != ml.end());
        aa.check("begin 3", i != ml.begin());
        aa.check("value 3", i.get() == &c);
        i.next();
        aa.check("end 4", i == ml.end());
        aa.check("begin 4", i != ml.begin());
    }

    // Regular iteration with two iterators
    {
        afl::test::Assert aa(as("parallel"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        aa.check("iter 1", i == j);
        aa.check("i 1", i.get() == &a);
        aa.check("j 1", j.get() == &a);
        i.next();
        aa.check("iter 2", i != j);
        aa.check("i 2", i.get() == &b);
        aa.check("j 2", j.get() == &a);
        j.next();
        aa.check("iter 3", i == j);
        aa.check("i 3", i.get() == &b);
        aa.check("j 3", j.get() == &b);
    }

    // Now, add one item at the front, and iterate again
    ml.pushFrontNew(&d);
    {
        afl::test::Assert aa(as("add-front"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        aa.check("end 1", i != ml.end());
        aa.check("begin 1", i == ml.begin());
        aa.check("value 1", i.get() == &d);
        i.next();
        aa.check("end 2", i != ml.end());
        aa.check("begin 2", i != ml.begin());
        aa.check("value 2", i.get() == &a);
        i.next();
        aa.check("end 3", i != ml.end());
        aa.check("begin 3", i != ml.begin());
        aa.check("value 3", i.get() == &b);
        i.next();
        aa.check("end 4", i != ml.end());
        aa.check("begin 4", i != ml.begin());
        aa.check("value 4", i.get() == &c);
        i.next();
        aa.check("end 5", i == ml.end());
        aa.check("begin 5", i != ml.begin());
    }

    // Extract
    void* p = ml.extractFront();
    as.check("extractFront d", p == &d);
    p = ml.extractFront();
    as.check("extractFront a", p == &a);
    p = ml.extractFront();
    as.check("extractFront b", p == &b);
    p = ml.extractFront();
    as.check("extractFront c", p == &c);
    as.check("final empty", ml.empty());
}

/** Test erase for afl::container::PtrMultiListBase with multiple iterators. */
AFL_TEST("afl.container.PtrMultiListBase:erase", as)
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Deletion of second element with both iterators pointing there
    {
        afl::test::Assert aa(as("second"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        j.next();
        ml.erase(i);
        aa.check("i 1", i.get() == 0);
        aa.check("j 1", j.get() == 0);
        i.next();
        j.next();
        aa.check("i 2", i.get() == &c);
        aa.check("j 2", j.get() == &c);
        aa.check("iter 2", i == j);
    }

    // List now has two elements. Try deleting the first.
    {
        afl::test::Assert aa(as("first"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        ml.erase(i);
        aa.check("i 1", i.get() == 0);
        aa.check("j 1", j.get() == 0);
        i.next();
        j.next();
        aa.check("i 2", i.get() == &c);
        aa.check("j 2", j.get() == &c);
    }

    // List now has one element. Must be c.
    {
        afl::test::Assert aa(as("final"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        aa.check("", i.get() == &c);
    }
}

/** Test erase for afl::container::PtrMultiListBase with multiple iterators. */
AFL_TEST("afl.container.PtrMultiListBase:erase:multi", as)
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Deletion of second element with one iterator pointing there
    {
        afl::test::Assert aa(as("second"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        ml.erase(i);
        aa.check("i 1", i.get() == 0);
        aa.check("j 1", j.get() == &a);
        i.next();
        j.next();
        aa.check("i 2", i.get() == &c);
        aa.check("j 2", j.get() == &c);
        aa.check("iter 2", i == j);
    }

    // List now has two elements. Delete first with iterator pointing at second.
    {
        afl::test::Assert aa(as("first"));
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        ml.erase(j);
        aa.check("i 1", i.get() == &c);
        aa.check("j 1", j.get() == 0);
        j.next();
        aa.check("i 2", i.get() == &c);
        aa.check("j 2", j.get() == &c);
        aa.check("iter 2", i == j);
    }
}

/** Test erase for afl::container::PtrMultiListBase with multiple iterators. */
AFL_TEST("afl.container.PtrMultiListBase:erase:multi2", as)
{
    int a = 1, b = 2;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // List has two elements. Delete second with iterator pointing at first.
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        ml.erase(i);
        as.check("i 1", i.get() == 0);
        as.check("j 1", j.get() == &a);
        i.next();
        j.next();
        as.check("i 2", i.get() == 0);
        as.check("j 2", j.get() == 0);
        as.check("iter 2", i == j);
    }
}

/** Test multiple iterators of nontrivial lifetime. */
AFL_TEST("afl.container.PtrMultiListBase:iterator-lifetime", as)
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Make three iterators pointing at a, b, c, respectively.
    afl::container::PtrMultiListBase::iterator* pa = new afl::container::PtrMultiListBase::iterator(ml.begin());
    afl::container::PtrMultiListBase::iterator* pb = new afl::container::PtrMultiListBase::iterator(ml.begin());
    pb->next();
    afl::container::PtrMultiListBase::iterator* pc = new afl::container::PtrMultiListBase::iterator(ml.begin());
    pc->next();
    pc->next();

    // Validate
    as.check("a 1", pa->get() == &a);
    as.check("b 1", pb->get() == &b);
    as.check("c 1", pc->get() == &c);

    // Delete in random order
    delete pb;
    as.check("a 2", pa->get() == &a);
    as.check("c 2", pc->get() == &c);

    delete pa;
    as.check("c 3", pc->get() == &c);

    delete pc;
}

/** Test afl::container::PtrMultiListBase::insert, middle of list. */
AFL_TEST("afl.container.PtrMultiListBase:insert:middle", as)
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // Make an iterator pointing at b
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    pa.next();
    as.check("01. value", pa.get() == &b);

    // Insert c
    ml.insert(pa, &c);

    // Validate iterator
    as.check("11. value", pa.get() == &b);
    pa.next();
    as.check("12. end", pa == ml.end());

    // Validate list
    // (This intentionally uses the assignment operator to test that as well.)
    pa = ml.begin();
    as.check("21. iter", pa != ml.end());
    as.check("22. value", pa.get() == &a);
    pa.next();
    as.check("23. iter", pa != ml.end());
    as.check("24. value", pa.get() == &c);
    pa.next();
    as.check("25. iter", pa != ml.end());
    as.check("26. value", pa.get() == &b);
    pa.next();
    as.check("27. iter", pa == ml.end());
}

/** Test afl::container::PtrMultiListBase::insert, beginning of list. */
AFL_TEST("afl.container.PtrMultiListBase:insert:beginning", as)
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // Make an iterator pointing at a
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    as.check("01. value", pa.get() == &a);
    as.check("02. iter", pa == ml.begin());

    // Insert c
    ml.insert(pa, &c);

    // Validate iterator
    as.check("11. iter", pa != ml.begin());
    as.check("12. value", pa.get() == &a);

    // Validate list
    pa = ml.begin();
    as.check("21. iter", pa != ml.end());
    as.check("22. value", pa.get() == &c);
    pa.next();
    as.check("23. iter", pa != ml.end());
    as.check("24. value", pa.get() == &a);
    pa.next();
    as.check("25. iter", pa != ml.end());
    as.check("26. value", pa.get() == &b);
    pa.next();
    as.check("27. iter", pa == ml.end());
}

/** Test afl::container::PtrMultiListBase::insert, end of list. */
AFL_TEST("afl.container.PtrMultiListBase:insert:end", as)
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // Make an iterator pointing after b
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    pa.next();
    pa.next();
    as.check("01. iter", pa == ml.end());

    // Insert c
    ml.insert(pa, &c);

    // Validate iterator
    as.check("11. iter", pa == ml.end());

    // Validate list
    pa = ml.begin();
    as.check("21. iter", pa != ml.end());
    as.check("22. value", pa.get() == &a);
    pa.next();
    as.check("23. iter", pa != ml.end());
    as.check("24. value", pa.get() == &b);
    pa.next();
    as.check("25. iter", pa != ml.end());
    as.check("26. value", pa.get() == &c);
    pa.next();
    as.check("27. iter", pa == ml.end());
}

/** Test afl::container::PtrMultiListBase::insert, at deleted element. */
AFL_TEST("afl.container.PtrMultiListBase:insert:deleted", as)
{
    int a = 1, b = 2, c = 3, d = 4;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Make an iterator pointing at b
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    pa.next();
    as.check("01. value", pa.get() == &b);

    // Make another iterator and erase it
    afl::container::PtrMultiListBase::iterator pb = pa;
    ml.erase(pb);
    as.check("11. value", pb.get() == 0);

    // Validate pa
    as.check("12. value", pa.get() == 0);

    // Insert d
    ml.insert(pa, &d);

    // Validate iterator
    as.check("21. value", pa.get() == 0);
    pa.next();
    as.check("22. iter", pa != ml.end());
    as.check("23. value", pa.get() == &c);
    pa.next();
    as.check("24. iter", pa == ml.end());

    // Validate list
    pa = ml.begin();
    as.check("31. iter", pa != ml.end());
    as.check("32. value", pa.get() == &a);
    pa.next();
    as.check("33. iter", pa != ml.end());
    as.check("34. value", pa.get() == &d);
    pa.next();
    as.check("35. iter", pa != ml.end());
    as.check("36. value", pa.get() == &c);
    pa.next();
    as.check("37. iter", pa == ml.end());
}
