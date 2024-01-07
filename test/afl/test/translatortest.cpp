/**
  *  \file afl/test/translatortest.cpp
  *  \brief Test for afl::test::Translator
  */

#include "afl/test/translator.hpp"
#include "afl/test/testrunner.hpp"

/** Simple functionality test. */
AFL_TEST("afl.test.Translator", a) {
    a.checkEqual("default", afl::test::Translator().translateString("x"), "<x>");
    a.checkEqual("infix",   afl::test::Translator("+", "-").translateString("x"), "+x-");
}
