/**
  *  \file test/afl/tmp/typelisttest.cpp
  *  \brief Test for afl::tmp::TypeList
  */

#include "afl/tmp/typelist.hpp"

#include "afl/base/staticassert.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.tmp.TypeList", a)
{
    using afl::tmp::TypeListContains;
    using afl::tmp::TypeList;

    a.checkEqual("01", (TypeListContains<int, TypeList<char, TypeList<short, TypeList<int, TypeList<long, void> > > > >::result), true);
    a.checkEqual("02", (TypeListContains<unsigned, TypeList<char, TypeList<short, TypeList<int, TypeList<long, void> > > > >::result), false);
    a.checkEqual("03", (TypeListContains<int, void>::result), false);

    // Actually, a compile-time check:
    static_assert((TypeListContains<int, TypeList<char, TypeList<short, TypeList<int, TypeList<long, void> > > > >::result), "test");
}
