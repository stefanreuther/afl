/**
  *  \file test/afl/string/nulltranslatortest.cpp
  *  \brief Test for afl::string::NullTranslator
  */

#include "afl/string/nulltranslator.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.NullTranslator", a)
{
    afl::string::NullTranslator tx;

    a.checkEqual("charp", tx.translateString("foo"), "foo");
    a.checkEqual("str", tx.translateString(String_t("bar")), "bar");
    a.checkEqual("mem", tx.translate(afl::string::ConstStringMemory_t::unsafeCreate("baz", 3)), "baz");
}
