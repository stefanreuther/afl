/**
  *  \file test/afl/charset/utf8test.cpp
  *  \brief Test for afl::charset::Utf8
  */

#include "afl/charset/utf8.hpp"

#include <cstdlib>
#include "afl/charset/unicode.hpp"
#include "afl/charset/utf8reader.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

using afl::charset::Unichar_t;
using afl::charset::Utf8;
using afl::charset::Utf8Reader;

/** Test that encoding and then decoding produces identical results. */

AFL_TEST("afl.charset.Utf8:encode-coverage", a)
{
    // This iterates through some non-characters, so we must configure appropriately.
    Utf8 u8(Utf8::AllowNonCharacters | Utf8::AllowErrorEscapes);
    for (Unichar_t i = 0; i < 0x20000; ++i) {
        String_t tmp;
        u8.append(tmp, i);
        Utf8Reader rdr(afl::string::toBytes(tmp), Utf8::AllowNonCharacters | Utf8::AllowErrorEscapes);
        a.check("hasMore", rdr.hasMore());
        Unichar_t g = rdr.eat();
        a.checkEqual("value", g, i);
        a.check("hasMore 2", !rdr.hasMore());
    }
}

AFL_TEST("afl.charset.Utf8:decode-error-character", a)
{
    // Error characters can now be preserved.
    Utf8 u8(Utf8::AllowNonCharacters);
    String_t e;
    u8.append(e, afl::charset::makeErrorCharacter(0xC2));
    u8.append(e, afl::charset::makeErrorCharacter(0x80));
    Utf8Reader rdr(afl::string::toBytes(e), Utf8::AllowNonCharacters);

    a.check("size", e.size() > 2);

    a.check("hasMode", rdr.hasMore());
    uint32_t g = rdr.eat();
    a.checkEqual("value", g, afl::charset::makeErrorCharacter(0xC2));

    a.check("hasMore 2", rdr.hasMore());
    g = rdr.eat();
    a.checkEqual("value 2", g, afl::charset::makeErrorCharacter(0x80));

    a.check("hasMore 3", !rdr.hasMore());
}

/** Test that decoding and then encoding produces identical results. */
AFL_TEST("afl.charset.Utf8:decode-coverage", a)
{
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            for (int k = 0; k < 256; k += 3) { /* step 3 to speed this up a little */
                uint8_t tmp[3] = { uint8_t(i), uint8_t(j), uint8_t(k) };

                Unichar_t u[10];
                size_t n = 0;
                Utf8Reader rdr(tmp, 0);
                while (rdr.hasMore()) {
                    u[n++] = rdr.eat();
                }
                a.checkEqual("count", n, Utf8Reader(tmp, 0).count());

                // This no longer holds, just check that it doesn't crash above
                // string_t tmp2; tmp2.reserve(3);
                // for (int m = 0; m < n; ++m)
                //     appendUtf8(tmp2, u[m]);
                //
                // a.checkEqual("", tmp2.size(), 3);
                // a.checkEqual("", tmp2.compare(0, 3, tmp, 3), 0);
                (void) u;
            }
        }
    }
}

/** Test decoding random strings. */
AFL_TEST("afl.charset.Utf8:decode-coverage-random", a)
{
    static const int NCHAR = 100;
    std::srand(42);
    for (int i = 0; i < 1000; ++i) {
        String_t tmp;
        Unichar_t u[NCHAR];
        for (int j = 0; j < NCHAR; ++j) {
            tmp += char(rand() & 0xFF);
        }

        int n = 0;
        Utf8Reader rdr(afl::string::toBytes(tmp), 0);
        while (rdr.hasMore()) {
            a.check("n", n <= NCHAR);
            u[n++] = rdr.eat();
        }

        // This no longer holds, so we just check it doesn't crash above
        // string_t tmp2; tmp.reserve(NCHAR);
        // for (int m = 0; m < n; ++m)
        //     appendUtf8(tmp2, u[m]);
        //
        // a.checkEqual("", tmp, tmp2);
        (void) u;
    }
}

/** Test length(). */
AFL_TEST("afl.charset.Utf8:length", a)
{
    a.checkEqual("0",             Utf8().length(""),             0U);
    a.checkEqual("ascii",         Utf8().length("123"),          3U);    /* ASCII ASCII ASCII */
    a.checkEqual("error + ascii", Utf8().length("\xc2""23"),     3U);    /* ERROR ASCII ASCII */
    a.checkEqual("rune + ascii",  Utf8().length("\xc2\x80""3"),  2U);    /* <--RUNE-->  ASCII */
    a.checkEqual("rune + error",  Utf8().length("\xc2\x80\xc2"), 2U);    /* <--RUNE-->  ERROR */
}

/** Test substr(). */
AFL_TEST("afl.charset.Utf8:substr", a)
{
    Utf8 u8;
    a.checkEqual("01", u8.substr("", 10, 10), String_t(""));

    // String -> String
    a.checkEqual("11", u8.substr("foobar", 0, 2), String_t("fo"));
    a.checkEqual("12", u8.substr("foobar", 5, 7), String_t("r"));
    a.checkEqual("13", u8.substr("foobar", 2, 2), String_t("ob"));
    a.checkEqual("14", u8.substr("f\xc2\x80obar", 0, 2), String_t("f\xc2\x80"));
    a.checkEqual("15", u8.substr("f\xc2\x80obar", 2, 2), String_t("ob"));

    // Memory -> String
    static const uint8_t foobar[] = { 'f', 'o', 'o', 'b', 'a', 'r' };
    static const uint8_t fUUobar[] = { 'f', 0xC2, 0x80, 'o', 'b', 'a', 'r' };
    a.checkEqual("21", u8.substr(foobar, 0, 2), String_t("fo"));
    a.checkEqual("22", u8.substr(foobar, 5, 7), String_t("r"));
    a.checkEqual("23", u8.substr(foobar, 2, 2), String_t("ob"));
    a.checkEqual("24", u8.substr(fUUobar, 0, 2), String_t("f\xc2\x80"));
    a.checkEqual("25", u8.substr(fUUobar, 2, 2), String_t("ob"));

    // Memory -> Memory
    Utf8::Bytes_t mem = u8.substrMemory(foobar, 0, 2);
    a.checkEqual("31", mem.unsafeData(), &foobar[0]);
    a.checkEqual("32", mem.size(), 2U);

    mem = u8.substrMemory(foobar, 2, 2);
    a.checkEqual("41", mem.unsafeData(), &foobar[2]);
    a.checkEqual("42", mem.size(), 2U);

    mem = u8.substrMemory(fUUobar, 0, 2);
    a.checkEqual("51", mem.unsafeData(), &fUUobar[0]);
    a.checkEqual("52", mem.size(), 3U);

    mem = u8.substrMemory(fUUobar, 2, 2);
    a.checkEqual("61", mem.unsafeData(), &fUUobar[3]);
    a.checkEqual("62", mem.size(), 2U);
}

/** Test byteToCharPos(), charToBytePos(). */
AFL_TEST("afl.charset.Utf8:pos-conversion", a)
{
    Utf8 u8;
    a.checkEqual("01", u8.charToBytePos("abc", 0), 0U);
    a.checkEqual("02", u8.charToBytePos("abc", 1), 1U);
    a.checkEqual("03", u8.charToBytePos("abc", 2), 2U);
    a.checkEqual("04", u8.charToBytePos("abc", 3), 3U);
    a.checkEqual("05", u8.charToBytePos("abc", 4), 3U);
    a.checkEqual("06", u8.charToBytePos("abc", size_t(-1)), 3U);

    a.checkEqual("11", u8.charToBytePos("a\xC2\x80y", 0), 0U);
    a.checkEqual("12", u8.charToBytePos("a\xC2\x80y", 1), 1U);
    a.checkEqual("13", u8.charToBytePos("a\xC2\x80y", 2), 3U);
    a.checkEqual("14", u8.charToBytePos("a\xC2\x80y", 3), 4U);
    a.checkEqual("15", u8.charToBytePos("a\xC2\x80y", 4), 4U);
    a.checkEqual("16", u8.charToBytePos("a\xC2\x80y", size_t(-1)), 4U);

    a.checkEqual("21", u8.byteToCharPos("abc", 0), 0U);
    a.checkEqual("22", u8.byteToCharPos("abc", 1), 1U);
    a.checkEqual("23", u8.byteToCharPos("abc", 2), 2U);
    a.checkEqual("24", u8.byteToCharPos("abc", 3), 3U);
    a.checkEqual("25", u8.byteToCharPos("abc", 4), 3U);
    a.checkEqual("26", u8.byteToCharPos("abc", size_t(-1)), 3U);

    a.checkEqual("31", u8.byteToCharPos("a\xC2\x80y", 0), 0U);
    a.checkEqual("32", u8.byteToCharPos("a\xC2\x80y", 1), 1U);
    a.checkEqual("33", u8.byteToCharPos("a\xC2\x80y", 2), 2U);
    a.checkEqual("34", u8.byteToCharPos("a\xC2\x80y", 3), 2U);
    a.checkEqual("35", u8.byteToCharPos("a\xC2\x80y", 4), 3U);
    a.checkEqual("36", u8.byteToCharPos("a\xC2\x80y", size_t(-1)), 3U);
}

/** Test charAt. */
AFL_TEST("afl.charset.Utf8:charAt", a)
{
    Utf8 u8;
    a.checkEqual("1", u8.charAt("a\xC2\x80y", 0), afl::charset::Unichar_t('a'));
    a.checkEqual("2", u8.charAt("a\xC2\x80y", 1), 0x80U);
    a.checkEqual("3", u8.charAt("a\xC2\x80y", 2), afl::charset::Unichar_t('y'));
    a.checkEqual("4", u8.charAt("a\xC2\x80y", 3), 0U);
    a.checkEqual("5", u8.charAt("a\xC2\x80y", 4), 0U);
}

/** Test isContinuationByte. */
AFL_TEST("afl.charset.Utf8:isContinuationByte", a)
{
    for (int i = 0; i < 256; ++i) {
        a.checkEqual("", Utf8::isContinuationByte(uint8_t(i)), (i >= 0x80 && i < 0xC0));
    }
}

/** Test behaviour with characters from the astral plane. */
AFL_TEST("afl.charset.Utf8:astral", a)
{
    // Encoder
    Utf8 u8;

    // By default, Utf8 decodes only the regular Unicode range, and no characters ending in FFFE or FFFF.
    // Disable error checking for reverse tests.
    Utf8 u8d(Utf8::AllowNonCharacters);
    {
        // 11110_000 10_010000 10_000000 10_000000
        String_t s;
        u8.append(s, 0x10000);
        a.checkEqual("01", s, "\xF0\x90\x80\x80");
        a.checkEqual("02", u8d.charAt("\xF0\x90\x80\x80", 0), 0x10000U);
    }
    {
        // 11110_111 10_111111 10_111111 10_111111
        String_t s;
        u8.append(s, 0x1FFFFF);
        a.checkEqual("11", s, "\xF7\xBF\xBF\xBF");
        a.checkEqual("12", u8d.charAt("\xF7\xBF\xBF\xBF", 0), 0x1FFFFFU);
    }
    {
        // 111110_00 10_001000 10_000000 10_000000 10_000000
        String_t s;
        u8.append(s, 0x200000);
        a.checkEqual("21", s, "\xF8\x88\x80\x80\x80");
        a.checkEqual("22", u8d.charAt("\xF8\x88\x80\x80\x80", 0), 0x200000U);
    }
    {
        // 111110_11 10_111111 10_111111 10_111111 10_111111
        String_t s;
        u8.append(s, 0x3FFFFFF);
        a.checkEqual("31", s, "\xFB\xBF\xBF\xBF\xBF");
        a.checkEqual("32", u8d.charAt("\xFB\xBF\xBF\xBF\xBF", 0), 0x3FFFFFFU);
    }
    {
        // 1111110_1 10_110000 10_000000 10_000000 10_000000 10_000000
        String_t s;
        u8.append(s, 0x70000000);
        a.checkEqual("41", s, "\xFD\xB0\x80\x80\x80\x80");
        a.checkEqual("42", u8d.charAt("\xFD\xB0\x80\x80\x80\x80", 0), 0x70000000U);
    }
    {
        // 11111110_ 10_000010 10_000000 10_000000 10_000000 10_000000 10_000000
        String_t s;
        u8.append(s, 0x80000000);
        a.checkEqual("51", s, "\xFE\x82\x80\x80\x80\x80\x80");
        a.checkEqual("52", u8d.charAt("\xFE\x82\x80\x80\x80\x80\x80", 0), 0x80000000U);
    }
}

/** Test zeroes. */
AFL_TEST("afl.charset.Utf8:zero:unencoded", a)
{
    String_t s;
    Utf8().append(s, 0);
    a.checkEqual("01", s.size(), 1U);
    a.checkEqual("02", s[0], '\0');

    // Add an additional character to prove we don't run into the out-of-range case
    Utf8().append(s, 'a');
    a.checkEqual("11", s.size(), 2U);
    a.checkEqual("12", s[1], 'a');

    a.checkEqual("13", Utf8().charAt(s, 0), 0U);
    a.checkEqual("14", Utf8().charAt(s, 1), afl::charset::Unichar_t('a'));
    a.checkEqual("15", Utf8().charAt(s, 2), 0U);
}

AFL_TEST("afl.charset.Utf8:zero:encoded", a)
{
    String_t s;
    Utf8(Utf8::AllowEncodedZero).append(s, 0);
    a.checkEqual("01", s, "\xC0\x80");
    s += 'a';

    // Default decoder refuses to decode encoded zero
    a.check("11", afl::charset::isErrorCharacter(Utf8().charAt(s, 0)));

    // Custom decoder
    Utf8 u8d(Utf8::AllowEncodedZero);
    a.checkEqual("21", u8d.charAt(s, 0), 0U);
    a.checkEqual("22", u8d.charAt(s, 1), afl::charset::Unichar_t('a'));
    a.checkEqual("23", u8d.charAt(s, 2), 0U);
}

/** Test surrogates (CESU-8). */
AFL_TEST("afl.charset.Utf8:surrogate", a)
{
    Utf8 u8s(Utf8::AllowSurrogates);

    {
        // 1110_1101 10_100000 10_00000
        // 1110_1101 10_110000 10_00000
        String_t s;
        u8s.append(s, 0x10000);
        a.checkEqual("01", s, "\xED\xA0\x80\xED\xB0\x80");
        a.checkEqual("02", u8s.charAt("\xED\xA0\x80\xED\xB0\x80", 0), 0x10000U);
    }
    {
        // 1110_1101 10_100000 10_00000
        // 1110_1101 10_110000 10_00001
        String_t s;
        u8s.append(s, 0x10001);
        a.checkEqual("11", s, "\xED\xA0\x80\xED\xB0\x81");
        a.checkEqual("12", u8s.charAt("\xED\xA0\x80\xED\xB0\x81", 0), 0x10001U);
    }
    {
        // 1110_1101 10_101111 10_11111
        // 1110_1101 10_111111 10_11111
        String_t s;
        u8s.append(s, 0x10FFFF);
        a.checkEqual("21", s, "\xED\xAF\xBF\xED\xBF\xBF");
        a.checkEqual("22", u8s.charAt("\xED\xAF\xBF\xED\xBF\xBF", 0), 0x10FFFFU);
    }
}
