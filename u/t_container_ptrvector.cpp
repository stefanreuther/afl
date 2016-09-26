/**
  *  \file u/t_container_ptrvector.cpp
  *  \brief Test for afl::container::PtrVector
  */

#include "afl/container/ptrvector.hpp"

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


    bool sortFunction(int a, int b)
    {
        return b < a;
    }
}

/** Simple test. */
void
TestContainerPtrVector::testIt()
{
    // Blank vector
    afl::container::PtrVector<Obj> pv;
    TS_ASSERT(pv.empty());
    TS_ASSERT_EQUALS(pv.size(), 0U);
    TS_ASSERT_EQUALS(Obj::live, 0);

    // Add three objects
    pv.pushBackNew(new Obj(1));
    pv.pushBackNew(new Obj(2));
    pv.pushBackNew(new Obj(3));   // 1 2 3
    TS_ASSERT(!pv.empty());
    TS_ASSERT_EQUALS(pv.size(), 3U);
    TS_ASSERT_EQUALS(Obj::live, 3);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 2);
    TS_ASSERT_EQUALS(pv[2]->n, 3);
    TS_ASSERT_EQUALS(pv[0], pv.front());
    TS_ASSERT_EQUALS(pv[2], pv.back());

    // Remove one
    pv.popBack();              // 1 2
    TS_ASSERT_EQUALS(pv.size(), 2U);
    TS_ASSERT_EQUALS(Obj::live, 2);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 2);

    // Add some more
    pv.pushBackNew(new Obj(4));
    pv.pushBackNew(new Obj(5));
    pv.pushBackNew(new Obj(6));
    pv.pushBackNew(new Obj(7));   // 1 2 4 5 6 7
    TS_ASSERT_EQUALS(Obj::live, 6);
    TS_ASSERT_EQUALS(pv.front()->n, 1);
    TS_ASSERT_EQUALS(pv.back()->n,  7);

    // Swap some
    pv.swapElements(1, 4);    // 1 6 4 5 2 7
    TS_ASSERT_EQUALS(Obj::live, 6);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 6);
    TS_ASSERT_EQUALS(pv[2]->n, 4);
    TS_ASSERT_EQUALS(pv[3]->n, 5);
    TS_ASSERT_EQUALS(pv[4]->n, 2);
    TS_ASSERT_EQUALS(pv[5]->n, 7);

    // Delete one
    pv.erase(pv.begin() + 2);   // 1 6 5 2 7
    TS_ASSERT_EQUALS(Obj::live, 5);
    TS_ASSERT_EQUALS(pv.size(), 5U);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 6);
    TS_ASSERT_EQUALS(pv[2]->n, 5);
    TS_ASSERT_EQUALS(pv[3]->n, 2);
    TS_ASSERT_EQUALS(pv[4]->n, 7);

    // Delete two
    pv.erase(pv.begin() + 3, pv.end());  // 1 6 5
    TS_ASSERT_EQUALS(Obj::live, 3);
    TS_ASSERT_EQUALS(pv.size(), 3U);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 6);
    TS_ASSERT_EQUALS(pv[2]->n, 5);

    // Delete another one
    delete pv.extractElement(1);
    TS_ASSERT_EQUALS(Obj::live, 2);      // 1 nil 5
    TS_ASSERT_EQUALS(pv.size(), 3U);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1],    (Obj*)0);
    TS_ASSERT_EQUALS(pv[2]->n, 5);

    pv.erase(pv.begin()+1);              // 1 5
    TS_ASSERT_EQUALS(Obj::live, 2);
    TS_ASSERT_EQUALS(pv.size(), 2U);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 5);

    // Insert one
    pv.insertNew(pv.begin()+1, new Obj(4)); // 1 4 5
    TS_ASSERT_EQUALS(Obj::live, 3);
    TS_ASSERT_EQUALS(pv.size(), 3U);
    TS_ASSERT_EQUALS(pv[0]->n, 1);
    TS_ASSERT_EQUALS(pv[1]->n, 4);
    TS_ASSERT_EQUALS(pv[2]->n, 5);
}

/** Test behaviour with const members. */
void
TestContainerPtrVector::testConst()
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(99));
    TS_ASSERT_EQUALS(v.size(), 1U);
    TS_ASSERT(v[0] != 0);
    TS_ASSERT_EQUALS(*v[0], 99);

    v.replaceElementNew(0, new int(88));
    TS_ASSERT_EQUALS(v.size(), 1U);
    TS_ASSERT(v[0] != 0);
    TS_ASSERT_EQUALS(*v[0], 88);

    v.insertNew(v.end(), new int(42));
    TS_ASSERT_EQUALS(v.size(), 2U);
    TS_ASSERT(v[1] != 0);
    TS_ASSERT_EQUALS(*v[1], 42);
}

/** Test sorting, standard case. */
void
TestContainerPtrVector::testSort1()
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(new int(20));
    v.pushBackNew(new int(15));
    v.pushBackNew(new int(25));
    v.pushBackNew(new int(5));
    v.sort();

    TS_ASSERT_EQUALS(v.size(), size_t(5));
    TS_ASSERT_EQUALS(*v[0], 5);
    TS_ASSERT_EQUALS(*v[1], 10);
    TS_ASSERT_EQUALS(*v[2], 15);
    TS_ASSERT_EQUALS(*v[3], 20);
    TS_ASSERT_EQUALS(*v[4], 25);
}

/** Test sorting, with predicate. */
void
TestContainerPtrVector::testSort2()
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(new int(20));
    v.pushBackNew(new int(15));
    v.pushBackNew(new int(25));
    v.pushBackNew(new int(5));
    v.sort(sortFunction);

    TS_ASSERT_EQUALS(v.size(), size_t(5));
    TS_ASSERT_EQUALS(*v[0], 25);
    TS_ASSERT_EQUALS(*v[1], 20);
    TS_ASSERT_EQUALS(*v[2], 15);
    TS_ASSERT_EQUALS(*v[3], 10);
    TS_ASSERT_EQUALS(*v[4], 5);
}

/** Test sorting, with null pointers. */
void
TestContainerPtrVector::testSort3()
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(0);
    v.pushBackNew(new int(5));
    v.sort();

    TS_ASSERT_EQUALS(v.size(), size_t(3));
    TS_ASSERT_EQUALS(*v[0], 5);
    TS_ASSERT_EQUALS(*v[1], 10);
    TS_ASSERT_EQUALS(v[2], (int*) 0);
}

/** Test sorting, with null pointers and predicate. */
void
TestContainerPtrVector::testSort4()
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(0);
    v.pushBackNew(new int(5));
    v.sort(sortFunction);

    TS_ASSERT_EQUALS(v.size(), size_t(3));
    TS_ASSERT_EQUALS(*v[0], 10);
    TS_ASSERT_EQUALS(*v[1], 5);
    TS_ASSERT_EQUALS(v[2], (int*) 0);
}
