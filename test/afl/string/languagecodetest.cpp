/**
  *  \file test/afl/string/languagecodetest.cpp
  *  \brief Test for afl::string::LanguageCode
  */

#include "afl/string/languagecode.hpp"
#include "afl/test/testrunner.hpp"

/** Test the basics. */
AFL_TEST("afl.string.LanguageCode:basic", a)
{
    using afl::string::LanguageCode;

    // isValid
    a.check("01. isValid", !LanguageCode().isValid());
    a.check("02. isValid", LanguageCode("x").isValid());

    // get, getWithDelimiter
    a.checkEqual("11. get", LanguageCode("x").get(), "x");
    a.checkEqual("12. getWithDelimiter", LanguageCode("en-us").getWithDelimiter('_'), "en_us");

    // Comparisons
    LanguageCode aa("a"), bb("b");
    a.checkEqual("21. eq", aa == bb, false);
    a.checkEqual("22. eq", aa == aa, true);

    a.checkEqual("31. ne", aa != bb, true);
    a.checkEqual("32. ne", aa != aa, false);

    a.checkEqual("41. lt", aa < bb, true);
    a.checkEqual("42. lt", aa < aa, false);
    a.checkEqual("43. lt", bb < aa, false);

    a.checkEqual("51. le", aa <= bb, true);
    a.checkEqual("52. le", aa <= aa, true);
    a.checkEqual("53. le", bb <= aa, false);

    a.checkEqual("61. gt", aa > bb, false);
    a.checkEqual("62. gt", aa > aa, false);
    a.checkEqual("63. gt", bb > aa, true);

    a.checkEqual("71. ge", aa >= bb, false);
    a.checkEqual("72. ge", aa >= aa, true);
    a.checkEqual("73. ge", bb >= aa, true);
}

/** Test generalize(). */
AFL_TEST("afl.string.LanguageCode:generalize", a)
{
    afl::string::LanguageCode t("de-DE@euro.utf-8");
    a.checkEqual("01", t.get(), "de-DE@euro.utf-8");
    a.checkEqual("02", t.generalize(), true);
    a.checkEqual("03", t.get(), "de-DE@euro");
    a.checkEqual("04", t.generalize(), true);
    a.checkEqual("05", t.get(), "de-DE");
    a.checkEqual("06", t.generalize(), true);
    a.checkEqual("07", t.get(), "de");
    a.checkEqual("08", t.generalize(), false);
    a.checkEqual("09", t.get(), "de");

    t = String_t("x-klingon-a-q'onos");
    a.checkEqual("11", t.generalize(), true);
    a.checkEqual("12", t.get(), "x-klingon");
    a.checkEqual("13", t.generalize(), false);
    a.checkEqual("14", t.get(), "x-klingon");

    t = String_t();
    a.checkEqual("21", t.generalize(), false);
}

/** Test isSameOrMoreGeneral(). */
AFL_TEST("afl.string.LanguageCode:isSameOrMoreGeneral", a)
{
    using afl::string::LanguageCode;
    a.check("01", LanguageCode("de").isSameOrMoreGeneral(LanguageCode("de-de")));
    a.check("02", LanguageCode("de-de").isSameOrMoreGeneral(LanguageCode("de-de")));

    a.check("03", !LanguageCode("de-de").isSameOrMoreGeneral(LanguageCode("de")));
    a.check("04", !LanguageCode("deu").isSameOrMoreGeneral(LanguageCode("de")));
    a.check("05", !LanguageCode("de-de").isSameOrMoreGeneral(LanguageCode("de-DE")));
    a.check("06", !LanguageCode("de").isSameOrMoreGeneral(LanguageCode("")));
    a.check("07", !LanguageCode("").isSameOrMoreGeneral(LanguageCode("de")));
}
