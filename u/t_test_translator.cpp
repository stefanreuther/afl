/**
  *  \file u/t_test_translator.cpp
  *  \brief Test for afl::test::Translator
  */

#include "afl/test/translator.hpp"

#include "t_test.hpp"

/** Simple functionality test. */
void
TestTestTranslator::testIt()
{
    TS_ASSERT_EQUALS(afl::test::Translator().translateString("x"), "<x>");
    TS_ASSERT_EQUALS(afl::test::Translator("+", "-").translateString("x"), "+x-");
}

