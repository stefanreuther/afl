/**
  *  \file test/afl/string/chartest.cpp
  *  \brief Test for afl::string::Char
  */

#include "afl/string/char.hpp"
#include "afl/test/testrunner.hpp"

/** Test charToLower. */
AFL_TEST("afl.string.charToLower", a)
{
    // Some border cases
    a.checkEqual("nul", afl::string::charToLower('\0'), '\0');
    a.checkEqual("tab", afl::string::charToLower('\t'), '\t');
    a.checkEqual("spc", afl::string::charToLower(' '), ' ');
    a.checkEqual("FF", afl::string::charToLower('\xFF'), '\xFF');

    // Lower-case and border cases
    a.checkEqual("backtick", afl::string::charToLower('`'), '`');
    a.checkEqual("a", afl::string::charToLower('a'), 'a');
    a.checkEqual("i", afl::string::charToLower('i'), 'i');
    a.checkEqual("z", afl::string::charToLower('z'), 'z');
    a.checkEqual("{", afl::string::charToLower('{'), '{');

    // Upper-case and border cases
    a.checkEqual("@", afl::string::charToLower('@'), '@');
    a.checkEqual("A", afl::string::charToLower('A'), 'a');
    a.checkEqual("I", afl::string::charToLower('I'), 'i');
    a.checkEqual("Z", afl::string::charToLower('Z'), 'z');
    a.checkEqual("[", afl::string::charToLower('['), '[');

    // Numbers
    a.checkEqual("0", afl::string::charToLower('0'), '0');
    a.checkEqual("5", afl::string::charToLower('5'), '5');
    a.checkEqual("9", afl::string::charToLower('9'), '9');

    // Latin-1 unaffected
    a.checkEqual("C4", afl::string::charToLower('\xC4'), '\xC4');
    a.checkEqual("C4", afl::string::charToLower('\xE4'), '\xE4');
}

/** Test charToUpper. */
AFL_TEST("afl.string.charToUpper", a)
{
    // Some border cases
    a.checkEqual("nul", afl::string::charToUpper('\0'), '\0');
    a.checkEqual("tab", afl::string::charToUpper('\t'), '\t');
    a.checkEqual("spc", afl::string::charToUpper(' '), ' ');
    a.checkEqual("FF", afl::string::charToUpper('\xFF'), '\xFF');

    // Lower-case and border cases
    a.checkEqual("backtick", afl::string::charToUpper('`'), '`');
    a.checkEqual("a", afl::string::charToUpper('a'), 'A');
    a.checkEqual("i", afl::string::charToUpper('i'), 'I');
    a.checkEqual("z", afl::string::charToUpper('z'), 'Z');
    a.checkEqual("{", afl::string::charToUpper('{'), '{');

    // Upper-case and border cases
    a.checkEqual("@", afl::string::charToUpper('@'), '@');
    a.checkEqual("A", afl::string::charToUpper('A'), 'A');
    a.checkEqual("I", afl::string::charToUpper('I'), 'I');
    a.checkEqual("Z", afl::string::charToUpper('Z'), 'Z');
    a.checkEqual("[", afl::string::charToUpper('['), '[');

    // Numbers
    a.checkEqual("0", afl::string::charToUpper('0'), '0');
    a.checkEqual("5", afl::string::charToUpper('5'), '5');
    a.checkEqual("9", afl::string::charToUpper('9'), '9');

    // Latin-1 unaffected
    a.checkEqual("C4", afl::string::charToUpper('\xC4'), '\xC4');
    a.checkEqual("E4", afl::string::charToUpper('\xE4'), '\xE4');
}

/** Test charIsLower. */
AFL_TEST("afl.string.charIsLower", a)
{
    // Some border cases
    a.check("nul", !afl::string::charIsLower('\0'));
    a.check("tab", !afl::string::charIsLower('\t'));
    a.check("spc", !afl::string::charIsLower(' '));
    a.check("FF", !afl::string::charIsLower('\xFF'));

    // Lower-case and border cases
    a.check("backtick", !afl::string::charIsLower('`'));
    a.check("a",  afl::string::charIsLower('a'));
    a.check("i",  afl::string::charIsLower('i'));
    a.check("z",  afl::string::charIsLower('z'));
    a.check("{", !afl::string::charIsLower('{'));

    // Upper-case and border cases
    a.check("@", !afl::string::charIsLower('@'));
    a.check("A", !afl::string::charIsLower('A'));
    a.check("I", !afl::string::charIsLower('I'));
    a.check("Z", !afl::string::charIsLower('Z'));
    a.check("[", !afl::string::charIsLower('['));

    // Numbers
    a.check("0", !afl::string::charIsLower('0'));
    a.check("5", !afl::string::charIsLower('5'));
    a.check("9", !afl::string::charIsLower('9'));

    // Latin-1
    a.check("C4", !afl::string::charIsLower('\xC4'));
    a.check("E4", !afl::string::charIsLower('\xE4'));
}

/** Test charIsUpper. */
AFL_TEST("afl.string.charIsUpper", a)
{
    // Some border cases
    a.check("nul", !afl::string::charIsUpper('\0'));
    a.check("tab", !afl::string::charIsUpper('\t'));
    a.check("spc", !afl::string::charIsUpper(' '));
    a.check("FF", !afl::string::charIsUpper('\xFF'));

    // Lower-case and border cases
    a.check("backtick", !afl::string::charIsUpper('`'));
    a.check("a", !afl::string::charIsUpper('a'));
    a.check("i", !afl::string::charIsUpper('i'));
    a.check("z", !afl::string::charIsUpper('z'));
    a.check("{", !afl::string::charIsUpper('{'));

    // Upper-case and border cases
    a.check("@", !afl::string::charIsUpper('@'));
    a.check("A",  afl::string::charIsUpper('A'));
    a.check("I",  afl::string::charIsUpper('I'));
    a.check("Z",  afl::string::charIsUpper('Z'));
    a.check("[", !afl::string::charIsUpper('['));

    // Numbers
    a.check("0", !afl::string::charIsUpper('0'));
    a.check("5", !afl::string::charIsUpper('5'));
    a.check("9", !afl::string::charIsUpper('9'));

    // Latin-1
    a.check("C4", !afl::string::charIsUpper('\xC4'));
    a.check("F4", !afl::string::charIsUpper('\xE4'));
}

/** Test charIsSpace. */
AFL_TEST("afl.string.charIsSpace", a)
{
    // Some border cases
    a.check("nul", !afl::string::charIsSpace('\0'));
    a.check("tab",  afl::string::charIsSpace('\t'));
    a.check("cr",   afl::string::charIsSpace('\r'));
    a.check("lf",   afl::string::charIsSpace('\n'));
    a.check("spc",  afl::string::charIsSpace(' '));
    a.check("FF",  !afl::string::charIsSpace('\xFF'));

    // Lower-case and border cases
    a.check("backtick", !afl::string::charIsSpace('`'));
    a.check("a", !afl::string::charIsSpace('a'));
    a.check("i", !afl::string::charIsSpace('i'));
    a.check("z", !afl::string::charIsSpace('z'));
    a.check("{", !afl::string::charIsSpace('{'));

    // Upper-case and border cases
    a.check("@", !afl::string::charIsSpace('@'));
    a.check("A", !afl::string::charIsSpace('A'));
    a.check("I", !afl::string::charIsSpace('I'));
    a.check("Z", !afl::string::charIsSpace('Z'));
    a.check("[", !afl::string::charIsSpace('['));

    // Numbers
    a.check("0", !afl::string::charIsSpace('0'));
    a.check("5", !afl::string::charIsSpace('5'));
    a.check("9", !afl::string::charIsSpace('9'));

    // Latin-1
    a.check("C4", !afl::string::charIsSpace('\xC4'));
    a.check("E4", !afl::string::charIsSpace('\xE4'));
}

/** Test charIsDigit. */
AFL_TEST("afl.string.charIsDigit", a)
{
    // Some border cases
    a.check("nul", !afl::string::charIsDigit('\0'));
    a.check("tab", !afl::string::charIsDigit('\t'));
    a.check("cr",  !afl::string::charIsDigit('\r'));
    a.check("lf",  !afl::string::charIsDigit('\n'));
    a.check("spc", !afl::string::charIsDigit(' '));
    a.check("FF" , !afl::string::charIsDigit('\xFF'));

    // Lower-case and border cases
    a.check("backtick", !afl::string::charIsDigit('`'));
    a.check("a", !afl::string::charIsDigit('a'));
    a.check("i", !afl::string::charIsDigit('i'));
    a.check("z", !afl::string::charIsDigit('z'));
    a.check("{", !afl::string::charIsDigit('{'));

    // Upper-case and border cases
    a.check("@", !afl::string::charIsDigit('@'));
    a.check("A", !afl::string::charIsDigit('A'));
    a.check("I", !afl::string::charIsDigit('I'));
    a.check("Z", !afl::string::charIsDigit('Z'));
    a.check("[", !afl::string::charIsDigit('['));

    // Numbers
    a.check("0",  afl::string::charIsDigit('0'));
    a.check("5",  afl::string::charIsDigit('5'));
    a.check("9",  afl::string::charIsDigit('9'));

    // Latin-1
    a.check("C4", !afl::string::charIsDigit('\xC4'));
    a.check("E4", !afl::string::charIsDigit('\xE4'));
}

/** Test charIsAlphanumeric. */
AFL_TEST("afl.string.charIsAlphanumeric", a)
{
    // Some border cases
    a.check("nul", !afl::string::charIsAlphanumeric('\0'));
    a.check("tab", !afl::string::charIsAlphanumeric('\t'));
    a.check("cr",  !afl::string::charIsAlphanumeric('\r'));
    a.check("lf",  !afl::string::charIsAlphanumeric('\n'));
    a.check("spc", !afl::string::charIsAlphanumeric(' '));
    a.check("FF",  !afl::string::charIsAlphanumeric('\xFF'));

    // Lower-case and border cases
    a.check("backtick", !afl::string::charIsAlphanumeric('`'));
    a.check("a",  afl::string::charIsAlphanumeric('a'));
    a.check("i",  afl::string::charIsAlphanumeric('i'));
    a.check("z",  afl::string::charIsAlphanumeric('z'));
    a.check("{", !afl::string::charIsAlphanumeric('{'));

    // Upper-case and border cases
    a.check("@", !afl::string::charIsAlphanumeric('@'));
    a.check("A",  afl::string::charIsAlphanumeric('A'));
    a.check("I",  afl::string::charIsAlphanumeric('I'));
    a.check("Z",  afl::string::charIsAlphanumeric('Z'));
    a.check("[", !afl::string::charIsAlphanumeric('['));

    // Numbers
    a.check("0",  afl::string::charIsAlphanumeric('0'));
    a.check("5",  afl::string::charIsAlphanumeric('5'));
    a.check("9",  afl::string::charIsAlphanumeric('9'));

    // Latin-1
    a.check("C4", !afl::string::charIsAlphanumeric('\xC4'));
    a.check("E4", !afl::string::charIsAlphanumeric('\xE4'));
}
