/**
  *  \file u/t_except_assertionfailedexception.cpp
  *  \brief Test for afl::except::AssertionFailedException
  */

#include "afl/except/assertionfailedexception.hpp"

#include "u/t_except.hpp"
#include <cstring>

/** Test AssertionFailedException. */
void
TestExceptAssertionFailedException::testClass()
{
    {
        afl::except::AssertionFailedException ex("TEXT", "");
        TS_ASSERT(std::strstr(ex.what(), "TEXT") != 0);
    }
    {
        afl::except::AssertionFailedException ex("MSG", "LOC");
        TS_ASSERT(std::strstr(ex.what(), "MSG") != 0);
        TS_ASSERT(std::strstr(ex.what(), "LOC") != 0);
    }
    {
        afl::except::AssertionFailedException ex("A", "B");
        afl::except::AssertionFailedException ex2(ex);
        std::runtime_error& err(ex);
        std::exception& x(ex);
        (void) err;
        (void) x;
    }
}

/** Test checkAssertion. */
void
TestExceptAssertionFailedException::testCheck()
{
    // No-throw case
    TS_ASSERT_THROWS_NOTHING(afl::except::checkAssertion(true, String_t("a"), String_t("b")));
    TS_ASSERT_THROWS_NOTHING(afl::except::checkAssertion(true,         ("a"), String_t("b")));
    TS_ASSERT_THROWS_NOTHING(afl::except::checkAssertion(true, String_t("a"),         ("b")));
    TS_ASSERT_THROWS_NOTHING(afl::except::checkAssertion(true,         ("a"),         ("b")));
    TS_ASSERT_THROWS_NOTHING(afl::except::checkAssertion(true, String_t("a")));
    TS_ASSERT_THROWS_NOTHING(afl::except::checkAssertion(true,         ("a")));

    // Throw case
    TS_ASSERT_THROWS(afl::except::checkAssertion(false, String_t("a"), String_t("b")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::except::checkAssertion(false,         ("a"), String_t("b")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::except::checkAssertion(false, String_t("a"),         ("b")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::except::checkAssertion(false,         ("a"),         ("b")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::except::checkAssertion(false, String_t("a")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::except::checkAssertion(false,         ("a")), afl::except::AssertionFailedException);
}
