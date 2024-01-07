/**
  *  \file test/afl/charset/unicodetest.cpp
  *  \brief Test for afl::charset::Unicode
  */

#include "afl/charset/unicode.hpp"
#include "afl/test/testrunner.hpp"

/** Test error characters. */
AFL_TEST("afl.charset.Unicode:isErrorCharacter", a)
{
    a.check("0", !afl::charset::isErrorCharacter(0));
    a.check("FF", !afl::charset::isErrorCharacter(0xFF));
    a.check("0404", !afl::charset::isErrorCharacter(0x0404));

    for (uint32_t i = 0x80; i < 0xFF; ++i) {
        afl::charset::Unichar_t ec = afl::charset::makeErrorCharacter(uint8_t(i));
        a.check("makeErrorCharacter", afl::charset::isErrorCharacter(ec));
        a.checkEqual("getErrorCharacterId", i, afl::charset::getErrorCharacterId(ec));
    }
}

/** Test case conversion. */
AFL_TEST("afl.charset.Unicode:case", a)
{
    // ASCII
    a.checkEqual("upper a", afl::charset::getUpperCase('a'), afl::charset::Unichar_t('A'));
    a.checkEqual("upper A", afl::charset::getUpperCase('A'), afl::charset::Unichar_t('A'));
    a.checkEqual("upper z", afl::charset::getUpperCase('z'), afl::charset::Unichar_t('Z'));
    a.checkEqual("upper Z", afl::charset::getUpperCase('Z'), afl::charset::Unichar_t('Z'));

    a.checkEqual("lower a", afl::charset::getLowerCase('a'), afl::charset::Unichar_t('a'));
    a.checkEqual("lower A", afl::charset::getLowerCase('A'), afl::charset::Unichar_t('a'));
    a.checkEqual("lower z", afl::charset::getLowerCase('z'), afl::charset::Unichar_t('z'));
    a.checkEqual("lower Z", afl::charset::getLowerCase('Z'), afl::charset::Unichar_t('z'));

    // Boundary cases
    a.checkEqual("upper @", afl::charset::getUpperCase('@'), afl::charset::Unichar_t('@'));
    a.checkEqual("lower @", afl::charset::getLowerCase('@'), afl::charset::Unichar_t('@'));
    a.checkEqual("upper [", afl::charset::getUpperCase('['), afl::charset::Unichar_t('['));
    a.checkEqual("lower [", afl::charset::getLowerCase('['), afl::charset::Unichar_t('['));

    // Non-ASCII:
    // - A with macron
    a.checkEqual("upper 100", afl::charset::getUpperCase(0x100), 0x100U);
    a.checkEqual("upper 101", afl::charset::getUpperCase(0x101), 0x100U);
    a.checkEqual("lower 100", afl::charset::getLowerCase(0x100), 0x101U);
    a.checkEqual("lower 101", afl::charset::getLowerCase(0x101), 0x101U);

    // - y with diaeresis
    a.checkEqual("upper FF",  afl::charset::getUpperCase(0xFF),  0x178U);
    a.checkEqual("upper 178", afl::charset::getUpperCase(0x178), 0x178U);
    a.checkEqual("lower FF",  afl::charset::getLowerCase(0xFF),  0xFFU);
    a.checkEqual("lower 178", afl::charset::getLowerCase(0x178), 0xFFU);

    // - Alpha
    a.checkEqual("upper 391", afl::charset::getUpperCase(0x391), 0x391U);
    a.checkEqual("upper 3B1", afl::charset::getUpperCase(0x3B1), 0x391U);
    a.checkEqual("lower 391", afl::charset::getLowerCase(0x391), 0x3B1U);
    a.checkEqual("lower 3B1", afl::charset::getLowerCase(0x3B1), 0x3B1U);

    // - cyrillic A
    a.checkEqual("upper 410", afl::charset::getUpperCase(0x410), 0x410U);
    a.checkEqual("upper 430", afl::charset::getUpperCase(0x430), 0x410U);
    a.checkEqual("lower 410", afl::charset::getLowerCase(0x410), 0x430U);
    a.checkEqual("lower 430", afl::charset::getLowerCase(0x430), 0x430U);

    // - cyrillic IO
    a.checkEqual("upper 401", afl::charset::getUpperCase(0x401), 0x401U);
    a.checkEqual("upper 451", afl::charset::getUpperCase(0x451), 0x401U);
    a.checkEqual("lower 401", afl::charset::getLowerCase(0x401), 0x451U);
    a.checkEqual("lower 451", afl::charset::getLowerCase(0x451), 0x451U);

    // - W with diaeresis
    a.checkEqual("upper 1E84", afl::charset::getUpperCase(0x1E84), 0x1E84U);
    a.checkEqual("upper 1E85", afl::charset::getUpperCase(0x1E85), 0x1E84U);
    a.checkEqual("lower 1E84", afl::charset::getLowerCase(0x1E84), 0x1E85U);
    a.checkEqual("lower 1E85", afl::charset::getLowerCase(0x1E85), 0x1E85U);

    // - DZ. This one is fun as it has three versions (DZ/Dz/dz).
    a.checkEqual("upper 1C4", afl::charset::getUpperCase(0x1C4), 0x1C4U);
    a.checkEqual("upper 1C5", afl::charset::getUpperCase(0x1C5), 0x1C4U);
    a.checkEqual("upper 1C6", afl::charset::getUpperCase(0x1C6), 0x1C4U);
    a.checkEqual("lower 1C4", afl::charset::getLowerCase(0x1C4), 0x1C6U);
    a.checkEqual("lower 1C5", afl::charset::getLowerCase(0x1C5), 0x1C6U);
    a.checkEqual("lower 1C6", afl::charset::getLowerCase(0x1C6), 0x1C6U);

    // Border cases
    a.checkEqual("lower 0", afl::charset::getLowerCase(0), 0U);
    a.checkEqual("lower FFFD", afl::charset::getLowerCase(0xFFFD), 0xFFFDU);
    a.checkEqual("lower 10000", afl::charset::getLowerCase(0x10000), 0x10000U);
    a.checkEqual("lower 100000", afl::charset::getLowerCase(0x100000), 0x100000U);
    a.checkEqual("lower err", afl::charset::getLowerCase(afl::charset::makeErrorCharacter(0)), afl::charset::makeErrorCharacter(0));

    a.checkEqual("upper 0", afl::charset::getUpperCase(0), 0U);
    a.checkEqual("upper FFFD", afl::charset::getUpperCase(0xFFFD), 0xFFFDU);
    a.checkEqual("upper 10000", afl::charset::getUpperCase(0x10000), 0x10000U);
    a.checkEqual("upper 100000", afl::charset::getUpperCase(0x100000), 0x100000U);
    a.checkEqual("upper err", afl::charset::getUpperCase(afl::charset::makeErrorCharacter(0)), afl::charset::makeErrorCharacter(0));
}
