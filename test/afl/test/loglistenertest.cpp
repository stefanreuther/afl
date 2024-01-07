/**
  *  \file test/afl/test/loglistenertest.cpp
  *  \brief Test for afl::test::LogListener
  */

#include "afl/test/loglistener.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.test.LogListener", a) {
    afl::test::LogListener t;
    a.checkEqual("msg1",  t.getNumMessages(), 0U);
    a.checkEqual("err1",  t.getNumErrors(), 0U);
    a.checkEqual("warn2", t.getNumWarnings(), 0U);

    t.write(afl::sys::LogListener::Info, "ch", "text");
    t.write(afl::sys::LogListener::Warn, "ch", "text");
    t.write(afl::sys::LogListener::Error, "ch", "text");
    t.write(afl::sys::LogListener::Info, "ch", "text");
    t.write(afl::sys::LogListener::Warn, "ch", "text");

    a.checkEqual("msg2",  t.getNumMessages(), 5U);
    a.checkEqual("err2",  t.getNumErrors(), 1U);
    a.checkEqual("warn2", t.getNumWarnings(), 2U);

    t.clear();
    a.checkEqual("msg3",  t.getNumMessages(), 0U);
    a.checkEqual("err3",  t.getNumErrors(), 0U);
    a.checkEqual("warn3", t.getNumWarnings(), 0U);
}

