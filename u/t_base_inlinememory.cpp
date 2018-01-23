/**
  *  \file u/t_base_inlinememory.cpp
  *  \brief Test for afl::base::InlineMemory
  */

#include "afl/base/inlinememory.hpp"

#include "t_base.hpp"

/** Simple tests. */
void
TestBaseInlineMemory::testIt()
{
    afl::base::InlineMemory<uint8_t,7> a;
    afl::base::InlineMemory<uint8_t,3> b;

    // Initial state
    TS_ASSERT_EQUALS(a.size(), 7U);
    TS_ASSERT_EQUALS(b.size(), 3U);
    a.fill(1);
    b.fill(2);
    TS_ASSERT_EQUALS(*a.at(0), 1);
    TS_ASSERT_EQUALS(*b.at(0), 2);

    // Copying copies the pointers
    afl::base::Memory<uint8_t> aa = a;
    a = b;
    TS_ASSERT_EQUALS(a.at(0), b.at(0));
    TS_ASSERT_DIFFERS(a.at(0), aa.at(0));
    TS_ASSERT_EQUALS(*a.at(0), 2);
    TS_ASSERT_EQUALS(*aa.at(0), 1);

    // reset clears
    a.reset();
    TS_ASSERT_EQUALS(a.size(), 0U);
}

