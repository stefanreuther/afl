/**
  *  \file u/t_string_nulltranslator.cpp
  *  \brief Test for afl::string::NullTranslator
  */

#include "afl/string/nulltranslator.hpp"

#include "u/t_string.hpp"

/** Simple test. */
void
TestStringNullTranslator::testIt()
{
    afl::string::NullTranslator tx;

    TS_ASSERT_EQUALS(tx.translateString("foo"), "foo");
    TS_ASSERT_EQUALS(tx.translateString(String_t("bar")), "bar");
    TS_ASSERT_EQUALS(tx.translate(afl::string::ConstStringMemory_t::unsafeCreate("baz", 3)), "baz");
}
