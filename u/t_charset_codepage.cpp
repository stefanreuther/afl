/**
  *  \file u/t_charset_codepage.cpp
  *  \brief Test for afl::charset::Codepage
  */

#include "afl/charset/codepage.hpp"

#include "u/t_charset.hpp"

/** Simple tests. */
void
TestCharsetCodepage::testIt()
{
    // afl::charset::Codepage is just a structure. Test the predefined instances.
    TS_ASSERT_EQUALS(afl::charset::g_codepage1250.m_characters[0x19], 0x2122U);  // trademark symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepage1250.m_characters[0x29], 0x00A9U);  // copyright symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepage1250.m_characters[0x40], 0x0154U);  // R with acute

    TS_ASSERT_EQUALS(afl::charset::g_codepage1251.m_characters[0x19], 0x2122U);  // trademark symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepage1251.m_characters[0x29], 0x00A9U);  // copyright symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepage1251.m_characters[0x40], 0x0410U);  // cyrillic A

    TS_ASSERT_EQUALS(afl::charset::g_codepage1252.m_characters[0x19], 0x2122U);  // trademark symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepage1252.m_characters[0x29], 0x00A9U);  // copyright symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepage1252.m_characters[0x40], 0x00C0U);  // A with grave

    TS_ASSERT_EQUALS(afl::charset::g_codepage437.m_characters[0x11], 0x00E6);    // ligature ae
    TS_ASSERT_EQUALS(afl::charset::g_codepage437.m_characters[0x40], 0x2514);    // single line top/right
    // The official definition has 0x2568 "up double and horizontal single" here.
    // We use 0x2538 "up heavy and horizontal light", because that also appears in the cyrillic versions.
    TS_ASSERT_EQUALS(afl::charset::g_codepage437.m_characters[0x50], 0x2538);

    TS_ASSERT_EQUALS(afl::charset::g_codepage850.m_characters[0x11], 0x00E6);    // ligature ae
    TS_ASSERT_EQUALS(afl::charset::g_codepage850.m_characters[0x40], 0x2514);    // single line top/right
    TS_ASSERT_EQUALS(afl::charset::g_codepage850.m_characters[0x50], 0x00F0);    // eth

    TS_ASSERT_EQUALS(afl::charset::g_codepage852.m_characters[0x11], 0x0139);    // L with acute
    TS_ASSERT_EQUALS(afl::charset::g_codepage852.m_characters[0x40], 0x2514);    // single line top/right
    TS_ASSERT_EQUALS(afl::charset::g_codepage852.m_characters[0x50], 0x0111);    // d with stroke

    TS_ASSERT_EQUALS(afl::charset::g_codepage866.m_characters[0x11], 0x0421);    // cyrillic S ("C")
    TS_ASSERT_EQUALS(afl::charset::g_codepage866.m_characters[0x40], 0x2514);    // single line top/right
    TS_ASSERT_EQUALS(afl::charset::g_codepage866.m_characters[0x50], 0x2538);    // box drawing, see comment at 437

    TS_ASSERT_EQUALS(afl::charset::g_codepageKOI8R.m_characters[0x11], 0x2592);  // 50% gray
    TS_ASSERT_EQUALS(afl::charset::g_codepageKOI8R.m_characters[0x40], 0x044E);  // cyrillic yu ("spaceship")
    TS_ASSERT_EQUALS(afl::charset::g_codepageKOI8R.m_characters[0x50], 0x043F);  // cyrillic p

    TS_ASSERT_EQUALS(afl::charset::g_codepageLatin1.m_characters[0x19], 0x0099U);  // control
    TS_ASSERT_EQUALS(afl::charset::g_codepageLatin1.m_characters[0x29], 0x00A9U);  // copyright symbol
    TS_ASSERT_EQUALS(afl::charset::g_codepageLatin1.m_characters[0x40], 0x00C0U);  // A with grave

    TS_ASSERT_EQUALS(afl::charset::g_codepageLatin2.m_characters[0x19], 0x0099U);  // control
    TS_ASSERT_EQUALS(afl::charset::g_codepageLatin2.m_characters[0x29], 0x0160U);  // S with caron
    TS_ASSERT_EQUALS(afl::charset::g_codepageLatin2.m_characters[0x40], 0x0154U);  // R with acute
}
