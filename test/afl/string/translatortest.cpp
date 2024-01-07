/**
  *  \file test/afl/string/translatortest.cpp
  *  \brief Test for afl::string::Translator
  */

#include "afl/string/translator.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.Translator", a)
{
    // Make sure a translator exists
    afl::string::Translator* tx = &afl::string::Translator::getSystemInstance();
    a.checkNonNull("system translator exists", tx);

    // Resetting it causes it to be created anew
    afl::string::Translator::setSystemInstance(std::auto_ptr<afl::string::Translator>(0));
    tx = &afl::string::Translator::getSystemInstance();
    a.checkNonNull("system translator is re-created", tx);

    // We know that the underlying instance is a NullTranslator, so exercise that:
    a.checkEqual("translate 01", tx->translateString         ("foo"),  "foo");
    a.checkEqual("translate 02", tx->translateString(String_t("foo")), "foo");
    a.checkEqual("translate 03", (*tx)                       ("foo"),  "foo");
    a.checkEqual("translate 04", (*tx)              (String_t("foo")), "foo");
}
