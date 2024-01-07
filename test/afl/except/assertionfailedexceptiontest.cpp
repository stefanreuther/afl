/**
  *  \file test/afl/except/assertionfailedexceptiontest.cpp
  *  \brief Test for afl::except::AssertionFailedException
  */

#include "afl/except/assertionfailedexception.hpp"

#include <cstring>
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.except.AssertionFailedException:create:one", a)
{
    afl::except::AssertionFailedException ex("TEXT", "");
    a.check("", std::strstr(ex.what(), "TEXT") != 0);
}


AFL_TEST("afl.except.AssertionFailedException:create:two", a)
{
    afl::except::AssertionFailedException ex("MSG", "LOC");
    a.check("first", std::strstr(ex.what(), "MSG") != 0);
    a.check("second", std::strstr(ex.what(), "LOC") != 0);
}

AFL_TEST_NOARG("afl.except.AssertionFailedException:convert")
{
    afl::except::AssertionFailedException ex("A", "B");
    afl::except::AssertionFailedException ex2(ex);
    std::runtime_error& err(ex);
    std::exception& x(ex);
    (void) err;
    (void) x;
}

/** Test checkAssertion. */
AFL_TEST("afl.except.AssertionFailedException:checkAssertion", a)
{
    // No-throw case
    AFL_CHECK_SUCCEEDS(a("no 1"), afl::except::checkAssertion(true, String_t("a"), String_t("b")));
    AFL_CHECK_SUCCEEDS(a("no 2"), afl::except::checkAssertion(true,         ("a"), String_t("b")));
    AFL_CHECK_SUCCEEDS(a("no 3"), afl::except::checkAssertion(true, String_t("a"),         ("b")));
    AFL_CHECK_SUCCEEDS(a("no 4"), afl::except::checkAssertion(true,         ("a"),         ("b")));
    AFL_CHECK_SUCCEEDS(a("no 5"), afl::except::checkAssertion(true, String_t("a")));
    AFL_CHECK_SUCCEEDS(a("no 6"), afl::except::checkAssertion(true,         ("a")));

    // Throw case
    AFL_CHECK_THROWS(a("throw 1"), afl::except::checkAssertion(false, String_t("a"), String_t("b")), afl::except::AssertionFailedException);
    AFL_CHECK_THROWS(a("throw 2"), afl::except::checkAssertion(false,         ("a"), String_t("b")), afl::except::AssertionFailedException);
    AFL_CHECK_THROWS(a("throw 3"), afl::except::checkAssertion(false, String_t("a"),         ("b")), afl::except::AssertionFailedException);
    AFL_CHECK_THROWS(a("throw 4"), afl::except::checkAssertion(false,         ("a"),         ("b")), afl::except::AssertionFailedException);
    AFL_CHECK_THROWS(a("throw 5"), afl::except::checkAssertion(false, String_t("a")), afl::except::AssertionFailedException);
    AFL_CHECK_THROWS(a("throw 6"), afl::except::checkAssertion(false,         ("a")), afl::except::AssertionFailedException);
}
