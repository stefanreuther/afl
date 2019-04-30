/**
  *  \file u/t_string_translator.cpp
  *  \brief Test for afl::string::Translator
  */

#include "afl/string/translator.hpp"

#include "u/t_string.hpp"

/** Simple test. */
void
TestStringTranslator::testIt()
{
    // Make sure a translator exists
    afl::string::Translator* tx = &afl::string::Translator::getSystemInstance();
    TS_ASSERT(tx != 0);

    // Resetting it causes it to be created anew
    afl::string::Translator::setSystemInstance(std::auto_ptr<afl::string::Translator>(0));
    tx = &afl::string::Translator::getSystemInstance();
    TS_ASSERT(tx != 0);

    // We know that the underlying instance is a NullTranslator, so exercise that:
    TS_ASSERT_EQUALS(tx->translateString         ("foo"),  "foo");
    TS_ASSERT_EQUALS(tx->translateString(String_t("foo")), "foo");
    TS_ASSERT_EQUALS((*tx)                       ("foo"),  "foo");
    TS_ASSERT_EQUALS((*tx)              (String_t("foo")), "foo");
}
