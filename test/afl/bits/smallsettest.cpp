/**
  *  \file test/afl/bits/smallsettest.cpp
  *  \brief Test for afl::bits::SmallSet
  */

#include "afl/bits/smallset.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    enum Numbers { One, Two, Three };
}

/** Simple tests. */
AFL_TEST("afl.bits.SmallSet:main", a)
{
    typedef afl::bits::SmallSet<Numbers> Num_set;

    Num_set e;
    Num_set s1(One), s2(Two), s3(Three);

    a.check("01. empty", e.empty());
    a.check("02. empty", !s1.empty());
    a.check("03. empty", !s2.empty());
    a.check("04. empty", !s3.empty());

    a.check("11. contains", s1.contains(One));
    a.check("12. contains", !s1.contains(Two));
    a.check("13. contains", !s1.contains(Three));

    a.check("21. eq", s1 == One);
    a.check("22. eq", s2 == Two);
    a.check("23. eq", s3 == Three);
    a.checkEqual("24. eq", s1, One);         // Check that test framework can compare sets

    a.check("31. ne", s3 != One);
    a.check("32. ne", s1 != Two);
    a.check("33. ne", s2 != Three);
    a.checkDifferent("34. ne", s3, One);     // Check that test framework can compare sets

    a.check("41. contains", (s1 + s2).contains(One));
    a.check("42. contains", !(s1 + s2).contains(Three));
    a.check("43. contains", (s1 + s2).contains(s1));
    a.check("44. contains", (s1 + s2).contains(s2));
    a.check("45. contains", !(s1 + s2).contains(s3));

    a.checkDifferent("51. value", s1.toInteger(), s2.toInteger());
    a.checkEqual("52. value", (s1 + Two - One).toInteger(), s2.toInteger());

    Num_set n(s1);
    n += Two;
    a.check("61. containsAnyOf", n.containsAnyOf(s1));
    a.check("62. contains", n.contains(s1));
    a.check("63. contains", !s1.contains(n));

    a.checkEqual("71. xor", (n ^ s1).toInteger(), s2.toInteger());
    a.checkEqual("72. xor", (s1 ^ s2).toInteger(), n.toInteger());
    a.checkEqual("73. xor", (n ^ s2).toInteger(), s1.toInteger());

    a.check("81. and", (s1 & s2).empty());
    a.check("82. sub", !(s1 - s2).empty());
    a.checkEqual("83. sub", (n - s2).toInteger(), s1.toInteger());

    Num_set x;
    x.set(Two, true);
    x.set(Three, false);
    a.checkEqual("91. contains", x.contains(Two), true);
    a.checkEqual("92. contains", x.contains(Three), false);

    x.set(Two, false);
    x.set(Three, true);
    a.checkEqual("A1. contains", x.contains(Two), false);
    a.checkEqual("A2. contains", x.contains(Three), true);
}

/** Test cardinality-related functions (take, size). */
AFL_TEST("afl.bits.SmallSet:cardinality", a)
{
    afl::bits::SmallSet<int> aa = afl::bits::SmallSet<int>::fromInteger(0x35);
    a.checkEqual("01. size", aa.size(), 4U);

    afl::bits::SmallSet<int> bb = aa.take(3);
    a.checkEqual("11. size", aa.size(), 1U);
    a.checkEqual("12. size", bb.size(), 3U);
    a.checkEqual("13. size", aa.toInteger(), 0x20U);
    a.checkEqual("14. size", bb.toInteger(), 0x15U);
}

/** Test size() for various representations. */
AFL_TEST("afl.bits.SmallSet:representation", a)
{
    a.checkEqual("u8", (afl::bits::SmallSet<int,uint8_t>::fromInteger(0x81).size()), 2U);
    a.checkEqual("u16", (afl::bits::SmallSet<int,uint16_t>::fromInteger(0x8001).size()), 2U);
    a.checkEqual("u32", (afl::bits::SmallSet<int,uint32_t>::fromInteger(0x80000001UL).size()), 2U);
    a.checkEqual("u64", (afl::bits::SmallSet<int,uint64_t>::fromInteger(0x8000000000000001ULL).size()), 2U);
}
