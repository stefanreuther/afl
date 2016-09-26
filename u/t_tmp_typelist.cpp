/**
  *  \file u/t_tmp_typelist.cpp
  *  \brief Test for afl::tmp::TypeList
  */

#include "afl/tmp/typelist.hpp"

#include "u/t_tmp.hpp"
#include "afl/base/staticassert.hpp"

/** Simple test. */
void
TestTmpTypeList::testIt()
{
    using afl::tmp::TypeListContains;
    using afl::tmp::TypeList;

    TS_ASSERT((TypeListContains<int, TypeList<char, TypeList<short, TypeList<int, TypeList<long, void> > > > >::result));
    TS_ASSERT((!TypeListContains<unsigned, TypeList<char, TypeList<short, TypeList<int, TypeList<long, void> > > > >::result));
    TS_ASSERT((!TypeListContains<int, void>::result));

    // Actually, a compile-time check:
    static_assert((TypeListContains<int, TypeList<char, TypeList<short, TypeList<int, TypeList<long, void> > > > >::result), "test");
}
