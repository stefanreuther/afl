/**
  *  \file test/afl/test/asserttest.cpp
  *  \brief Test for afl::test::Assert
  */

#include "afl/test/assert.hpp"
#include "afl/test/testrunner.hpp"
#include "afl/except/assertionfailedexception.hpp"

using afl::except::AssertionFailedException;
using afl::string::toMemory;
using afl::test::Assert;

// Success cases
AFL_TEST("afl.test.Assert:success", a) {
    Assert t("z");

    AFL_CHECK_SUCCEEDS(a("check"),                t.check("hi", true));
    AFL_CHECK_SUCCEEDS(a("checkEqual:int"),       t.checkEqual("hi", 9, 9));
    AFL_CHECK_SUCCEEDS(a("checkEqual:mixed-int"), t.checkEqual("hi", 9, 9L));
    AFL_CHECK_SUCCEEDS(a("checkEqual:mixed-str"), t.checkEqual("hi", "a", String_t("a")));
    AFL_CHECK_SUCCEEDS(a("checkEqualContent"),    t.checkEqualContent("hi", toMemory("axy"), toMemory("axy")));
}

// Error cases
AFL_TEST("afl.test.Assert:error", a) {
    Assert t("z");

    AFL_CHECK_THROWS(a("check"),                          t.check("hi", false),                                        AssertionFailedException);
    AFL_CHECK_THROWS(a("checkEqual:int"),                 t.checkEqual("hi", 1, 9),                                    AssertionFailedException);
    AFL_CHECK_THROWS(a("checkEqual:mixed-int"),           t.checkEqual("hi", 1, 9L),                                   AssertionFailedException);
    AFL_CHECK_THROWS(a("checkEqual:mixed-str"),           t.checkEqual("hi", "a", String_t("b")),                      AssertionFailedException);
    AFL_CHECK_THROWS(a("checkEqualContent:content-diff"), t.checkEqualContent("hi", toMemory("azy"), toMemory("axy")), AssertionFailedException);
    AFL_CHECK_THROWS(a("checkEqualContent:size-diff"),    t.checkEqualContent("hi", toMemory("hjk"), toMemory("hj")),  AssertionFailedException);
}

// Constructor variants
AFL_TEST("afl.test.Assert:ctor", a) {
    AFL_CHECK_THROWS(a("string"),        Assert(String_t("s1")).check("hi", false),                 AssertionFailedException);
    AFL_CHECK_THROWS(a("charp"),         Assert("s1").check("hi", false),                           AssertionFailedException);
    AFL_CHECK_THROWS(a("string+string"), Assert(String_t("s1"))(String_t("s2")).check("hi", false), AssertionFailedException);
    AFL_CHECK_THROWS(a("charp+charp"),   Assert("s1")("s2").check("hi", false),                     AssertionFailedException);
}

// Exception content
AFL_TEST("afl.test.Assert:exception-content", a) {
    try {
        Assert("xxx1")("xxx2").check("xxx3", false);
    }
    catch (AssertionFailedException& e) {
        a.checkDifferent("xxx1", String_t(e.what()).find("xxx1"), String_t::npos);
        a.checkDifferent("xxx2", String_t(e.what()).find("xxx2"), String_t::npos);
        a.checkDifferent("xxx3", String_t(e.what()).find("xxx3"), String_t::npos);
    }
}
