/**
  *  \file u/t_test_loglistener.cpp
  *  \brief Test for afl::test::LogListener
  */

#include "afl/test/loglistener.hpp"

#include "t_test.hpp"

void
TestTestLogListener::testIt()
{
    afl::test::LogListener t;
    TS_ASSERT_EQUALS(t.getNumMessages(), 0U);
    TS_ASSERT_EQUALS(t.getNumErrors(), 0U);
    TS_ASSERT_EQUALS(t.getNumWarnings(), 0U);

    t.write(afl::sys::LogListener::Info, "ch", "text");
    t.write(afl::sys::LogListener::Warn, "ch", "text");
    t.write(afl::sys::LogListener::Error, "ch", "text");
    t.write(afl::sys::LogListener::Info, "ch", "text");
    t.write(afl::sys::LogListener::Warn, "ch", "text");

    TS_ASSERT_EQUALS(t.getNumMessages(), 5U);
    TS_ASSERT_EQUALS(t.getNumErrors(), 1U);
    TS_ASSERT_EQUALS(t.getNumWarnings(), 2U);

    t.clear();
    TS_ASSERT_EQUALS(t.getNumMessages(), 0U);
    TS_ASSERT_EQUALS(t.getNumErrors(), 0U);
    TS_ASSERT_EQUALS(t.getNumWarnings(), 0U);
}

