/**
  *  \file test/afl/charset/utf8readertest.cpp
  *  \brief Test for afl::charset::Utf8Reader
  */

#include "afl/charset/utf8reader.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/charset/utf8.hpp"

/** Test various options for decoding various characters. */
AFL_TEST("afl.charset.Utf8Reader:decoder", outer)
{
    typedef afl::base::Memory<const uint8_t> Memory;
    typedef afl::charset::Utf8Reader Utf8Reader;
    typedef afl::charset::Utf8 Utf8;

    const uint8_t ascii[]         = { 'a', 'b', 'c' };
    const uint8_t null[]          = { 0 };
    const uint8_t nonBMP[]        = { 0xF1, 0x90, 0x84, 0x80 };             // 50100 regularly encoded
    const uint8_t longNull[]      = { 0xC0, 0x80 };
    const uint8_t invalid1[]      = { 0x85 };
    const uint8_t invalid2[]      = { 0xC0, 0xC0 };
    const uint8_t invalid3[]      = { 0xC0, 'a' };
    const uint8_t invalid4[]      = { 0xC0 };
    const uint8_t invalid5[]      = { 0xEE, 0xBE, 0x99 };                   // EF99, error character
    const uint8_t highSurrogate[] = { 0xED, 0xA4, 0x80 };                   // D900, high-surrogate
    const uint8_t lowSurrogate[]  = { 0xED, 0xB4, 0x80 };                   // DD00, low-surrogate
    const uint8_t surrogatePair[] = { 0xED, 0xA4, 0x80, 0xED, 0xB4, 0x80 }; // 50100 as surrogate pair
    const uint8_t longAscii[]     = { 0xC0, 0xB0 };                         // 0030, nonminimal
    const uint8_t longLatin[]     = { 0xE0, 0x83, 0xBF };                   // 00FF, nonminimal
    const uint8_t nonChar1[]      = { 0xEF, 0xBF, 0xBE };                   // FFFE, non-character
    const uint8_t nonChar2[]      = { 0xFE, 0x83, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF }; // FFFFFFFF
    const uint8_t nonChar3[]      = { 0xFE, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xB0 }; // FFFFFFFF0 (overflow)

    {
        afl::test::Assert a(outer("0"));
        const uint32_t OPTIONS = 0;

        a.checkEqual("eat 1", +Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        a.checkEqual("eat 2", +Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        a.checkEqual("eat 3", +Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);

        a.check("isError 1", afl::charset::isErrorCharacter(Utf8Reader(Memory(longNull), OPTIONS).eat()));
        a.check("isError 2", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        a.check("isError 3", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        a.check("isError 4", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        a.check("isError 5", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        a.check("isError 6", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        a.check("isError 7", afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        a.check("isError 8", afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        a.check("isError 9", afl::charset::isErrorCharacter(Utf8Reader(Memory(surrogatePair), OPTIONS).eat()));
        a.check("isError A", afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        a.check("isError B", afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        a.check("isError C", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        a.check("isError D", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        a.check("isError E", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        afl::test::Assert a(outer("AllowEncodedZero"));
        const uint32_t OPTIONS = Utf8::AllowEncodedZero;

        a.checkEqual("eat 1", +Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        a.checkEqual("eat 2", +Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        a.checkEqual("eat 3", +Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);
        a.checkEqual("eat 4", +Utf8Reader(Memory(longNull), OPTIONS).eat(), 0U);

        a.check("isError 1", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        a.check("isError 2", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        a.check("isError 3", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        a.check("isError 4", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        a.check("isError 5", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        a.check("isError 6", afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        a.check("isError 7", afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        a.check("isError 8", afl::charset::isErrorCharacter(Utf8Reader(Memory(surrogatePair), OPTIONS).eat()));
        a.check("isError 9", afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        a.check("isError A", afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        a.check("isError B", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        a.check("isError C", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        a.check("isError D", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        afl::test::Assert a(outer("AllowEncodedZero+AllowSurrogates"));
        const uint32_t OPTIONS = Utf8::AllowEncodedZero | Utf8::AllowSurrogates;

        a.checkEqual("eat 1", +Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        a.checkEqual("eat 2", +Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        a.checkEqual("eat 3", +Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);
        a.checkEqual("eat 4", +Utf8Reader(Memory(longNull), OPTIONS).eat(), 0U);

        a.check("isError 1", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        a.check("isError 2", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        a.check("isError 3", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        a.check("isError 4", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        a.check("isError 5", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        a.check("isError 6", afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        a.check("isError 7", afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        a.checkEqual("eat 5", +Utf8Reader(Memory(surrogatePair), OPTIONS).eat(), 0x50100U);
        a.check("isError 8", afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        a.check("isError 9", afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        a.check("isError A", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        a.check("isError B", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        a.check("isError C", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        afl::test::Assert a(outer("AllowNonCharacters"));
        const uint32_t OPTIONS = Utf8::AllowNonCharacters;

        a.checkEqual("eat 1", +Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        a.checkEqual("eat 2", +Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        a.checkEqual("eat 3", +Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);

        a.check("isError 1", afl::charset::isErrorCharacter(Utf8Reader(Memory(longNull), OPTIONS).eat()));
        a.check("isError 2", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        a.check("isError 3", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        a.check("isError 4", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        a.check("isError 5", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        a.check("isError 6", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        a.checkEqual("eat 4", +Utf8Reader(Memory(highSurrogate), OPTIONS).eat(), 0xD900U);
        a.checkEqual("eat 5", +Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat(), 0xDD00U);
        a.checkEqual("eat 6", +Utf8Reader(Memory(surrogatePair), OPTIONS).eat(), 0xD900U);
        a.check("isError 7", afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        a.check("isError 8", afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        a.checkEqual("eat 7", +Utf8Reader(Memory(nonChar1), OPTIONS).eat(), 0xFFFEU);
        a.checkEqual("eat 8", +Utf8Reader(Memory(nonChar2), OPTIONS).eat(), 0xFFFFFFFFU);
        a.check("isError 9", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        afl::test::Assert a(outer("AllowNonMinimal"));
        const uint32_t OPTIONS = Utf8::AllowNonMinimal;

        a.checkEqual("eat 1", +Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        a.checkEqual("eat 2", +Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        a.checkEqual("eat 3", +Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);
        a.checkEqual("eat 4", +Utf8Reader(Memory(longNull), OPTIONS).eat(), 0U);

        a.check("isError 1", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        a.check("isError 2", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        a.check("isError 3", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        a.check("isError 4", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        a.check("isError 5", afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        a.check("isError 6", afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        a.check("isError 7", afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        a.check("isError 8", afl::charset::isErrorCharacter(Utf8Reader(Memory(surrogatePair), OPTIONS).eat()));
        a.checkEqual("eat 5", +Utf8Reader(Memory(longAscii), OPTIONS).eat(), 0x30U);
        a.checkEqual("eat 6", +Utf8Reader(Memory(longLatin), OPTIONS).eat(), 0xFFU);
        a.check("isError 9", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        a.check("isError A", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        a.check("isError B", afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }
}

/** Test decoding multi-character sequences. */
AFL_TEST("afl.charset.Utf8Reader:sequence", outer)
{
    typedef afl::base::Memory<const uint8_t> Memory;
    typedef afl::charset::Utf8Reader Utf8Reader;
    typedef afl::charset::Utf8 Utf8;
    using afl::charset::isErrorCharacter;

    static const uint8_t data[] = {
        0x30,                                     // ASCII
        0xC4, 0x80,                               // U+0100
        0xED, 0xA4, 0x80, 0xED, 0xB4, 0x80,       // U+50100 as surrogate pair
        0xF1, 0x90, 0x84, 0x80,                   // U+50100 regularly encoded
    };

    {
        afl::test::Assert a(outer("0"));
        Utf8Reader rdr((Memory(data)), 0);

        a.check("01", rdr.hasMore());
        a.checkEqual("02", rdr.eat(), 0x30U);
        a.check("03", rdr.hasMore());
        a.checkEqual("04", rdr.eat(), 0x100U);
        for (int i = 0; i < 6; ++i) {
            a.check("05", rdr.hasMore());
            a.check("06", isErrorCharacter(rdr.eat()));
        }
        a.check("07", rdr.hasMore());
        a.checkEqual("08", rdr.eat(), 0x50100U);
        a.check("09", !rdr.hasMore());
    }

    {
        afl::test::Assert a(outer("AllowSurrogates"));
        Utf8Reader rdr((Memory(data)), Utf8::AllowSurrogates);

        a.check("01", rdr.hasMore());
        a.checkEqual("02", rdr.eat(), 0x30U);
        a.check("03", rdr.hasMore());
        a.checkEqual("04", rdr.eat(), 0x100U);
        a.check("05", rdr.hasMore());
        a.checkEqual("06", rdr.eat(), 0x50100U);
        a.check("07", rdr.hasMore());
        a.checkEqual("08", rdr.eat(), 0x50100U);
        a.check("09", !rdr.hasMore());
    }
}
