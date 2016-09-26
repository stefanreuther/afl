/**
  *  \file u/t_tmp_issametype.cpp
  *  \brief Test for afl::tmp::IsSameType
  */

#include "afl/tmp/issametype.hpp"

#include "u/t_tmp.hpp"
#include "afl/base/staticassert.hpp"

/** Simple test. */
void
TestTmpIsSameType::testIt()
{
    TS_ASSERT_EQUALS((afl::tmp::IsSameType<int,int>::result),            true);
    TS_ASSERT_EQUALS((afl::tmp::IsSameType<int,signed int>::result),     true);
    TS_ASSERT_EQUALS((afl::tmp::IsSameType<int,unsigned int>::result),   false);

    TS_ASSERT_EQUALS((afl::tmp::IsSameType<char,signed char>::result),   false);
    TS_ASSERT_EQUALS((afl::tmp::IsSameType<char,unsigned char>::result), false);

    TS_ASSERT_EQUALS((afl::tmp::IsSameType<void,void>::result),          true);
    TS_ASSERT_EQUALS((afl::tmp::IsSameType<long,long>::result),          true);

    // It's actually a compile-time constant:
    static_assert((afl::tmp::IsSameType<int,signed int>::result), "int/signed");
    static_assert(!(afl::tmp::IsSameType<int,unsigned int>::result), "int/unsigned");
}
