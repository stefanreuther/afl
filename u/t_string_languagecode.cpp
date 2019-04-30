/**
  *  \file u/t_string_languagecode.cpp
  *  \brief Test for afl::string::LanguageCode
  */

#include "afl/string/languagecode.hpp"

#include "t_string.hpp"

/** Test the basics. */
void
TestStringLanguageCode::testBasic()
{
    using afl::string::LanguageCode;

    // isValid
    TS_ASSERT(!LanguageCode().isValid());
    TS_ASSERT(LanguageCode("x").isValid());

    // get, getWithDelimiter
    TS_ASSERT_EQUALS(LanguageCode("x").get(), "x");
    TS_ASSERT_EQUALS(LanguageCode("en-us").getWithDelimiter('_'), "en_us");

    // Comparisons
    LanguageCode a("a"), b("b");
    TS_ASSERT_EQUALS(a == b, false);
    TS_ASSERT_EQUALS(a == a, true);

    TS_ASSERT_EQUALS(a != b, true);
    TS_ASSERT_EQUALS(a != a, false);

    TS_ASSERT_EQUALS(a < b, true);
    TS_ASSERT_EQUALS(a < a, false);
    TS_ASSERT_EQUALS(b < a, false);

    TS_ASSERT_EQUALS(a <= b, true);
    TS_ASSERT_EQUALS(a <= a, true);
    TS_ASSERT_EQUALS(b <= a, false);

    TS_ASSERT_EQUALS(a > b, false);
    TS_ASSERT_EQUALS(a > a, false);
    TS_ASSERT_EQUALS(b > a, true);

    TS_ASSERT_EQUALS(a >= b, false);
    TS_ASSERT_EQUALS(a >= a, true);
    TS_ASSERT_EQUALS(b >= a, true);
}

/** Test generalize(). */
void
TestStringLanguageCode::testGeneralize()
{
    afl::string::LanguageCode t("de-DE@euro.utf-8");
    TS_ASSERT_EQUALS(t.get(), "de-DE@euro.utf-8");
    TS_ASSERT_EQUALS(t.generalize(), true);
    TS_ASSERT_EQUALS(t.get(), "de-DE@euro");
    TS_ASSERT_EQUALS(t.generalize(), true);
    TS_ASSERT_EQUALS(t.get(), "de-DE");
    TS_ASSERT_EQUALS(t.generalize(), true);
    TS_ASSERT_EQUALS(t.get(), "de");
    TS_ASSERT_EQUALS(t.generalize(), false);
    TS_ASSERT_EQUALS(t.get(), "de");

    t = String_t("x-klingon-a-q'onos");
    TS_ASSERT_EQUALS(t.generalize(), true);
    TS_ASSERT_EQUALS(t.get(), "x-klingon");
    TS_ASSERT_EQUALS(t.generalize(), false);
    TS_ASSERT_EQUALS(t.get(), "x-klingon");

    t = String_t();
    TS_ASSERT_EQUALS(t.generalize(), false);
}

/** Test isSameOrMoreGeneral(). */
void
TestStringLanguageCode::testIsSameOrMoreGeneral()
{
    using afl::string::LanguageCode;
    TS_ASSERT(LanguageCode("de").isSameOrMoreGeneral(LanguageCode("de-de")));
    TS_ASSERT(LanguageCode("de-de").isSameOrMoreGeneral(LanguageCode("de-de")));

    TS_ASSERT(!LanguageCode("de-de").isSameOrMoreGeneral(LanguageCode("de")));
    TS_ASSERT(!LanguageCode("deu").isSameOrMoreGeneral(LanguageCode("de")));
    TS_ASSERT(!LanguageCode("de-de").isSameOrMoreGeneral(LanguageCode("de-DE")));
    TS_ASSERT(!LanguageCode("de").isSameOrMoreGeneral(LanguageCode("")));
    TS_ASSERT(!LanguageCode("").isSameOrMoreGeneral(LanguageCode("de")));
}

