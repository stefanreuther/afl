/**
  *  \file test/afl/container/ptrvectortest.cpp
  *  \brief Test for afl::container::PtrVector
  */

#include "afl/container/ptrvector.hpp"
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


    bool sortFunction(int a, int b)
    {
        return b < a;
    }
}

/** Simple test. */
AFL_TEST("afl.container.PtrVector", a)
{
    // Blank vector
    afl::container::PtrVector<Obj> pv;
    a.check("01. empty", pv.empty());
    a.checkEqual("02. size", pv.size(), 0U);
    a.checkEqual("03. live", Obj::live, 0);

    // Add three objects
    pv.pushBackNew(new Obj(1));
    pv.pushBackNew(new Obj(2));
    pv.pushBackNew(new Obj(3));   // 1 2 3
    a.check("11. empty", !pv.empty());
    a.checkEqual("12. size", pv.size(), 3U);
    a.checkEqual("13. live", Obj::live, 3);
    a.checkEqual("14. value", pv[0]->n, 1);
    a.checkEqual("15. value", pv[1]->n, 2);
    a.checkEqual("16. value", pv[2]->n, 3);
    a.checkEqual("17. front", pv[0], pv.front());
    a.checkEqual("18. front", pv[2], pv.back());

    // Remove one
    pv.popBack();              // 1 2
    a.checkEqual("21. size", pv.size(), 2U);
    a.checkEqual("22. live", Obj::live, 2);
    a.checkEqual("23. value", pv[0]->n, 1);
    a.checkEqual("24. value", pv[1]->n, 2);

    // Add some more
    pv.pushBackNew(new Obj(4));
    pv.pushBackNew(new Obj(5));
    pv.pushBackNew(new Obj(6));
    pv.pushBackNew(new Obj(7));   // 1 2 4 5 6 7
    a.checkEqual("31. live", Obj::live, 6);
    a.checkEqual("32. front", pv.front()->n, 1);
    a.checkEqual("33. back", pv.back()->n,  7);

    // Swap some
    pv.swapElements(1, 4);    // 1 6 4 5 2 7
    a.checkEqual("41. live", Obj::live, 6);
    a.checkEqual("42. value", pv[0]->n, 1);
    a.checkEqual("43. value", pv[1]->n, 6);
    a.checkEqual("44. value", pv[2]->n, 4);
    a.checkEqual("45. value", pv[3]->n, 5);
    a.checkEqual("46. value", pv[4]->n, 2);
    a.checkEqual("47. value", pv[5]->n, 7);

    // Delete one
    pv.erase(pv.begin() + 2);   // 1 6 5 2 7
    a.checkEqual("51. live", Obj::live, 5);
    a.checkEqual("52. size", pv.size(), 5U);
    a.checkEqual("53. value", pv[0]->n, 1);
    a.checkEqual("54. value", pv[1]->n, 6);
    a.checkEqual("55. value", pv[2]->n, 5);
    a.checkEqual("56. value", pv[3]->n, 2);
    a.checkEqual("57. value", pv[4]->n, 7);

    // Delete two
    pv.erase(pv.begin() + 3, pv.end());  // 1 6 5
    a.checkEqual("61. live", Obj::live, 3);
    a.checkEqual("62. size", pv.size(), 3U);
    a.checkEqual("63. value", pv[0]->n, 1);
    a.checkEqual("64. value", pv[1]->n, 6);
    a.checkEqual("65. value", pv[2]->n, 5);

    // Delete another one
    delete pv.extractElement(1);
    a.checkEqual("71. live", Obj::live, 2);      // 1 nil 5
    a.checkEqual("72. size", pv.size(), 3U);
    a.checkEqual("73. value", pv[0]->n, 1);
    a.checkEqual("74. value", pv[1],    (Obj*)0);
    a.checkEqual("75. value", pv[2]->n, 5);

    pv.erase(pv.begin()+1);              // 1 5
    a.checkEqual("81. live", Obj::live, 2);
    a.checkEqual("82. size", pv.size(), 2U);
    a.checkEqual("83. value", pv[0]->n, 1);
    a.checkEqual("84. value", pv[1]->n, 5);

    // Insert one
    pv.insertNew(pv.begin()+1, new Obj(4)); // 1 4 5
    a.checkEqual("91. live", Obj::live, 3);
    a.checkEqual("92. size", pv.size(), 3U);
    a.checkEqual("93. value", pv[0]->n, 1);
    a.checkEqual("94. value", pv[1]->n, 4);
    a.checkEqual("95. value", pv[2]->n, 5);
}

/** Test behaviour with const members. */
AFL_TEST("afl.container.PtrVector:const", a)
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(99));
    a.checkEqual("01. size", v.size(), 1U);
    a.checkNonNull("02. ptr", v[0]);
    a.checkEqual("03. value", *v[0], 99);

    v.replaceElementNew(0, new int(88));
    a.checkEqual("11. size", v.size(), 1U);
    a.checkNonNull("12. ptr", v[0]);
    a.checkEqual("13. value", *v[0], 88);

    v.insertNew(v.end(), new int(42));
    a.checkEqual("21. size", v.size(), 2U);
    a.checkNonNull("22. ptr", v[1]);
    a.checkEqual("23. value", *v[1], 42);
}

/** Test sorting, standard case. */
AFL_TEST("afl.container.PtrVector:sort:default", a)
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(new int(20));
    v.pushBackNew(new int(15));
    v.pushBackNew(new int(25));
    v.pushBackNew(new int(5));
    v.sort();

    a.checkEqual("size", v.size(), size_t(5));
    a.checkEqual("idx 0", *v[0], 5);
    a.checkEqual("idx 1", *v[1], 10);
    a.checkEqual("idx 2", *v[2], 15);
    a.checkEqual("idx 3", *v[3], 20);
    a.checkEqual("idx 4", *v[4], 25);
}

/** Test sorting, with predicate. */
AFL_TEST("afl.container.PtrVector:sort:predicate", a)
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(new int(20));
    v.pushBackNew(new int(15));
    v.pushBackNew(new int(25));
    v.pushBackNew(new int(5));
    v.sort(sortFunction);

    a.checkEqual("size", v.size(), size_t(5));
    a.checkEqual("idx 0", *v[0], 25);
    a.checkEqual("idx 1", *v[1], 20);
    a.checkEqual("idx 2", *v[2], 15);
    a.checkEqual("idx 3", *v[3], 10);
    a.checkEqual("idx 4", *v[4], 5);
}

/** Test sorting, with null pointers. */
AFL_TEST("afl.container.PtrVector:sort:default-null", a)
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(0);
    v.pushBackNew(new int(5));
    v.sort();

    a.checkEqual("size", v.size(), size_t(3));
    a.checkEqual("idx 0", *v[0], 5);
    a.checkEqual("idx 1", *v[1], 10);
    a.checkNull("idx 2", v[2]);
}

/** Test sorting, with null pointers and predicate. */
AFL_TEST("afl.container.PtrVector:sort:predicate-null", a)
{
    afl::container::PtrVector<const int> v;
    v.pushBackNew(new int(10));
    v.pushBackNew(0);
    v.pushBackNew(new int(5));
    v.sort(sortFunction);

    a.checkEqual("size", v.size(), size_t(3));
    a.checkEqual("idx 0", *v[0], 10);
    a.checkEqual("idx 1", *v[1], 5);
    a.checkNull("idx 2", v[2]);
}

/** Test resize(). */
AFL_TEST("afl.container.PtrVector:resize", a)
{
    // Blank vector
    afl::container::PtrVector<Obj> pv;
    a.check("01. empty", pv.empty());
    a.checkEqual("02. size", pv.size(), 0U);
    a.checkEqual("03. live", Obj::live, 0);

    // Append three elements
    pv.pushBackNew(new Obj(10));
    pv.pushBackNew(new Obj(11));
    pv.pushBackNew(new Obj(12));
    a.check("11. empty", !pv.empty());
    a.checkEqual("12. size", pv.size(), 3U);
    a.checkEqual("13. live", Obj::live, 3);

    // Resize to drop
    pv.resize(2);
    a.check("21. empty", !pv.empty());
    a.checkEqual("22. size", pv.size(), 2U);
    a.checkEqual("23. live", Obj::live, 2);
    a.checkEqual("24. value", pv[0]->n, 10);
    a.checkEqual("25. value", pv[1]->n, 11);

    // Resize to extend
    pv.resize(4);
    a.check("31. empty", !pv.empty());
    a.checkEqual("32. size", pv.size(), 4U);
    a.checkEqual("33. live", Obj::live, 2);
    a.checkEqual("34. value", pv[0]->n, 10);
    a.checkEqual("35. value", pv[1]->n, 11);
    a.checkNull("36. null", pv[2]);
    a.checkNull("37. null", pv[3]);

    // Clear
    pv.resize(0);
    a.check("41. empty", pv.empty());
    a.checkEqual("42. null", Obj::live, 0);
}
