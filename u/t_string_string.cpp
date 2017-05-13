/**
  *  \file u/t_string_string.cpp
  *  \brief Test for afl/string/string.hpp
  */

#include "u/t_string.hpp"
#include "afl/string/string.hpp"

/** Test conversions to and from Memory. */
void
TestStringString::testMemory()
{
    TS_ASSERT_EQUALS(afl::string::toMemory("foo").size(), 3U);
    TS_ASSERT_EQUALS(afl::string::toMemory("").size(), 0U);
    TS_ASSERT_EQUALS(afl::string::toMemory(0).size(), 0U);

    const String_t cs("foo");
    TS_ASSERT_EQUALS(afl::string::toMemory(cs).size(), 3U);
    TS_ASSERT_EQUALS(afl::string::toMemory(String_t()).size(), 0U);

    TS_ASSERT_EQUALS(afl::string::fromMemory(afl::string::toMemory(cs)), "foo");
    TS_ASSERT_EQUALS(afl::string::fromMemory(afl::string::toMemory("foo")), "foo");

    TS_ASSERT_EQUALS(afl::string::toMemoryLimited("foobar", 9).size(), 6U);
    TS_ASSERT_EQUALS(afl::string::toMemoryLimited("foobar", 3).size(), 3U);

    TS_ASSERT_EQUALS(afl::string::toMemoryLimited(0, 3).size(), 0U);
}

/** Test strCaseCompare. */
void
TestStringString::testCompare()
{
    // Test all signatures
    TS_ASSERT_EQUALS(afl::string::strCaseCompare(afl::string::toMemory("foo"), afl::string::toMemory("foo")), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare(String_t("foo"), String_t("foo")), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare(String_t("foo"), "foo"), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare("foo", String_t("foo")), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare("foo", "foo"), 0);

    // Comparisons
    TS_ASSERT_EQUALS(afl::string::strCaseCompare("foo", "FOO"), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare("foo", "Foo"), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare("foo", "FoO"), 0);
    TS_ASSERT_EQUALS(afl::string::strCaseCompare("", ""), 0);

    TS_ASSERT(afl::string::strCaseCompare("foo", "bar") > 0);
    TS_ASSERT(afl::string::strCaseCompare("bar", "foo") < 0);

    TS_ASSERT(afl::string::strCaseCompare("foo", "fon") > 0);
    TS_ASSERT(afl::string::strCaseCompare("fon", "foo") < 0);

    TS_ASSERT(afl::string::strCaseCompare("foo", "") > 0);
    TS_ASSERT(afl::string::strCaseCompare("", "foo") < 0);

    TS_ASSERT(afl::string::strCaseCompare("foo", "fo") > 0);
    TS_ASSERT(afl::string::strCaseCompare("fo", "foo") < 0);
}

/** Test trim functions. */
void
TestStringString::testTrim()
{
    TS_ASSERT_EQUALS(afl::string::strLTrim(""), "");
    TS_ASSERT_EQUALS(afl::string::strLTrim("     "), "");
    TS_ASSERT_EQUALS(afl::string::strLTrim("  \t "), "");
    TS_ASSERT_EQUALS(afl::string::strLTrim("foo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strLTrim("   foo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strLTrim("\t\nfoo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strLTrim("foo   "), "foo   ");
    TS_ASSERT_EQUALS(afl::string::strLTrim("foo\tbar"), "foo\tbar");
    TS_ASSERT_EQUALS(afl::string::strLTrim("   foo\tbar  "), "foo\tbar  ");

    TS_ASSERT_EQUALS(afl::string::strRTrim(""), "");
    TS_ASSERT_EQUALS(afl::string::strRTrim("     "), "");
    TS_ASSERT_EQUALS(afl::string::strRTrim("  \t "), "");
    TS_ASSERT_EQUALS(afl::string::strRTrim("foo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strRTrim("   foo"), "   foo");
    TS_ASSERT_EQUALS(afl::string::strRTrim("\t\nfoo"), "\t\nfoo");
    TS_ASSERT_EQUALS(afl::string::strRTrim("foo   "), "foo");
    TS_ASSERT_EQUALS(afl::string::strRTrim("foo\tbar"), "foo\tbar");
    TS_ASSERT_EQUALS(afl::string::strRTrim("   foo\tbar  "), "   foo\tbar");

    TS_ASSERT_EQUALS(afl::string::strTrim(""), "");
    TS_ASSERT_EQUALS(afl::string::strTrim("     "), "");
    TS_ASSERT_EQUALS(afl::string::strTrim("  \t "), "");
    TS_ASSERT_EQUALS(afl::string::strTrim("foo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strTrim("   foo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strTrim("\t\nfoo"), "foo");
    TS_ASSERT_EQUALS(afl::string::strTrim("foo   "), "foo");
    TS_ASSERT_EQUALS(afl::string::strTrim("foo\tbar"), "foo\tbar");
    TS_ASSERT_EQUALS(afl::string::strTrim("   foo\tbar  "), "foo\tbar");
}

/** Test parse functions. */
void
TestStringString::testParse()
{
    // Basic parsing
    String_t sentence = "The quick brown fox";
    TS_ASSERT_EQUALS(afl::string::strFirst(sentence, " "), "The");
    TS_ASSERT(afl::string::strRemove(sentence, " "));

    TS_ASSERT_EQUALS(afl::string::strFirst(sentence, " "), "quick");
    TS_ASSERT(afl::string::strRemove(sentence, " "));

    TS_ASSERT_EQUALS(afl::string::strFirst(sentence, " "), "brown");
    TS_ASSERT(afl::string::strRemove(sentence, " "));

    TS_ASSERT_EQUALS(afl::string::strFirst(sentence, " "), "fox");
    TS_ASSERT(!afl::string::strRemove(sentence, " "));
    TS_ASSERT(!afl::string::strRemove(sentence, " "));
    TS_ASSERT(!afl::string::strRemove(sentence, " "));

    // This is strstr, not strspn!
    sentence = "a-b+c+-d-e";
    TS_ASSERT_EQUALS(afl::string::strFirst(sentence, "+-"), "a-b+c");
    TS_ASSERT(afl::string::strRemove(sentence, "+-"));
    TS_ASSERT(!afl::string::strRemove(sentence, "+-"));

    // Splitting
    sentence = "a-b+c+-d-e";
    String_t a, b;
    TS_ASSERT(afl::string::strSplit(sentence, a, b, "+-"));
    TS_ASSERT_EQUALS(sentence, "a-b+c+-d-e");
    TS_ASSERT_EQUALS(a, "a-b+c");
    TS_ASSERT_EQUALS(b, "d-e");

    // Splitting with mismatch: no change to a/b
    a = "?";
    b = "?";
    TS_ASSERT(!afl::string::strSplit(sentence, a, b, "zz"));
    TS_ASSERT_EQUALS(sentence, "a-b+c+-d-e");
    TS_ASSERT_EQUALS(a, "?");
    TS_ASSERT_EQUALS(b, "?");

    // Check aliasing (this once was a problem)
    sentence = "abc";
    TS_ASSERT(afl::string::strSplit(sentence, sentence, b, "b"));
    TS_ASSERT_EQUALS(sentence, "a");
    TS_ASSERT_EQUALS(b, "c");

    sentence = "abc";
    TS_ASSERT(afl::string::strSplit(sentence, a, sentence, "b"));
    TS_ASSERT_EQUALS(a, "a");
    TS_ASSERT_EQUALS(sentence, "c");

    sentence = "abc";
    TS_ASSERT(afl::string::strSplit(sentence, a, b, sentence));
    TS_ASSERT_EQUALS(a, "");
    TS_ASSERT_EQUALS(b, "");
}

/** Test case conversion. */
void
TestStringString::testCase()
{
    // Upper case
    TS_ASSERT_EQUALS(afl::string::strUCase(""), "");
    TS_ASSERT_EQUALS(afl::string::strUCase(" "), " ");
    TS_ASSERT_EQUALS(afl::string::strUCase("bla"), "BLA");
    TS_ASSERT_EQUALS(afl::string::strUCase("Bla Bla-bla"), "BLA BLA-BLA");
    TS_ASSERT_EQUALS(afl::string::strUCase("BLAFASEL"), "BLAFASEL");
    TS_ASSERT_EQUALS(afl::string::strUCase("BL\xC4H"), "BL\xC4H");
    TS_ASSERT_EQUALS(afl::string::strUCase("bl\xE4h"), "BL\xE4H");

    // Lower case
    TS_ASSERT_EQUALS(afl::string::strLCase(""), "");
    TS_ASSERT_EQUALS(afl::string::strLCase(" "), " ");
    TS_ASSERT_EQUALS(afl::string::strLCase("bla"), "bla");
    TS_ASSERT_EQUALS(afl::string::strLCase("Bla Bla-bla"), "bla bla-bla");
    TS_ASSERT_EQUALS(afl::string::strLCase("BLAFASEL"), "blafasel");
    TS_ASSERT_EQUALS(afl::string::strLCase("BL\xC4H"), "bl\xC4h");
    TS_ASSERT_EQUALS(afl::string::strLCase("bl\xE4h"), "bl\xE4h");

    // Word case
    TS_ASSERT_EQUALS(afl::string::strUCFirst(""), "");
    TS_ASSERT_EQUALS(afl::string::strUCFirst(" "), " ");
    TS_ASSERT_EQUALS(afl::string::strUCFirst("bla"), "Bla");
    TS_ASSERT_EQUALS(afl::string::strUCFirst("Bla Bla-bla"), "Bla Bla-Bla");
    TS_ASSERT_EQUALS(afl::string::strUCFirst("BLAFASEL"), "Blafasel");
    TS_ASSERT_EQUALS(afl::string::strUCFirst("BL\xC4H"), "Bl\xC4h");
    TS_ASSERT_EQUALS(afl::string::strUCFirst("bl\xE4h"), "Bl\xE4h");

    // Lower-case words
    TS_ASSERT_EQUALS(afl::string::strLCWords(""), "");
    TS_ASSERT_EQUALS(afl::string::strLCWords(" "), " ");
    TS_ASSERT_EQUALS(afl::string::strLCWords("bla"), "bla");
    TS_ASSERT_EQUALS(afl::string::strLCWords("Bla Bla-bla"), "Bla Bla-bla");
    TS_ASSERT_EQUALS(afl::string::strLCWords("BLAFASEL"), "Blafasel");
    TS_ASSERT_EQUALS(afl::string::strLCWords("BL\xC4H"), "Bl\xC4h");
    TS_ASSERT_EQUALS(afl::string::strLCWords("bl\xE4h"), "bl\xE4h");
}

/** Test word splitting. */
void
TestStringString::testWord()
{
    // Simple usage
    String_t sentence = "The quick brown fox";
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 0), "The");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 1), "quick");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 2), "brown");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 3), "fox");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 4), "");

    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 0), "The quick brown fox");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 1), "quick brown fox");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 2), "brown fox");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 3), "fox");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 4), "");

    // Empty
    sentence = "  \n ";
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 0), "");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 1), "");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 2), "");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 3), "");

    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 0), "");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 1), "");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 2), "");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 3), "");

    // Really empty
    sentence = "";
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 0), "");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 1), "");

    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 0), "");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 1), "");

    // Space around
    sentence = "  hi there!\n";
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 0), "hi");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 1), "there!");
    TS_ASSERT_EQUALS(afl::string::strNthWord(sentence, 2), "");

    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 0), "hi there!\n");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 1), "there!\n");
    TS_ASSERT_EQUALS(afl::string::strNthWordRest(sentence, 2), "");
}
