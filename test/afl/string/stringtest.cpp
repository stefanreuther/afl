/**
  *  \file test/afl/string/stringtest.cpp
  *  \brief Test for afl::string::String
  */

#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.String:toMemory", a)
{
    a.checkEqual("01", afl::string::toMemory("foo").size(), 3U);
    a.checkEqual("02", afl::string::toMemory("").size(), 0U);
    a.checkEqual("03", afl::string::toMemory(0).size(), 0U);

    const String_t cs("foo");
    a.checkEqual("11", afl::string::toMemory(cs).size(), 3U);
    a.checkEqual("12", afl::string::toMemory(String_t()).size(), 0U);

    a.checkEqual("21", afl::string::fromMemory(afl::string::toMemory(cs)), "foo");
    a.checkEqual("22", afl::string::fromMemory(afl::string::toMemory("foo")), "foo");

    a.checkEqual("31", afl::string::toMemoryLimited("foobar", 9).size(), 6U);
    a.checkEqual("32", afl::string::toMemoryLimited("foobar", 3).size(), 3U);

    a.checkEqual("41", afl::string::toMemoryLimited(0, 3).size(), 0U);
}

AFL_TEST("afl.string.strCaseCompare", a)
{
    // Test all signatures
    a.checkEqual("01", afl::string::strCaseCompare(afl::string::toMemory("foo"), afl::string::toMemory("foo")), 0);
    a.checkEqual("02", afl::string::strCaseCompare(String_t("foo"), String_t("foo")), 0);
    a.checkEqual("03", afl::string::strCaseCompare(String_t("foo"), "foo"), 0);
    a.checkEqual("04", afl::string::strCaseCompare("foo", String_t("foo")), 0);
    a.checkEqual("05", afl::string::strCaseCompare("foo", "foo"), 0);

    // Comparisons
    a.checkEqual("11", afl::string::strCaseCompare("foo", "FOO"), 0);
    a.checkEqual("12", afl::string::strCaseCompare("foo", "Foo"), 0);
    a.checkEqual("13", afl::string::strCaseCompare("foo", "FoO"), 0);
    a.checkEqual("14", afl::string::strCaseCompare("", ""), 0);

    a.check("21", afl::string::strCaseCompare("foo", "bar") > 0);
    a.check("22", afl::string::strCaseCompare("bar", "foo") < 0);

    a.check("31", afl::string::strCaseCompare("foo", "fon") > 0);
    a.check("32", afl::string::strCaseCompare("fon", "foo") < 0);

    a.check("41", afl::string::strCaseCompare("foo", "") > 0);
    a.check("42", afl::string::strCaseCompare("", "foo") < 0);

    a.check("51", afl::string::strCaseCompare("foo", "fo") > 0);
    a.check("52", afl::string::strCaseCompare("fo", "foo") < 0);
}

AFL_TEST("afl.string.strLTrim", a)
{
    a.checkEqual("01", afl::string::strLTrim(""), "");
    a.checkEqual("02", afl::string::strLTrim("     "), "");
    a.checkEqual("03", afl::string::strLTrim("  \t "), "");
    a.checkEqual("04", afl::string::strLTrim("foo"), "foo");
    a.checkEqual("05", afl::string::strLTrim("   foo"), "foo");
    a.checkEqual("06", afl::string::strLTrim("\t\nfoo"), "foo");
    a.checkEqual("07", afl::string::strLTrim("foo   "), "foo   ");
    a.checkEqual("08", afl::string::strLTrim("foo\tbar"), "foo\tbar");
    a.checkEqual("09", afl::string::strLTrim("   foo\tbar  "), "foo\tbar  ");
}

AFL_TEST("afl.string.strRTrim", a)
{
    a.checkEqual("01", afl::string::strRTrim(""), "");
    a.checkEqual("02", afl::string::strRTrim("     "), "");
    a.checkEqual("03", afl::string::strRTrim("  \t "), "");
    a.checkEqual("04", afl::string::strRTrim("foo"), "foo");
    a.checkEqual("05", afl::string::strRTrim("   foo"), "   foo");
    a.checkEqual("06", afl::string::strRTrim("\t\nfoo"), "\t\nfoo");
    a.checkEqual("07", afl::string::strRTrim("foo   "), "foo");
    a.checkEqual("08", afl::string::strRTrim("foo\tbar"), "foo\tbar");
    a.checkEqual("09", afl::string::strRTrim("   foo\tbar  "), "   foo\tbar");
}

AFL_TEST("afl.string.strTrim", a)
{
    a.checkEqual("01", afl::string::strTrim(""), "");
    a.checkEqual("02", afl::string::strTrim("     "), "");
    a.checkEqual("03", afl::string::strTrim("  \t "), "");
    a.checkEqual("04", afl::string::strTrim("foo"), "foo");
    a.checkEqual("05", afl::string::strTrim("   foo"), "foo");
    a.checkEqual("06", afl::string::strTrim("\t\nfoo"), "foo");
    a.checkEqual("07", afl::string::strTrim("foo   "), "foo");
    a.checkEqual("08", afl::string::strTrim("foo\tbar"), "foo\tbar");
    a.checkEqual("09", afl::string::strTrim("   foo\tbar  "), "foo\tbar");
}

/*
 *  Parsing (strFirst, strRemove, strSplit)
 */

AFL_TEST("afl.string.String:parsing", a)
{
    // Basic parsing
    String_t sentence = "The quick brown fox";
    a.checkEqual("01", afl::string::strFirst(sentence, " "), "The");
    a.check("02", afl::string::strRemove(sentence, " "));

    a.checkEqual("11", afl::string::strFirst(sentence, " "), "quick");
    a.check("12", afl::string::strRemove(sentence, " "));

    a.checkEqual("21", afl::string::strFirst(sentence, " "), "brown");
    a.check("22", afl::string::strRemove(sentence, " "));

    a.checkEqual("31", afl::string::strFirst(sentence, " "), "fox");
    a.check("32", !afl::string::strRemove(sentence, " "));
    a.check("33", !afl::string::strRemove(sentence, " "));
    a.check("34", !afl::string::strRemove(sentence, " "));
}

AFL_TEST("afl.string.String:parsing:2", a)
{
    // This is strstr, not strspn!
    String_t sentence = "a-b+c+-d-e";
    a.checkEqual("01", afl::string::strFirst(sentence, "+-"), "a-b+c");
    a.check("02", afl::string::strRemove(sentence, "+-"));
    a.check("03", !afl::string::strRemove(sentence, "+-"));
}

AFL_TEST("afl.string.String:parsing:3", a)
{
    // Splitting
    String_t sentence = "a-b+c+-d-e";
    String_t sa, sb;
    a.check("01", afl::string::strSplit(sentence, sa, sb, "+-"));
    a.checkEqual("02", sentence, "a-b+c+-d-e");
    a.checkEqual("03", sa, "a-b+c");
    a.checkEqual("04", sb, "d-e");

    // Splitting with mismatch: no change to a/b
    sa = "?";
    sb = "?";
    a.check("11", !afl::string::strSplit(sentence, sa, sb, "zz"));
    a.checkEqual("12", sentence, "a-b+c+-d-e");
    a.checkEqual("13", sa, "?");
    a.checkEqual("14", sb, "?");
}

AFL_TEST("afl.string.String:parsing:4", a)
{
    // Check aliasing (this once was a problem)
    String_t sentence = "abc";
    String_t sb;
    a.check("01", afl::string::strSplit(sentence, sentence, sb, "b"));
    a.checkEqual("02", sentence, "a");
    a.checkEqual("03", sb, "c");
}

AFL_TEST("afl.string.String:parsing:5", a)
{
    // Check aliasing (this once was a problem)
    String_t sentence = "abc";
    String_t sa;
    a.check("01", afl::string::strSplit(sentence, sa, sentence, "b"));
    a.checkEqual("02", sa, "a");
    a.checkEqual("03", sentence, "c");
}

AFL_TEST("afl.string.String:parsing:6", a)
{
    String_t sentence = "abc";
    String_t sa, sb;
    a.check("01", afl::string::strSplit(sentence, sa, sb, sentence));
    a.checkEqual("02", sa, "");
    a.checkEqual("03", sb, "");
}

/*
 *  Case conversion
 */

AFL_TEST("afl.string.strUCase", a)
{
    // Upper case
    a.checkEqual("01", afl::string::strUCase(""), "");
    a.checkEqual("02", afl::string::strUCase(" "), " ");
    a.checkEqual("03", afl::string::strUCase("bla"), "BLA");
    a.checkEqual("04", afl::string::strUCase("Bla Bla-bla"), "BLA BLA-BLA");
    a.checkEqual("05", afl::string::strUCase("BLAFASEL"), "BLAFASEL");
    a.checkEqual("06", afl::string::strUCase("BL\xC4H"), "BL\xC4H");
    a.checkEqual("07", afl::string::strUCase("bl\xE4h"), "BL\xE4H");
}

AFL_TEST("afl.string.strLCase", a)
{
    // Lower case
    a.checkEqual("01", afl::string::strLCase(""), "");
    a.checkEqual("02", afl::string::strLCase(" "), " ");
    a.checkEqual("03", afl::string::strLCase("bla"), "bla");
    a.checkEqual("04", afl::string::strLCase("Bla Bla-bla"), "bla bla-bla");
    a.checkEqual("05", afl::string::strLCase("BLAFASEL"), "blafasel");
    a.checkEqual("06", afl::string::strLCase("BL\xC4H"), "bl\xC4h");
    a.checkEqual("07", afl::string::strLCase("bl\xE4h"), "bl\xE4h");
}

AFL_TEST("afl.string.strUCFirst", a)
{
    // Word case
    a.checkEqual("01", afl::string::strUCFirst(""), "");
    a.checkEqual("02", afl::string::strUCFirst(" "), " ");
    a.checkEqual("03", afl::string::strUCFirst("bla"), "Bla");
    a.checkEqual("04", afl::string::strUCFirst("Bla Bla-bla"), "Bla Bla-Bla");
    a.checkEqual("05", afl::string::strUCFirst("BLAFASEL"), "Blafasel");
    a.checkEqual("06", afl::string::strUCFirst("BL\xC4H"), "Bl\xC4h");
    a.checkEqual("07", afl::string::strUCFirst("bl\xE4h"), "Bl\xE4h");
}

AFL_TEST("afl.string.strLCWords", a)
{
    // Lower-case words
    a.checkEqual("01", afl::string::strLCWords(""), "");
    a.checkEqual("02", afl::string::strLCWords(" "), " ");
    a.checkEqual("03", afl::string::strLCWords("bla"), "bla");
    a.checkEqual("04", afl::string::strLCWords("Bla Bla-bla"), "Bla Bla-bla");
    a.checkEqual("05", afl::string::strLCWords("BLAFASEL"), "Blafasel");
    a.checkEqual("06", afl::string::strLCWords("BL\xC4H"), "Bl\xC4h");
    a.checkEqual("07", afl::string::strLCWords("bl\xE4h"), "bl\xE4h");
}

/*
 *  Word splitting
 */

AFL_TEST("afl.string.strNthWord", a)
{
    // Simple usage
    const String_t sentence = "The quick brown fox";
    a.checkEqual("01", afl::string::strNthWord(sentence, 0), "The");
    a.checkEqual("02", afl::string::strNthWord(sentence, 1), "quick");
    a.checkEqual("03", afl::string::strNthWord(sentence, 2), "brown");
    a.checkEqual("04", afl::string::strNthWord(sentence, 3), "fox");
    a.checkEqual("05", afl::string::strNthWord(sentence, 4), "");

    a.checkEqual("11", afl::string::strNthWordRest(sentence, 0), "The quick brown fox");
    a.checkEqual("12", afl::string::strNthWordRest(sentence, 1), "quick brown fox");
    a.checkEqual("13", afl::string::strNthWordRest(sentence, 2), "brown fox");
    a.checkEqual("14", afl::string::strNthWordRest(sentence, 3), "fox");
    a.checkEqual("15", afl::string::strNthWordRest(sentence, 4), "");
}

AFL_TEST("afl.string.strNthWord:blank", a)
{
    // Empty
    const String_t sentence = "  \n ";
    a.checkEqual("01", afl::string::strNthWord(sentence, 0), "");
    a.checkEqual("02", afl::string::strNthWord(sentence, 1), "");
    a.checkEqual("03", afl::string::strNthWord(sentence, 2), "");
    a.checkEqual("04", afl::string::strNthWord(sentence, 3), "");

    a.checkEqual("11", afl::string::strNthWordRest(sentence, 0), "");
    a.checkEqual("12", afl::string::strNthWordRest(sentence, 1), "");
    a.checkEqual("13", afl::string::strNthWordRest(sentence, 2), "");
    a.checkEqual("14", afl::string::strNthWordRest(sentence, 3), "");
}

AFL_TEST("afl.string.strNthWord:empty", a)
{
    // Really empty
    const String_t sentence = "";
    a.checkEqual("01", afl::string::strNthWord(sentence, 0), "");
    a.checkEqual("02", afl::string::strNthWord(sentence, 1), "");

    a.checkEqual("11", afl::string::strNthWordRest(sentence, 0), "");
    a.checkEqual("12", afl::string::strNthWordRest(sentence, 1), "");
}

AFL_TEST("afl.string.strNthWord:space", a)
{
    // Space around
    const String_t sentence = "  hi there!\n";
    a.checkEqual("01", afl::string::strNthWord(sentence, 0), "hi");
    a.checkEqual("02", afl::string::strNthWord(sentence, 1), "there!");
    a.checkEqual("03", afl::string::strNthWord(sentence, 2), "");

    a.checkEqual("11", afl::string::strNthWordRest(sentence, 0), "hi there!\n");
    a.checkEqual("12", afl::string::strNthWordRest(sentence, 1), "there!\n");
    a.checkEqual("13", afl::string::strNthWordRest(sentence, 2), "");
}
