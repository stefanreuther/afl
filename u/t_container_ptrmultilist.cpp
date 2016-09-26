/**
  *  \file u/t_container_ptrmultilist.cpp
  *  \brief Test for afl::container::PtrMultiList
  */

#include "afl/container/ptrmultilist.hpp"

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

    bool objCompare(Obj* a, Obj* b)
        {
            return a->n < b->n;
        }
}

/** Test typed operation. */
void
TestContainerPtrMultiList::testTyped()
{
    TS_ASSERT_EQUALS(Obj::live, 0);

    {
        // Create something
        afl::container::PtrMultiList<Obj> ml;
        TS_ASSERT(ml.empty());
        TS_ASSERT_EQUALS(ml.size(), 0U);
        ml.pushBackNew(new Obj(1));
        TS_ASSERT(!ml.empty());
        TS_ASSERT_EQUALS(ml.size(), 1U);
        ml.pushBackNew(new Obj(2));
        ml.pushBackNew(new Obj(3));
        TS_ASSERT(!ml.empty());
        TS_ASSERT_EQUALS(Obj::live, 3);
        TS_ASSERT_EQUALS(ml.size(), 3U);

        // Regular iteration
        {
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            TS_ASSERT(i != ml.end());
            TS_ASSERT(i == ml.begin());
            TS_ASSERT_EQUALS((*i)->n, 1);
            ++i;
            TS_ASSERT(i != ml.end());
            TS_ASSERT(i != ml.begin());
            TS_ASSERT_EQUALS((*i)->n, 2);
            ++i;
            TS_ASSERT(i != ml.end());
            TS_ASSERT(i != ml.begin());
            TS_ASSERT_EQUALS((*i)->n, 3);
            ++i;
            TS_ASSERT(i == ml.end());
            TS_ASSERT(i != ml.begin());
        }

        // Regular iteration with two iterators
        {
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            afl::container::PtrMultiList<Obj>::iterator j = ml.begin();
            TS_ASSERT(i == j);
            TS_ASSERT_EQUALS((*i)->n, 1);
            TS_ASSERT_EQUALS((*j)->n, 1);
            ++i;
            TS_ASSERT(i != j);
            TS_ASSERT_EQUALS((*i)->n, 2);
            TS_ASSERT_EQUALS((*j)->n, 1);
            ++j;
            TS_ASSERT(i == j);
            TS_ASSERT_EQUALS((*i)->n, 2);
            TS_ASSERT_EQUALS((*j)->n, 2);
            TS_ASSERT_EQUALS(Obj::live, 3);
        }
        
        // Deletion of second element with both iterators pointing there
        {
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            afl::container::PtrMultiList<Obj>::iterator j = ml.begin();
            ++i;
            ++j;
            ml.erase(i);
            TS_ASSERT(*i == 0);
            TS_ASSERT(*j == 0);
            ++i;
            ++j;
            TS_ASSERT_EQUALS((*i)->n, 3);
            TS_ASSERT_EQUALS((*j)->n, 3);
            TS_ASSERT(i == j);
            TS_ASSERT_EQUALS(Obj::live, 2);
        }

        // List now has two elements. Try deleting the first.
        {
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            afl::container::PtrMultiList<Obj>::iterator j = ml.begin();
            ml.erase(i);
            TS_ASSERT(*i == 0);
            TS_ASSERT(*j == 0);
            ++i;
            ++j;
            TS_ASSERT_EQUALS((*i)->n, 3);
            TS_ASSERT_EQUALS((*j)->n, 3);
            TS_ASSERT_EQUALS(Obj::live, 1);
        }

        // List now has one element. Must be 3.
        {
            afl::container::PtrMultiList<Obj>::iterator i = ml.begin();
            TS_ASSERT_EQUALS((*i)->n, 3);
            TS_ASSERT_EQUALS(ml.size(), 1U);
        }

        // Insert one element to test insert()
        {
            ml.insert(ml.begin(), new Obj(9));
            TS_ASSERT_EQUALS(Obj::live, 2);
            TS_ASSERT_EQUALS(ml.size(), 2U);
            TS_ASSERT_EQUALS(ml.front()->n, 9);
        }
    }

    TS_ASSERT_EQUALS(Obj::live, 0);
}

/** Test typed merge operation. */
void
TestContainerPtrMultiList::testTypedMerge()
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

    TS_ASSERT_EQUALS(Obj::live, 12);
    TS_ASSERT_EQUALS(ml.size(), 6U);
    TS_ASSERT_EQUALS(ml2.size(), 6U);

    // Merge
    ml.merge(ml2, objCompare);
    TS_ASSERT_EQUALS(Obj::live, 12);
    TS_ASSERT_EQUALS(ml.size(), 12U);
    TS_ASSERT_EQUALS(ml2.size(), 0U);

    // Verify
    afl::container::PtrMultiList<Obj>::iterator p = ml.begin();
    for (int i = 1; i <= 12; ++i) {
        TS_ASSERT(p != ml.end());
        TS_ASSERT(*p != 0);
        TS_ASSERT_EQUALS((*p)->n, i);
        ++p;
    }
    TS_ASSERT(p == ml.end());

    // Merge and verify again (pathological case I)
    ml.merge(ml2, objCompare);
    TS_ASSERT_EQUALS(Obj::live, 12);
    TS_ASSERT_EQUALS(ml.size(), 12U);
    TS_ASSERT_EQUALS(ml2.size(), 0U);

    // Verify
    p = ml.begin();
    for (int i = 1; i <= 12; ++i) {
        TS_ASSERT(p != ml.end());
        TS_ASSERT(*p != 0);
        TS_ASSERT_EQUALS((*p)->n, i);
        ++p;
    }
    TS_ASSERT(p == ml.end());

    // Merge and verify again (pathological case II)
    ml2.merge(ml, objCompare);
    TS_ASSERT_EQUALS(Obj::live, 12);
    TS_ASSERT_EQUALS(ml2.size(), 12U);
    TS_ASSERT_EQUALS(ml.size(), 0U);

    // Verify
    p = ml2.begin();
    for (int i = 1; i <= 12; ++i) {
        TS_ASSERT(p != ml2.end());
        TS_ASSERT(*p != 0);
        TS_ASSERT_EQUALS((*p)->n, i);
        ++p;
    }
    TS_ASSERT(p == ml2.end());
}

/** Test typed sort operation. */
void
TestContainerPtrMultiList::testTypedSort()
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

    TS_ASSERT_EQUALS(Obj::live, 9);
    TS_ASSERT_EQUALS(ml.size(), 9U);

    // Sort
    ml.sort(objCompare);
    
    // Verify
    afl::container::PtrMultiList<Obj>::iterator p = ml.begin();
    for (int i = 1; i <= 9; ++i) {
        TS_ASSERT(p != ml.end());
        TS_ASSERT(*p != 0);
        TS_ASSERT_EQUALS((*p)->n, i);
        ++p;
    }
    TS_ASSERT(p == ml.end());
}
