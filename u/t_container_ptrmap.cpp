/**
  *  \file u/t_container_ptrmap.cpp
  *  \brief Test for afl::container::PtrMap
  */

#include "afl/container/ptrmap.hpp"

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
TestContainerPtrMap::testIt()
{
    const Obj* null = 0;

    // Blank map
    afl::container::PtrMap<int,Obj> pm;
    TS_ASSERT(pm.empty());
    TS_ASSERT_EQUALS(pm.size(), 0U);
    TS_ASSERT_EQUALS(Obj::live, 0);
    TS_ASSERT_EQUALS(pm.begin(), pm.end());

    // Add three objects
    pm.insertNew(42, new Obj(1));
    pm.insertNew(23, new Obj(2));
    pm.insertNew(95, new Obj(3));
    TS_ASSERT(!pm.empty());
    TS_ASSERT_EQUALS(pm.size(), 3U);
    TS_ASSERT_EQUALS(Obj::live, 3);
    TS_ASSERT_EQUALS(pm[42]->n, 1);
    TS_ASSERT_EQUALS(pm[23]->n, 2);
    TS_ASSERT_EQUALS(pm[95]->n, 3);
    TS_ASSERT_EQUALS(pm[17], null);

    // Iteration
    {
        afl::container::PtrMap<int,Obj>::iterator i = pm.begin();
        TS_ASSERT_DIFFERS(i, pm.end());
        TS_ASSERT_EQUALS(i->first, 23);
        TS_ASSERT_EQUALS(i->second->n, 2);
        ++i;
        TS_ASSERT_DIFFERS(i, pm.end());
        TS_ASSERT_EQUALS(i->first, 42);
        TS_ASSERT_EQUALS(i->second->n, 1);
        ++i;
        TS_ASSERT_DIFFERS(i, pm.end());
        TS_ASSERT_EQUALS(i->first, 95);
        TS_ASSERT_EQUALS(i->second->n, 3);
        ++i;
        TS_ASSERT_EQUALS(i, pm.end());
        --i;
        TS_ASSERT_DIFFERS(i, pm.end());
        TS_ASSERT_EQUALS(i->first, 95);
        TS_ASSERT_EQUALS(i->second->n, 3);
    }

    // Remove one
    size_t n = pm.erase(12);
    TS_ASSERT_EQUALS(n, 0U);
    n = pm.erase(95);
    TS_ASSERT_EQUALS(n, 1U);
    TS_ASSERT_EQUALS(pm.size(), 2U);
    TS_ASSERT_EQUALS(Obj::live, 2);
    TS_ASSERT_EQUALS(pm[42]->n, 1);
    TS_ASSERT_EQUALS(pm[23]->n, 2);
    TS_ASSERT_EQUALS(pm[95], null);

    // Remove all
    pm.erase(pm.begin(), pm.end());
    TS_ASSERT(pm.empty());
    TS_ASSERT_EQUALS(pm.size(), 0U);
    TS_ASSERT_EQUALS(Obj::live, 0);
    TS_ASSERT_EQUALS(pm.begin(), pm.end());

    // Add some more
    pm.insertNew(7,  new Obj(4));
    pm.insertNew(8,  new Obj(5));
    pm.insertNew(9,  new Obj(6));
    pm.insertNew(10, new Obj(7));
    TS_ASSERT_EQUALS(Obj::live, 4);
    TS_ASSERT_EQUALS(pm[7]->n, 4);
    TS_ASSERT_EQUALS(pm[8]->n, 5);
    TS_ASSERT_EQUALS(pm[9]->n, 6);
    TS_ASSERT_EQUALS(pm[10]->n, 7);

    // Replace
    pm.insertNew(8, new Obj(8));
    TS_ASSERT_EQUALS(Obj::live, 4);
    TS_ASSERT_EQUALS(pm[7]->n, 4);
    TS_ASSERT_EQUALS(pm[8]->n, 8);
    TS_ASSERT_EQUALS(pm[9]->n, 6);
    TS_ASSERT_EQUALS(pm[10]->n, 7);

    // Swap some
    pm.swapElements(pm.find(7), pm.find(9));
    TS_ASSERT_EQUALS(Obj::live, 4);
    TS_ASSERT_EQUALS(pm[7]->n, 6);
    TS_ASSERT_EQUALS(pm[8]->n, 8);
    TS_ASSERT_EQUALS(pm[9]->n, 4);
    TS_ASSERT_EQUALS(pm[10]->n, 7);

    // Extract
    Obj* o = pm.extractElement(pm.find(10));
    TS_ASSERT_DIFFERS(o, null);
    TS_ASSERT_EQUALS(o->n, 7);
    TS_ASSERT_EQUALS(Obj::live, 4);
    delete o;
    TS_ASSERT_EQUALS(Obj::live, 3);
    TS_ASSERT_EQUALS(pm.find(10), pm.end());

    // Temporary map
    {
        afl::container::PtrMap<int,Obj> tm;
        tm.insertNew(0, new Obj(22));
        tm.insertNew(99, 0);
        tm.insertNew(42, new Obj(17));
        TS_ASSERT_EQUALS(Obj::live, 5);
    }
    TS_ASSERT_EQUALS(Obj::live, 3);

    // Swap
    afl::container::PtrMap<int,Obj> pm2;
    pm2.insertNew(8, new Obj(15));
    TS_ASSERT_EQUALS(pm2.size(), 1U);
    pm2.swap(pm);
    TS_ASSERT_EQUALS(pm2.size(), 3U);
    TS_ASSERT_EQUALS(pm2[7]->n, 6);
    TS_ASSERT_EQUALS(pm2[8]->n, 8);
    TS_ASSERT_EQUALS(pm2[9]->n, 4);

    TS_ASSERT_EQUALS(pm.size(), 1U);
    TS_ASSERT_EQUALS(pm[8]->n, 15);

    // Clear
    pm.clear();
    TS_ASSERT_EQUALS(pm.size(), 0U);
    TS_ASSERT(pm.empty());
    TS_ASSERT_EQUALS(Obj::live, 3);

    // Count
    TS_ASSERT_EQUALS(pm.count(0), 0U);
    TS_ASSERT_EQUALS(pm.count(7), 0U);
    TS_ASSERT_EQUALS(pm.count(10), 0U);
    TS_ASSERT_EQUALS(pm2.count(0), 0U);
    TS_ASSERT_EQUALS(pm2.count(7), 1U);
    TS_ASSERT_EQUALS(pm2.count(8), 1U);

    // Erase iterator
    pm2.erase(pm2.begin());
    TS_ASSERT_EQUALS(pm2.size(), 2U);
    TS_ASSERT_EQUALS(Obj::live, 2);
    TS_ASSERT_EQUALS(pm2[7], null);
    TS_ASSERT_EQUALS(pm2[8]->n, 8);
}

/** Test find, lower_bound, upper_bound. */
void
TestContainerPtrMap::testFind()
{
    // Make a map
    afl::container::PtrMap<int,Obj> pm;
    pm.insertNew(10, new Obj(1));
    pm.insertNew(20, new Obj(2));
    pm.insertNew(30, new Obj(3));
    pm.insertNew(40, new Obj(4));
    pm.insertNew(50, new Obj(5));

    // Check iterators
    afl::container::PtrMap<int,Obj>::iterator it;
    for (int i = 10; i <= 50; i += 10) {
        // Finding an element works
        it = pm.find(i);
        TS_ASSERT_DIFFERS(it, pm.end());
        TS_ASSERT_EQUALS(it->first, i);

        // Finding a nonexistant element fails
        it = pm.find(i - 1);
        TS_ASSERT_EQUALS(it, pm.end());
        it = pm.find(i + 1);
        TS_ASSERT_EQUALS(it, pm.end());

        // Finding first element >= i finds i
        it = pm.lower_bound(i);
        TS_ASSERT_DIFFERS(it, pm.end());
        TS_ASSERT_EQUALS(it->first, i);

        // Finding first element >= i-1 finds i
        it = pm.lower_bound(i - 1);
        TS_ASSERT_DIFFERS(it, pm.end());
        TS_ASSERT_EQUALS(it->first, i);

        // Finding first element > i-1 finds i
        it = pm.upper_bound(i - 1);
        TS_ASSERT_DIFFERS(it, pm.end());
        TS_ASSERT_EQUALS(it->first, i);
    }

    // Boundary cases
    TS_ASSERT_EQUALS(pm.lower_bound(0), pm.begin());
    TS_ASSERT_EQUALS(pm.lower_bound(10), pm.begin());
    TS_ASSERT_EQUALS(pm.upper_bound(0), pm.begin());

    TS_ASSERT_EQUALS(pm.lower_bound(100), pm.end());
    TS_ASSERT_EQUALS(pm.upper_bound(50), pm.end());
    TS_ASSERT_EQUALS(pm.upper_bound(100), pm.end());
}

/** Test behaviour with const members. */
void
TestContainerPtrMap::testConst()
{
    afl::container::PtrMap<int, const int> m;
    m.insertNew(1, new int(99));
    TS_ASSERT(m[0] == 0);
    TS_ASSERT(m[1] != 0);
    TS_ASSERT_EQUALS(*m[1], 99);
}
