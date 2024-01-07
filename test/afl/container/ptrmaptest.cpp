/**
  *  \file test/afl/container/ptrmaptest.cpp
  *  \brief Test for afl::container::PtrMap
  */

#include "afl/container/ptrmap.hpp"
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
AFL_TEST("afl.container.PtrMap", a)
{
    // Blank map
    afl::container::PtrMap<int,Obj> pm;
    a.check("01. empty", pm.empty());
    a.checkEqual("02. size", pm.size(), 0U);
    a.checkEqual("03. live", Obj::live, 0);
    a.check("04. it", pm.begin() == pm.end());

    // Add three objects
    pm.insertNew(42, new Obj(1));
    pm.insertNew(23, new Obj(2));
    pm.insertNew(95, new Obj(3));
    a.check("11. empty", !pm.empty());
    a.checkEqual("12. size", pm.size(), 3U);
    a.checkEqual("13. live", Obj::live, 3);
    a.checkEqual("14. val", pm[42]->n, 1);
    a.checkEqual("15. val", pm[23]->n, 2);
    a.checkEqual("16. val", pm[95]->n, 3);
    a.checkNull("17. val", pm[17]);

    // Iteration
    {
        afl::container::PtrMap<int,Obj>::iterator i = pm.begin();
        a.check("21. end", i != pm.end());
        a.checkEqual("21. key", i->first, 23);
        a.checkEqual("21. value", i->second->n, 2);
        ++i;
        a.check("22. end", i != pm.end());
        a.checkEqual("22. key", i->first, 42);
        a.checkEqual("22. value", i->second->n, 1);
        ++i;
        a.check("23. end", i != pm.end());
        a.checkEqual("23. key", i->first, 95);
        a.checkEqual("23. value", i->second->n, 3);
        ++i;
        a.check("24. end", i == pm.end());
        --i;
        a.check("25. end", i != pm.end());
        a.checkEqual("25. key", i->first, 95);
        a.checkEqual("25. value", i->second->n, 3);
    }

    // Remove one
    size_t n = pm.erase(12);
    a.checkEqual("31. erase", n, 0U);
    n = pm.erase(95);
    a.checkEqual("32. erase", n, 1U);
    a.checkEqual("33. size", pm.size(), 2U);
    a.checkEqual("34. live", Obj::live, 2);
    a.checkEqual("35. val", pm[42]->n, 1);
    a.checkEqual("36. val", pm[23]->n, 2);
    a.checkNull("37. val", pm[95]);

    // Remove all
    pm.erase(pm.begin(), pm.end());
    a.check("41. empty", pm.empty());
    a.checkEqual("42. size", pm.size(), 0U);
    a.checkEqual("43. live", Obj::live, 0);
    a.check("44. it", pm.begin() == pm.end());

    // Add some more
    pm.insertNew(7,  new Obj(4));
    pm.insertNew(8,  new Obj(5));
    pm.insertNew(9,  new Obj(6));
    pm.insertNew(10, new Obj(7));
    a.checkEqual("51. live", Obj::live, 4);
    a.checkEqual("52. val", pm[7]->n, 4);
    a.checkEqual("53. val", pm[8]->n, 5);
    a.checkEqual("54. val", pm[9]->n, 6);
    a.checkEqual("55. val", pm[10]->n, 7);

    // Replace
    pm.insertNew(8, new Obj(8));
    a.checkEqual("61. live", Obj::live, 4);
    a.checkEqual("62. val", pm[7]->n, 4);
    a.checkEqual("63. val", pm[8]->n, 8);
    a.checkEqual("64. val", pm[9]->n, 6);
    a.checkEqual("65. val", pm[10]->n, 7);

    // Swap some
    pm.swapElements(pm.find(7), pm.find(9));
    a.checkEqual("71. live", Obj::live, 4);
    a.checkEqual("72. val", pm[7]->n, 6);
    a.checkEqual("73. val", pm[8]->n, 8);
    a.checkEqual("74. val", pm[9]->n, 4);
    a.checkEqual("75. val", pm[10]->n, 7);

    // Extract
    Obj* o = pm.extractElement(pm.find(10));
    a.checkNonNull("81. live", o);
    a.checkEqual("82. val", o->n, 7);
    a.checkEqual("83. live", Obj::live, 4);
    delete o;
    a.checkEqual("84. live", Obj::live, 3);
    a.check("85. find", pm.find(10) == pm.end());

    // Temporary map
    {
        afl::container::PtrMap<int,Obj> tm;
        tm.insertNew(0, new Obj(22));
        tm.insertNew(99, 0);
        tm.insertNew(42, new Obj(17));
        a.checkEqual("91. live", Obj::live, 5);
    }
    a.checkEqual("92. live", Obj::live, 3);

    // Swap
    afl::container::PtrMap<int,Obj> pm2;
    pm2.insertNew(8, new Obj(15));
    a.checkEqual("A1. size", pm2.size(), 1U);
    pm2.swap(pm);
    a.checkEqual("A2. size", pm2.size(), 3U);
    a.checkEqual("A3. val", pm2[7]->n, 6);
    a.checkEqual("A4. val", pm2[8]->n, 8);
    a.checkEqual("A5. val", pm2[9]->n, 4);

    a.checkEqual("A6. size", pm.size(), 1U);
    a.checkEqual("A7. val", pm[8]->n, 15);

    // Clear
    pm.clear();
    a.checkEqual("B1. size", pm.size(), 0U);
    a.check("B2. empty", pm.empty());
    a.checkEqual("B3. live", Obj::live, 3);

    // Count
    a.checkEqual("C1. count", pm.count(0), 0U);
    a.checkEqual("C2. count", pm.count(7), 0U);
    a.checkEqual("C3. count", pm.count(10), 0U);
    a.checkEqual("C4. count", pm2.count(0), 0U);
    a.checkEqual("C5. count", pm2.count(7), 1U);
    a.checkEqual("C6. count", pm2.count(8), 1U);

    // Erase iterator
    pm2.erase(pm2.begin());
    a.checkEqual("D1. size", pm2.size(), 2U);
    a.checkEqual("D2. live", Obj::live, 2);
    a.checkNull("D3. val", pm2[7]);
    a.checkEqual("D4. val", pm2[8]->n, 8);
}

/** Test find, lower_bound, upper_bound. */
AFL_TEST("afl.container.PtrMap:find", a)
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
        a.check("01. find", it != pm.end());
        a.checkEqual("02. val", it->first, i);

        // Finding a nonexistant element fails
        it = pm.find(i - 1);
        a.check("03. it", it == pm.end());
        it = pm.find(i + 1);
        a.check("04. it", it == pm.end());

        // Finding first element >= i finds i
        it = pm.lower_bound(i);
        a.check("05. lower_bound", it != pm.end());
        a.check("06. lower_bound", it->first == i);

        // Finding first element >= i-1 finds i
        it = pm.lower_bound(i - 1);
        a.check("07. lower_bound", it != pm.end());
        a.check("08. lower_bound", it->first == i);

        // Finding first element > i-1 finds i
        it = pm.upper_bound(i - 1);
        a.check("09. upper_bound", it != pm.end());
        a.check("10. upper_bound", it->first == i);
    }

    // Boundary cases
    a.check("lower_bound 0",   pm.lower_bound(0) == pm.begin());
    a.check("upper_bound 10",  pm.lower_bound(10) == pm.begin());
    a.check("upper_bound 0",   pm.upper_bound(0) == pm.begin());

    a.check("lower_bound 100", pm.lower_bound(100) == pm.end());
    a.check("upper_bound 50",  pm.upper_bound(50) == pm.end());
    a.check("upper_bound 100", pm.upper_bound(100) == pm.end());
}

/** Test behaviour with const members. */
AFL_TEST("afl.container.PtrMap:const", a)
{
    afl::container::PtrMap<int, const int> m;
    m.insertNew(1, new int(99));
    a.checkNull("ele 0", m[0]);
    a.checkNonNull("ele 1", m[1]);
    a.checkEqual("val 1", *m[1], 99);
}
