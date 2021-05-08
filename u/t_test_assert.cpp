/**
  *  \file u/t_test_assert.cpp
  *  \brief Test for afl::test::Assert
  */

#include "afl/test/assert.hpp"

#include "t_test.hpp"
#include "afl/except/assertionfailedexception.hpp"

void
TestTestAssert::testIt()
{
    afl::test::Assert t("z");

    // Success cases
    TS_ASSERT_THROWS_NOTHING(t.check("hi", true));
    TS_ASSERT_THROWS_NOTHING(t.checkEqual("hi", 9, 9));
    TS_ASSERT_THROWS_NOTHING(t.checkEqual("hi", 9, 9L));
    TS_ASSERT_THROWS_NOTHING(t.checkEqual("hi", "a", String_t("a")));
    TS_ASSERT_THROWS_NOTHING(t.checkEqualContent("hi", afl::string::toMemory("axy"), afl::string::toMemory("axy")));

    // Error cases
    TS_ASSERT_THROWS(t.check("hi", false), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(t.checkEqual("hi", 1, 9), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(t.checkEqual("hi", 1, 9L), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(t.checkEqual("hi", "a", String_t("b")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(t.checkEqualContent("hi", afl::string::toMemory("azy"), afl::string::toMemory("axy")), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(t.checkEqualContent("hi", afl::string::toMemory("hjk"), afl::string::toMemory("hj")), afl::except::AssertionFailedException);

    // Constructor variants
    TS_ASSERT_THROWS(afl::test::Assert(String_t("s1")).check("hi", false), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::test::Assert("s1").check("hi", false), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::test::Assert(String_t("s1"))(String_t("s2")).check("hi", false), afl::except::AssertionFailedException);
    TS_ASSERT_THROWS(afl::test::Assert("s1")("s2").check("hi", false), afl::except::AssertionFailedException);

    // Exception content
    try {
        afl::test::Assert("xxx1")("xxx2").check("xxx3", false);
    }
    catch (afl::except::AssertionFailedException& e) {
        TS_ASSERT_DIFFERS(String_t(e.what()).find("xxx1"), String_t::npos);
        TS_ASSERT_DIFFERS(String_t(e.what()).find("xxx2"), String_t::npos);
        TS_ASSERT_DIFFERS(String_t(e.what()).find("xxx3"), String_t::npos);
    }
}
