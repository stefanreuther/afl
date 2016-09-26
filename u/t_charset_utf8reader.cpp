/**
  *  \file u/t_charset_utf8reader.cpp
  *  \brief Test for afl::charset::Utf8Reader
  */

#include "afl/charset/utf8reader.hpp"

#include "u/t_charset.hpp"
#include "afl/charset/utf8.hpp"

/** Test various options for decoding various characters. */
void
TestCharsetUtf8Reader::testDecoder()
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
        const uint32_t OPTIONS = 0;

        TS_ASSERT_EQUALS(+Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);

        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longNull), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(surrogatePair), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }
    
    {
        const uint32_t OPTIONS = Utf8::AllowEncodedZero;

        TS_ASSERT_EQUALS(+Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(longNull), OPTIONS).eat(), 0U);

        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(surrogatePair), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        const uint32_t OPTIONS = Utf8::AllowEncodedZero | Utf8::AllowSurrogates;

        TS_ASSERT_EQUALS(+Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(longNull), OPTIONS).eat(), 0U);

        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(surrogatePair), OPTIONS).eat(), 0x50100U);
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        const uint32_t OPTIONS = Utf8::AllowNonCharacters;

        TS_ASSERT_EQUALS(+Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);

        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longNull), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(highSurrogate), OPTIONS).eat(), 0xD900U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat(), 0xDD00U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(surrogatePair), OPTIONS).eat(), 0xD900U);
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longAscii), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(longLatin), OPTIONS).eat()));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonChar1), OPTIONS).eat(), 0xFFFEU);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonChar2), OPTIONS).eat(), 0xFFFFFFFFU);
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }

    {
        const uint32_t OPTIONS = Utf8::AllowNonMinimal;

        TS_ASSERT_EQUALS(+Utf8Reader(Memory(ascii), OPTIONS).eat(), uint8_t('a'));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(null),  OPTIONS).eat(), 0U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(nonBMP),  OPTIONS).eat(), 0x50100U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(longNull), OPTIONS).eat(), 0U);

        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid1), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid3), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid4), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(invalid5), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(highSurrogate), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(lowSurrogate),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(surrogatePair), OPTIONS).eat()));
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(longAscii), OPTIONS).eat(), 0x30U);
        TS_ASSERT_EQUALS(+Utf8Reader(Memory(longLatin), OPTIONS).eat(), 0xFFU);
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar1),  OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar2), OPTIONS).eat()));
        TS_ASSERT(afl::charset::isErrorCharacter(Utf8Reader(Memory(nonChar3), OPTIONS).eat()));
    }
}

/** Test decoding multi-character sequences. */
void
TestCharsetUtf8Reader::testSequence()
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
        Utf8Reader rdr((Memory(data)), 0);

        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x30U);
        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x100U);
        for (int i = 0; i < 6; ++i) {
            TS_ASSERT(rdr.hasMore());
            TS_ASSERT(isErrorCharacter(rdr.eat()));
        }
        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x50100U);
        TS_ASSERT(!rdr.hasMore());
    }

    {
        Utf8Reader rdr((Memory(data)), Utf8::AllowSurrogates);

        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x30U);
        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x100U);
        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x50100U);
        TS_ASSERT(rdr.hasMore());
        TS_ASSERT_EQUALS(rdr.eat(), 0x50100U);
        TS_ASSERT(!rdr.hasMore());
    }
}
