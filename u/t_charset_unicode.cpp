/**
  *  \file u/t_charset_unicode.cpp
  *  \brief Test for afl/charset/unicode.hpp
  */

#include "afl/charset/unicode.hpp"

#include "u/t_charset.hpp"

/** Test error characters. */
void
TestCharsetUnicode::testErrorChar()
{
    TS_ASSERT(!afl::charset::isErrorCharacter(0));
    TS_ASSERT(!afl::charset::isErrorCharacter(0xFF));
    TS_ASSERT(!afl::charset::isErrorCharacter(0x0404));

    for (uint32_t i = 0x80; i < 0xFF; ++i) {
        afl::charset::Unichar_t ec = afl::charset::makeErrorCharacter(uint8_t(i));
        TS_ASSERT(afl::charset::isErrorCharacter(ec));
        TS_ASSERT_EQUALS(i, afl::charset::getErrorCharacterId(ec));
    }
}

/** Test case conversion. */
void
TestCharsetUnicode::testCase()
{
    // ASCII
    TS_ASSERT_EQUALS(afl::charset::getUpperCase('a'), afl::charset::Unichar_t('A'));
    TS_ASSERT_EQUALS(afl::charset::getUpperCase('A'), afl::charset::Unichar_t('A'));
    TS_ASSERT_EQUALS(afl::charset::getUpperCase('z'), afl::charset::Unichar_t('Z'));
    TS_ASSERT_EQUALS(afl::charset::getUpperCase('Z'), afl::charset::Unichar_t('Z'));

    TS_ASSERT_EQUALS(afl::charset::getLowerCase('a'), afl::charset::Unichar_t('a'));
    TS_ASSERT_EQUALS(afl::charset::getLowerCase('A'), afl::charset::Unichar_t('a'));
    TS_ASSERT_EQUALS(afl::charset::getLowerCase('z'), afl::charset::Unichar_t('z'));
    TS_ASSERT_EQUALS(afl::charset::getLowerCase('Z'), afl::charset::Unichar_t('z'));

    // Boundary cases
    TS_ASSERT_EQUALS(afl::charset::getUpperCase('@'), afl::charset::Unichar_t('@'));
    TS_ASSERT_EQUALS(afl::charset::getLowerCase('@'), afl::charset::Unichar_t('@'));
    TS_ASSERT_EQUALS(afl::charset::getUpperCase('['), afl::charset::Unichar_t('['));
    TS_ASSERT_EQUALS(afl::charset::getLowerCase('['), afl::charset::Unichar_t('['));

    // Non-ASCII:
    // - A with macron
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x100), 0x100U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x101), 0x100U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x100), 0x101U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x101), 0x101U);

    // - y with diaeresis
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0xFF),  0x178U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x178), 0x178U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0xFF),  0xFFU);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x178), 0xFFU);

    // - Alpha
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x391), 0x391U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x3B1), 0x391U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x391), 0x3B1U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x3B1), 0x3B1U);

    // - cyrillic A
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x410), 0x410U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x430), 0x410U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x410), 0x430U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x430), 0x430U);

    // - cyrillic IO
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x401), 0x401U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x451), 0x401U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x401), 0x451U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x451), 0x451U);

    // - W with diaeresis
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x1E84), 0x1E84U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x1E85), 0x1E84U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x1E84), 0x1E85U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x1E85), 0x1E85U);

    // - DZ. This one is fun as it has three versions (DZ/Dz/dz).
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x1C4), 0x1C4U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x1C5), 0x1C4U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x1C6), 0x1C4U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x1C6), 0x1C6U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x1C6), 0x1C6U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x1C6), 0x1C6U);

    // Border cases
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0), 0U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0xFFFD), 0xFFFDU);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x10000), 0x10000U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(0x100000), 0x100000U);
    TS_ASSERT_EQUALS(afl::charset::getLowerCase(afl::charset::makeErrorCharacter(0)), afl::charset::makeErrorCharacter(0));

    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0), 0U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0xFFFD), 0xFFFDU);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x10000), 0x10000U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(0x100000), 0x100000U);
    TS_ASSERT_EQUALS(afl::charset::getUpperCase(afl::charset::makeErrorCharacter(0)), afl::charset::makeErrorCharacter(0));
}
