/**
  *  \file u/t_string_char.cpp
  *  \brief Test for afl/string/char.hpp
  */

#include "afl/string/char.hpp"

#include "u/t_string.hpp"

/** Test charToLower. */
void
TestStringChar::testToLower()
{
    // Some border cases
    TS_ASSERT_EQUALS(afl::string::charToLower('\0'), '\0');
    TS_ASSERT_EQUALS(afl::string::charToLower('\t'), '\t');
    TS_ASSERT_EQUALS(afl::string::charToLower(' '), ' ');
    TS_ASSERT_EQUALS(afl::string::charToLower('\xFF'), '\xFF');

    // Lower-case and border cases
    TS_ASSERT_EQUALS(afl::string::charToLower('`'), '`');
    TS_ASSERT_EQUALS(afl::string::charToLower('a'), 'a');
    TS_ASSERT_EQUALS(afl::string::charToLower('i'), 'i');
    TS_ASSERT_EQUALS(afl::string::charToLower('z'), 'z');
    TS_ASSERT_EQUALS(afl::string::charToLower('{'), '{');

    // Upper-case and border cases
    TS_ASSERT_EQUALS(afl::string::charToLower('@'), '@');
    TS_ASSERT_EQUALS(afl::string::charToLower('A'), 'a');
    TS_ASSERT_EQUALS(afl::string::charToLower('I'), 'i');
    TS_ASSERT_EQUALS(afl::string::charToLower('Z'), 'z');
    TS_ASSERT_EQUALS(afl::string::charToLower('['), '[');

    // Numbers
    TS_ASSERT_EQUALS(afl::string::charToLower('0'), '0');
    TS_ASSERT_EQUALS(afl::string::charToLower('5'), '5');
    TS_ASSERT_EQUALS(afl::string::charToLower('9'), '9');

    // Latin-1 unaffected
    TS_ASSERT_EQUALS(afl::string::charToLower('\xC4'), '\xC4');
    TS_ASSERT_EQUALS(afl::string::charToLower('\xE4'), '\xE4');
}

/** Test charToUpper. */
void
TestStringChar::testToUpper()
{
    // Some border cases
    TS_ASSERT_EQUALS(afl::string::charToUpper('\0'), '\0');
    TS_ASSERT_EQUALS(afl::string::charToUpper('\t'), '\t');
    TS_ASSERT_EQUALS(afl::string::charToUpper(' '), ' ');
    TS_ASSERT_EQUALS(afl::string::charToUpper('\xFF'), '\xFF');

    // Lower-case and border cases
    TS_ASSERT_EQUALS(afl::string::charToUpper('`'), '`');
    TS_ASSERT_EQUALS(afl::string::charToUpper('a'), 'A');
    TS_ASSERT_EQUALS(afl::string::charToUpper('i'), 'I');
    TS_ASSERT_EQUALS(afl::string::charToUpper('z'), 'Z');
    TS_ASSERT_EQUALS(afl::string::charToUpper('{'), '{');

    // Upper-case and border cases
    TS_ASSERT_EQUALS(afl::string::charToUpper('@'), '@');
    TS_ASSERT_EQUALS(afl::string::charToUpper('A'), 'A');
    TS_ASSERT_EQUALS(afl::string::charToUpper('I'), 'I');
    TS_ASSERT_EQUALS(afl::string::charToUpper('Z'), 'Z');
    TS_ASSERT_EQUALS(afl::string::charToUpper('['), '[');

    // Numbers
    TS_ASSERT_EQUALS(afl::string::charToUpper('0'), '0');
    TS_ASSERT_EQUALS(afl::string::charToUpper('5'), '5');
    TS_ASSERT_EQUALS(afl::string::charToUpper('9'), '9');

    // Latin-1 unaffected
    TS_ASSERT_EQUALS(afl::string::charToUpper('\xC4'), '\xC4');
    TS_ASSERT_EQUALS(afl::string::charToUpper('\xE4'), '\xE4');
}

/** Test charIsLower. */
void
TestStringChar::testIsLower()
{
    // Some border cases
    TS_ASSERT(!afl::string::charIsLower('\0'));
    TS_ASSERT(!afl::string::charIsLower('\t'));
    TS_ASSERT(!afl::string::charIsLower(' '));
    TS_ASSERT(!afl::string::charIsLower('\xFF'));

    // Lower-case and border cases
    TS_ASSERT(!afl::string::charIsLower('`'));
    TS_ASSERT( afl::string::charIsLower('a'));
    TS_ASSERT( afl::string::charIsLower('i'));
    TS_ASSERT( afl::string::charIsLower('z'));
    TS_ASSERT(!afl::string::charIsLower('{'));

    // Upper-case and border cases
    TS_ASSERT(!afl::string::charIsLower('@'));
    TS_ASSERT(!afl::string::charIsLower('A'));
    TS_ASSERT(!afl::string::charIsLower('I'));
    TS_ASSERT(!afl::string::charIsLower('Z'));
    TS_ASSERT(!afl::string::charIsLower('['));

    // Numbers
    TS_ASSERT(!afl::string::charIsLower('0'));
    TS_ASSERT(!afl::string::charIsLower('5'));
    TS_ASSERT(!afl::string::charIsLower('9'));

    // Latin-1
    TS_ASSERT(!afl::string::charIsLower('\xC4'));
    TS_ASSERT(!afl::string::charIsLower('\xE4'));
}

/** Test charIsUpper. */
void
TestStringChar::testIsUpper()
{
    // Some border cases
    TS_ASSERT(!afl::string::charIsUpper('\0'));
    TS_ASSERT(!afl::string::charIsUpper('\t'));
    TS_ASSERT(!afl::string::charIsUpper(' '));
    TS_ASSERT(!afl::string::charIsUpper('\xFF'));

    // Lower-case and border cases
    TS_ASSERT(!afl::string::charIsUpper('`'));
    TS_ASSERT(!afl::string::charIsUpper('a'));
    TS_ASSERT(!afl::string::charIsUpper('i'));
    TS_ASSERT(!afl::string::charIsUpper('z'));
    TS_ASSERT(!afl::string::charIsUpper('{'));

    // Upper-case and border cases
    TS_ASSERT(!afl::string::charIsUpper('@'));
    TS_ASSERT( afl::string::charIsUpper('A'));
    TS_ASSERT( afl::string::charIsUpper('I'));
    TS_ASSERT( afl::string::charIsUpper('Z'));
    TS_ASSERT(!afl::string::charIsUpper('['));

    // Numbers
    TS_ASSERT(!afl::string::charIsUpper('0'));
    TS_ASSERT(!afl::string::charIsUpper('5'));
    TS_ASSERT(!afl::string::charIsUpper('9'));

    // Latin-1
    TS_ASSERT(!afl::string::charIsUpper('\xC4'));
    TS_ASSERT(!afl::string::charIsUpper('\xE4'));
}

/** Test charIsSpace. */
void
TestStringChar::testIsSpace()
{
    // Some border cases
    TS_ASSERT(!afl::string::charIsSpace('\0'));
    TS_ASSERT( afl::string::charIsSpace('\t'));
    TS_ASSERT( afl::string::charIsSpace('\r'));
    TS_ASSERT( afl::string::charIsSpace('\n'));
    TS_ASSERT( afl::string::charIsSpace(' '));
    TS_ASSERT(!afl::string::charIsSpace('\xFF'));

    // Lower-case and border cases
    TS_ASSERT(!afl::string::charIsSpace('`'));
    TS_ASSERT(!afl::string::charIsSpace('a'));
    TS_ASSERT(!afl::string::charIsSpace('i'));
    TS_ASSERT(!afl::string::charIsSpace('z'));
    TS_ASSERT(!afl::string::charIsSpace('{'));

    // Upper-case and border cases
    TS_ASSERT(!afl::string::charIsSpace('@'));
    TS_ASSERT(!afl::string::charIsSpace('A'));
    TS_ASSERT(!afl::string::charIsSpace('I'));
    TS_ASSERT(!afl::string::charIsSpace('Z'));
    TS_ASSERT(!afl::string::charIsSpace('['));

    // Numbers
    TS_ASSERT(!afl::string::charIsSpace('0'));
    TS_ASSERT(!afl::string::charIsSpace('5'));
    TS_ASSERT(!afl::string::charIsSpace('9'));

    // Latin-1
    TS_ASSERT(!afl::string::charIsSpace('\xC4'));
    TS_ASSERT(!afl::string::charIsSpace('\xE4'));
}

/** Test charIsDigit. */
void
TestStringChar::testIsDigit()
{
    // Some border cases
    TS_ASSERT(!afl::string::charIsDigit('\0'));
    TS_ASSERT(!afl::string::charIsDigit('\t'));
    TS_ASSERT(!afl::string::charIsDigit('\r'));
    TS_ASSERT(!afl::string::charIsDigit('\n'));
    TS_ASSERT(!afl::string::charIsDigit(' '));
    TS_ASSERT(!afl::string::charIsDigit('\xFF'));

    // Lower-case and border cases
    TS_ASSERT(!afl::string::charIsDigit('`'));
    TS_ASSERT(!afl::string::charIsDigit('a'));
    TS_ASSERT(!afl::string::charIsDigit('i'));
    TS_ASSERT(!afl::string::charIsDigit('z'));
    TS_ASSERT(!afl::string::charIsDigit('{'));

    // Upper-case and border cases
    TS_ASSERT(!afl::string::charIsDigit('@'));
    TS_ASSERT(!afl::string::charIsDigit('A'));
    TS_ASSERT(!afl::string::charIsDigit('I'));
    TS_ASSERT(!afl::string::charIsDigit('Z'));
    TS_ASSERT(!afl::string::charIsDigit('['));

    // Numbers
    TS_ASSERT( afl::string::charIsDigit('0'));
    TS_ASSERT( afl::string::charIsDigit('5'));
    TS_ASSERT( afl::string::charIsDigit('9'));

    // Latin-1
    TS_ASSERT(!afl::string::charIsDigit('\xC4'));
    TS_ASSERT(!afl::string::charIsDigit('\xE4'));
}

/** Test charIsAlphanumeric. */
void
TestStringChar::testIsAlphanumeric()
{
    // Some border cases
    TS_ASSERT(!afl::string::charIsAlphanumeric('\0'));
    TS_ASSERT(!afl::string::charIsAlphanumeric('\t'));
    TS_ASSERT(!afl::string::charIsAlphanumeric('\r'));
    TS_ASSERT(!afl::string::charIsAlphanumeric('\n'));
    TS_ASSERT(!afl::string::charIsAlphanumeric(' '));
    TS_ASSERT(!afl::string::charIsAlphanumeric('\xFF'));

    // Lower-case and border cases
    TS_ASSERT(!afl::string::charIsAlphanumeric('`'));
    TS_ASSERT( afl::string::charIsAlphanumeric('a'));
    TS_ASSERT( afl::string::charIsAlphanumeric('i'));
    TS_ASSERT( afl::string::charIsAlphanumeric('z'));
    TS_ASSERT(!afl::string::charIsAlphanumeric('{'));

    // Upper-case and border cases
    TS_ASSERT(!afl::string::charIsAlphanumeric('@'));
    TS_ASSERT( afl::string::charIsAlphanumeric('A'));
    TS_ASSERT( afl::string::charIsAlphanumeric('I'));
    TS_ASSERT( afl::string::charIsAlphanumeric('Z'));
    TS_ASSERT(!afl::string::charIsAlphanumeric('['));

    // Numbers
    TS_ASSERT( afl::string::charIsAlphanumeric('0'));
    TS_ASSERT( afl::string::charIsAlphanumeric('5'));
    TS_ASSERT( afl::string::charIsAlphanumeric('9'));

    // Latin-1
    TS_ASSERT(!afl::string::charIsAlphanumeric('\xC4'));
    TS_ASSERT(!afl::string::charIsAlphanumeric('\xE4'));
}
