/**
  *  \file u/t_container_ptrmultilistbase.cpp
  *  \brief Test for afl::container::PtrMultiListBase
  */

#include "afl/container/ptrmultilistbase.hpp"

#include "u/t_container.hpp"

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

/** Test basic operations for afl::container::PtrMultiListBase. */
void
TestContainerPtrMultiListBase::testBasic()
{
    // Four elements
    int a = 1, b = 2, c = 3, d = 4;

    // A list with a null deleter. Add our variables.
    afl::container::PtrMultiListBase ml(nullDeleter);
    TS_ASSERT(ml.empty());
    TS_ASSERT_EQUALS(ml.size(), 0U);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);
    TS_ASSERT(!ml.empty());
    TS_ASSERT_EQUALS(ml.size(), 3U);

    // Regular iteration
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i == ml.begin());
        TS_ASSERT(i.get() == &a);
        i.next();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i != ml.begin());
        TS_ASSERT(i.get() == &b);
        i.next();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i != ml.begin());
        TS_ASSERT(i.get() == &c);
        i.next();
        TS_ASSERT(i == ml.end());
        TS_ASSERT(i != ml.begin());
    }

    // Regular iteration with two iterators
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        TS_ASSERT(i == j);
        TS_ASSERT(i.get() == &a);
        TS_ASSERT(j.get() == &a);
        i.next();
        TS_ASSERT(i != j);
        TS_ASSERT(i.get() == &b);
        TS_ASSERT(j.get() == &a);
        j.next();
        TS_ASSERT(i == j);
        TS_ASSERT(i.get() == &b);
        TS_ASSERT(j.get() == &b);
    }

    // Now, add one item at the front, and iterate again
    ml.pushFrontNew(&d);
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i == ml.begin());
        TS_ASSERT(i.get() == &d);
        i.next();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i != ml.begin());
        TS_ASSERT(i.get() == &a);
        i.next();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i != ml.begin());
        TS_ASSERT(i.get() == &b);
        i.next();
        TS_ASSERT(i != ml.end());
        TS_ASSERT(i != ml.begin());
        TS_ASSERT(i.get() == &c);
        i.next();
        TS_ASSERT(i == ml.end());
        TS_ASSERT(i != ml.begin());
    }

    // Extract
    void* p = ml.extractFront();
    TS_ASSERT(p == &d);
    p = ml.extractFront();
    TS_ASSERT(p == &a);
    p = ml.extractFront();
    TS_ASSERT(p == &b);
    p = ml.extractFront();
    TS_ASSERT(p == &c);
    TS_ASSERT(ml.empty());
}

/** Test erase for afl::container::PtrMultiListBase with multiple iterators. */
void
TestContainerPtrMultiListBase::testBasicErase1()
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Deletion of second element with both iterators pointing there
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        j.next();
        ml.erase(i);
        TS_ASSERT(i.get() == 0);
        TS_ASSERT(j.get() == 0);
        i.next();
        j.next();
        TS_ASSERT(i.get() == &c);
        TS_ASSERT(j.get() == &c);
        TS_ASSERT(i == j);
    }

    // List now has two elements. Try deleting the first.
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        ml.erase(i);
        TS_ASSERT(i.get() == 0);
        TS_ASSERT(j.get() == 0);
        i.next();
        j.next();
        TS_ASSERT(i.get() == &c);
        TS_ASSERT(j.get() == &c);
    }

    // List now has one element. Must be c.
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        TS_ASSERT(i.get() == &c);
    }
}

/** Test erase for afl::container::PtrMultiListBase with multiple iterators. */
void
TestContainerPtrMultiListBase::testBasicErase2()
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Deletion of second element with one iterator pointing there
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        ml.erase(i);
        TS_ASSERT(i.get() == 0);
        TS_ASSERT(j.get() == &a);
        i.next();
        j.next();
        TS_ASSERT(i.get() == &c);
        TS_ASSERT(j.get() == &c);
        TS_ASSERT(i == j);
    }

    // List now has two elements. Delete first with iterator pointing at second.
    {
        afl::container::PtrMultiListBase::iterator i = ml.begin();
        afl::container::PtrMultiListBase::iterator j = ml.begin();
        i.next();
        ml.erase(j);
        TS_ASSERT(i.get() == &c);
        TS_ASSERT(j.get() == 0);
        j.next();
        TS_ASSERT(i.get() == &c);
        TS_ASSERT(j.get() == &c);
        TS_ASSERT(i == j);
    }
}

/** Test erase for afl::container::PtrMultiListBase with multiple iterators. */
void
TestContainerPtrMultiListBase::testBasicErase3()
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
        TS_ASSERT(i.get() == 0);
        TS_ASSERT(j.get() == &a);
        i.next();
        j.next();
        TS_ASSERT(i.get() == 0);
        TS_ASSERT(j.get() == 0);
        TS_ASSERT(i == j);
    }
}

/** Test multiple iterators of nontrivial lifetime. */
void
TestContainerPtrMultiListBase::testBasicMultiIter()
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
    TS_ASSERT(pa->get() == &a);
    TS_ASSERT(pb->get() == &b);
    TS_ASSERT(pc->get() == &c);

    // Delete in random order
    delete pb;
    TS_ASSERT(pa->get() == &a);
    TS_ASSERT(pc->get() == &c);

    delete pa;
    TS_ASSERT(pc->get() == &c);

    delete pc;
}

/** Test afl::container::PtrMultiListBase::insert, middle of list. */
void
TestContainerPtrMultiListBase::testBasicInsert1()
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // Make an iterator pointing at b
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    pa.next();
    TS_ASSERT(pa.get() == &b);

    // Insert c
    ml.insert(pa, &c);

    // Validate iterator
    TS_ASSERT(pa.get() == &b);
    pa.next();
    TS_ASSERT(pa == ml.end());

    // Validate list
    // (This intentionally uses the assignment operator to test that as well.)
    pa = ml.begin();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &a);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &c);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &b);
    pa.next();
    TS_ASSERT(pa == ml.end());
}

/** Test afl::container::PtrMultiListBase::insert, beginning of list. */
void
TestContainerPtrMultiListBase::testBasicInsert2()
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // Make an iterator pointing at a
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    TS_ASSERT(pa.get() == &a);
    TS_ASSERT(pa == ml.begin());

    // Insert c
    ml.insert(pa, &c);

    // Validate iterator
    TS_ASSERT(pa != ml.begin());
    TS_ASSERT(pa.get() == &a);

    // Validate list
    pa = ml.begin();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &c);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &a);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &b);
    pa.next();
    TS_ASSERT(pa == ml.end());
}

/** Test afl::container::PtrMultiListBase::insert, end of list. */
void
TestContainerPtrMultiListBase::testBasicInsert3()
{
    int a = 1, b = 2, c = 3;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);

    // Make an iterator pointing after b
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    pa.next();
    pa.next();
    TS_ASSERT(pa == ml.end());

    // Insert c
    ml.insert(pa, &c);
    
    // Validate iterator
    TS_ASSERT(pa == ml.end());

    // Validate list
    pa = ml.begin();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &a);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &b);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &c);
    pa.next();
    TS_ASSERT(pa == ml.end());
}

/** Test afl::container::PtrMultiListBase::insert, at deleted element. */
void
TestContainerPtrMultiListBase::testBasicInsert4()
{
    int a = 1, b = 2, c = 3, d = 4;
    afl::container::PtrMultiListBase ml(nullDeleter);
    ml.pushBackNew(&a);
    ml.pushBackNew(&b);
    ml.pushBackNew(&c);

    // Make an iterator pointing at b
    afl::container::PtrMultiListBase::iterator pa = ml.begin();
    pa.next();
    TS_ASSERT(pa.get() == &b);

    // Make another iterator and erase it
    afl::container::PtrMultiListBase::iterator pb = pa;
    ml.erase(pb);
    TS_ASSERT(pb.get() == 0);

    // Validate pa
    TS_ASSERT(pa.get() == 0);

    // Insert d
    ml.insert(pa, &d);

    // Validate iterator
    TS_ASSERT(pa.get() == 0);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &c);
    pa.next();
    TS_ASSERT(pa == ml.end());

    // Validate list
    pa = ml.begin();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &a);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &d);
    pa.next();
    TS_ASSERT(pa != ml.end());
    TS_ASSERT(pa.get() == &c);
    pa.next();
    TS_ASSERT(pa == ml.end());
}
