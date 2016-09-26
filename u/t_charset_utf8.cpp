/**
  *  \file u/t_charset_utf8.cpp
  *  \brief Test for afl::charset::Utf8
  */

#include <cstdlib>
#include "afl/charset/utf8.hpp"

#include "u/t_charset.hpp"
#include "afl/charset/utf8reader.hpp"
#include "afl/charset/unicode.hpp"
#include "afl/string/string.hpp"

using afl::charset::Utf8;
using afl::charset::Utf8Reader;
using afl::charset::Unichar_t;
using afl::string::String_t;

/** Test that encoding and then decoding produces identical results. */
void
TestCharsetUtf8::testEncDec()
{
    // This iterates through some non-characters, so we must configure appropriately.
    {
        Utf8 u8(Utf8::AllowNonCharacters | Utf8::AllowErrorEscapes);
        for (Unichar_t i = 0; i < 0x20000; ++i) {
            String_t tmp;
            u8.append(tmp, i);
            Utf8Reader rdr(afl::string::toBytes(tmp), Utf8::AllowNonCharacters | Utf8::AllowErrorEscapes);
            TS_ASSERT(rdr.hasMore());
            Unichar_t g = rdr.eat();
            TS_ASSERT_EQUALS(g, i);
            TS_ASSERT(!rdr.hasMore());
        }
    }

    // Error characters can now be preserved.
    {
        Utf8 u8(Utf8::AllowNonCharacters);
        String_t e;
        u8.append(e, afl::charset::makeErrorCharacter(0xC2));
        u8.append(e, afl::charset::makeErrorCharacter(0x80));
        Utf8Reader rdr(afl::string::toBytes(e), Utf8::AllowNonCharacters);

        TS_ASSERT(e.size() > 2);

        TS_ASSERT(rdr.hasMore());
        uint32_t g = rdr.eat();
        TS_ASSERT_EQUALS(g, afl::charset::makeErrorCharacter(0xC2));

        TS_ASSERT(rdr.hasMore());
        g = rdr.eat();
        TS_ASSERT_EQUALS(g, afl::charset::makeErrorCharacter(0x80));

        TS_ASSERT(!rdr.hasMore());
    }
}

/** Test that decoding and then encoding produces identical results. */
void
TestCharsetUtf8::testDecEnc()
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
                TS_ASSERT_EQUALS(n, Utf8Reader(tmp, 0).count());

                // This no longer holds, just check that it doesn't crash above
                // string_t tmp2; tmp2.reserve(3);
                // for (int m = 0; m < n; ++m)
                //     appendUtf8(tmp2, u[m]);
                //
                // TS_ASSERT_EQUALS(tmp2.size(), 3);
                // TS_ASSERT_EQUALS(tmp2.compare(0, 3, tmp, 3), 0);
                (void) u;
            }
        }
    }
}

/** Test decoding random strings. */
void
TestCharsetUtf8::testFuzz()
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
            TS_ASSERT_LESS_THAN_EQUALS(n, NCHAR);
            u[n++] = rdr.eat();
        }

        // This no longer holds, so we just check it doesn't crash above
        // string_t tmp2; tmp.reserve(NCHAR);
        // for (int m = 0; m < n; ++m)
        //     appendUtf8(tmp2, u[m]);
        //
        // TS_ASSERT_EQUALS(tmp, tmp2);
        (void) u;
    }
}

/** Test lengthUtf8(). */
void
TestCharsetUtf8::testLength()
{
    TS_ASSERT_EQUALS(Utf8().length(""),             0U);
    TS_ASSERT_EQUALS(Utf8().length("123"),          3U);    /* ASCII ASCII ASCII */
    TS_ASSERT_EQUALS(Utf8().length("\xc2""23"),     3U);    /* ERROR ASCII ASCII */
    TS_ASSERT_EQUALS(Utf8().length("\xc2\x80""3"),  2U);    /* <--RUNE-->  ASCII */
    TS_ASSERT_EQUALS(Utf8().length("\xc2\x80\xc2"), 2U);    /* <--RUNE-->  ERROR */
}

/** Test substrUtf8(). */
void
TestCharsetUtf8::testSubstr()
{
    Utf8 u8;
    TS_ASSERT_EQUALS(u8.substr("", 10, 10), String_t(""));

    // String -> String
    TS_ASSERT_EQUALS(u8.substr("foobar", 0, 2), String_t("fo"));
    TS_ASSERT_EQUALS(u8.substr("foobar", 5, 7), String_t("r"));
    TS_ASSERT_EQUALS(u8.substr("foobar", 2, 2), String_t("ob"));
    TS_ASSERT_EQUALS(u8.substr("f\xc2\x80obar", 0, 2), String_t("f\xc2\x80"));
    TS_ASSERT_EQUALS(u8.substr("f\xc2\x80obar", 2, 2), String_t("ob"));

    // Memory -> String
    static const uint8_t foobar[] = { 'f', 'o', 'o', 'b', 'a', 'r' };
    static const uint8_t fUUobar[] = { 'f', 0xC2, 0x80, 'o', 'b', 'a', 'r' };
    TS_ASSERT_EQUALS(u8.substr(foobar, 0, 2), String_t("fo"));
    TS_ASSERT_EQUALS(u8.substr(foobar, 5, 7), String_t("r"));
    TS_ASSERT_EQUALS(u8.substr(foobar, 2, 2), String_t("ob"));
    TS_ASSERT_EQUALS(u8.substr(fUUobar, 0, 2), String_t("f\xc2\x80"));
    TS_ASSERT_EQUALS(u8.substr(fUUobar, 2, 2), String_t("ob"));

    // Memory -> Memory
    Utf8::Bytes_t mem = u8.substrMemory(foobar, 0, 2);
    TS_ASSERT_EQUALS(mem.unsafeData(), &foobar[0]);
    TS_ASSERT_EQUALS(mem.size(), 2U);

    mem = u8.substrMemory(foobar, 2, 2);
    TS_ASSERT_EQUALS(mem.unsafeData(), &foobar[2]);
    TS_ASSERT_EQUALS(mem.size(), 2U);

    mem = u8.substrMemory(fUUobar, 0, 2);
    TS_ASSERT_EQUALS(mem.unsafeData(), &fUUobar[0]);
    TS_ASSERT_EQUALS(mem.size(), 3U);

    mem = u8.substrMemory(fUUobar, 2, 2);
    TS_ASSERT_EQUALS(mem.unsafeData(), &fUUobar[3]);
    TS_ASSERT_EQUALS(mem.size(), 2U);
}

/** Test byteToCharPos(), charToBytePos(). */
void
TestCharsetUtf8::testPos()
{
    Utf8 u8;
    TS_ASSERT_EQUALS(u8.charToBytePos("abc", 0), 0U);
    TS_ASSERT_EQUALS(u8.charToBytePos("abc", 1), 1U);
    TS_ASSERT_EQUALS(u8.charToBytePos("abc", 2), 2U);
    TS_ASSERT_EQUALS(u8.charToBytePos("abc", 3), 3U);
    TS_ASSERT_EQUALS(u8.charToBytePos("abc", 4), 3U);
    TS_ASSERT_EQUALS(u8.charToBytePos("abc", -1), 3U);

    TS_ASSERT_EQUALS(u8.charToBytePos("a\xC2\x80y", 0), 0U);
    TS_ASSERT_EQUALS(u8.charToBytePos("a\xC2\x80y", 1), 1U);
    TS_ASSERT_EQUALS(u8.charToBytePos("a\xC2\x80y", 2), 3U);
    TS_ASSERT_EQUALS(u8.charToBytePos("a\xC2\x80y", 3), 4U);
    TS_ASSERT_EQUALS(u8.charToBytePos("a\xC2\x80y", 4), 4U);
    TS_ASSERT_EQUALS(u8.charToBytePos("a\xC2\x80y", -1), 4U);

    TS_ASSERT_EQUALS(u8.byteToCharPos("abc", 0), 0U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("abc", 1), 1U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("abc", 2), 2U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("abc", 3), 3U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("abc", 4), 3U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("abc", -1), 3U);

    TS_ASSERT_EQUALS(u8.byteToCharPos("a\xC2\x80y", 0), 0U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("a\xC2\x80y", 1), 1U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("a\xC2\x80y", 2), 2U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("a\xC2\x80y", 3), 2U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("a\xC2\x80y", 4), 3U);
    TS_ASSERT_EQUALS(u8.byteToCharPos("a\xC2\x80y", -1), 3U);
}
