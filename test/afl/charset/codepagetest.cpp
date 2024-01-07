/**
  *  \file test/afl/charset/codepagetest.cpp
  *  \brief Test for afl::charset::Codepage
  */

#include "afl/charset/codepage.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.charset.Codepage", a)
{
    // afl::charset::Codepage is just a structure. Test the predefined instances.
    a.checkEqual("1250.19", afl::charset::g_codepage1250.m_characters[0x19], 0x2122U);  // trademark symbol
    a.checkEqual("1250.29", afl::charset::g_codepage1250.m_characters[0x29], 0x00A9U);  // copyright symbol
    a.checkEqual("1250.40", afl::charset::g_codepage1250.m_characters[0x40], 0x0154U);  // R with acute

    a.checkEqual("1251.19", afl::charset::g_codepage1251.m_characters[0x19], 0x2122U);  // trademark symbol
    a.checkEqual("1251.29", afl::charset::g_codepage1251.m_characters[0x29], 0x00A9U);  // copyright symbol
    a.checkEqual("1251.40", afl::charset::g_codepage1251.m_characters[0x40], 0x0410U);  // cyrillic A

    a.checkEqual("1252.19", afl::charset::g_codepage1252.m_characters[0x19], 0x2122U);  // trademark symbol
    a.checkEqual("1252.29", afl::charset::g_codepage1252.m_characters[0x29], 0x00A9U);  // copyright symbol
    a.checkEqual("1252.40", afl::charset::g_codepage1252.m_characters[0x40], 0x00C0U);  // A with grave

    a.checkEqual("437.11", afl::charset::g_codepage437.m_characters[0x11], 0x00E6);    // ligature ae
    a.checkEqual("437.40", afl::charset::g_codepage437.m_characters[0x40], 0x2514);    // single line top/right
    // The official definition has 0x2568 "up double and horizontal single" here.
    // We use 0x2538 "up heavy and horizontal light", because that also appears in the cyrillic versions.
    a.checkEqual("437.50", afl::charset::g_codepage437.m_characters[0x50], 0x2538);

    a.checkEqual("850.11", afl::charset::g_codepage850.m_characters[0x11], 0x00E6);    // ligature ae
    a.checkEqual("850.40", afl::charset::g_codepage850.m_characters[0x40], 0x2514);    // single line top/right
    a.checkEqual("850.50", afl::charset::g_codepage850.m_characters[0x50], 0x00F0);    // eth

    a.checkEqual("852.11", afl::charset::g_codepage852.m_characters[0x11], 0x0139);    // L with acute
    a.checkEqual("852.40", afl::charset::g_codepage852.m_characters[0x40], 0x2514);    // single line top/right
    a.checkEqual("852.50", afl::charset::g_codepage852.m_characters[0x50], 0x0111);    // d with stroke

    a.checkEqual("866.11", afl::charset::g_codepage866.m_characters[0x11], 0x0421);    // cyrillic S ("C")
    a.checkEqual("866.40", afl::charset::g_codepage866.m_characters[0x40], 0x2514);    // single line top/right
    a.checkEqual("866.50", afl::charset::g_codepage866.m_characters[0x50], 0x2538);    // box drawing, see comment at 437

    a.checkEqual("k8.11", afl::charset::g_codepageKOI8R.m_characters[0x11], 0x2592);  // 50% gray
    a.checkEqual("k8.40", afl::charset::g_codepageKOI8R.m_characters[0x40], 0x044E);  // cyrillic yu ("spaceship")
    a.checkEqual("k8.50", afl::charset::g_codepageKOI8R.m_characters[0x50], 0x043F);  // cyrillic p

    a.checkEqual("l1.19", afl::charset::g_codepageLatin1.m_characters[0x19], 0x0099U);  // control
    a.checkEqual("l1.29", afl::charset::g_codepageLatin1.m_characters[0x29], 0x00A9U);  // copyright symbol
    a.checkEqual("l1.40", afl::charset::g_codepageLatin1.m_characters[0x40], 0x00C0U);  // A with grave

    a.checkEqual("l2.19", afl::charset::g_codepageLatin2.m_characters[0x19], 0x0099U);  // control
    a.checkEqual("l2.29", afl::charset::g_codepageLatin2.m_characters[0x29], 0x0160U);  // S with caron
    a.checkEqual("l2.40", afl::charset::g_codepageLatin2.m_characters[0x40], 0x0154U);  // R with acute
}
