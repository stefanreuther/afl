/**
  *  \file afl/charset/utf8reader.cpp
  *  \brief Class afl::charset::Utf8Reader
  */

#include "afl/charset/utf8reader.hpp"
#include "afl/charset/utf8.hpp"

struct afl::charset::Utf8Reader::Character {
    Unichar_t m_value;
    uint8_t m_firstChar;
    size_t m_size;
};


afl::charset::Utf8Reader::Utf8Reader(afl::base::ConstBytes_t data, uint32_t flags)
    : m_data(data),
      m_flags(flags)
{ }

afl::charset::Unichar_t
afl::charset::Utf8Reader::eat()
{
    // Parse a character
    Character first;
    parse(m_data, first);
    if (first.m_size == 0) {
        return 0;
    }

    // Process surrogates
    if (first.m_value >= UNICODE_HIGH_SURROGATE_MIN && first.m_value <= UNICODE_HIGH_SURROGATE_MAX) {
        if ((m_flags & Utf8::AllowSurrogates) != 0) {
            // OK, parse another one.
            Character second;
            afl::base::ConstBytes_t tmp(m_data);
            tmp.split(first.m_size);
            parse(tmp, second);
            if (second.m_value >= UNICODE_LOW_SURROGATE_MIN && second.m_value <= UNICODE_LOW_SURROGATE_MAX) {
                // Valid.
                m_data.split(first.m_size + second.m_size);
                return 0x10000 + ((first.m_value & 0x3FF) << 10) + (second.m_value & 0x3FF);
            } else {
                // Invalid.
                m_data.split(1);
                return makeErrorCharacter(first.m_firstChar);
            }
        } else if ((m_flags & Utf8::AllowNonCharacters) != 0) {
            // Surrogates not accepted, but non-characters
            m_data.split(first.m_size);
            return first.m_value;
        } else {
            // Invalid.
            m_data.split(1);
            return makeErrorCharacter(first.m_firstChar);
        }
    } else if (first.m_value >= UNICODE_LOW_SURROGATE_MIN && first.m_value <= UNICODE_LOW_SURROGATE_MAX) {
        // Low surrogates
        if ((m_flags & Utf8::AllowNonCharacters) != 0) {
            // Surrogates not accepted, but non-characters
            m_data.split(first.m_size);
            return first.m_value;
        } else {
            // Invalid.
            m_data.split(1);
            return makeErrorCharacter(first.m_firstChar);
        }
    } else {
        // Not a surrogate; return as is.
        m_data.split(first.m_size);
        return first.m_value;
    }
}

bool
afl::charset::Utf8Reader::hasMore() const
{
    return !m_data.empty();
}

size_t
afl::charset::Utf8Reader::count() const
{
    Utf8Reader me(*this);
    size_t n = 0;
    while (me.hasMore()) {
        me.eat();
        ++n;
    }
    return n;
}

afl::base::ConstBytes_t
afl::charset::Utf8Reader::getRemainder() const
{
    return m_data;
}

void
afl::charset::Utf8Reader::parse(afl::base::ConstBytes_t data, Character& result) const
{
    const uint8_t* pch = data.eat();
    if (!pch) {
        // Cannot read anything.
        result.m_value = 0;
        result.m_size = 0;
        result.m_firstChar = 0;
        return;
    }

    // 0xxxxxxx                                                       =     0000 ..     007F
    // 110zzzzz 10XXXXXX                                              =     0080 ..     07FF
    // 1110zzzz 10XXXXXX 10XXXXXX                                     =     0800 ..     FFFF
    // 11110zzz 10XXXXXX 10XXXXXX 10XXXXXX                            =    10000 ..   1FFFFF
    // 111110zz 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX                   =   200000 ..  3FFFFFF [not UTF-8]
    // 1111110z 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX          =  4000000 .. 7FFFFFFF [not UTF-8]
    // 11111110 100000XX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX = 80000000 .. FFFFFFFF [not UTF-8]
    uint8_t ch = *pch;
    Unichar_t rune;
    Unichar_t minVal;
    size_t extra;
    result.m_firstChar = ch;
    if (ch < 0x80) {
        // 1 byte
        rune = ch;
        extra = 0;
        minVal = 0;
    } else if (ch >= 0xC0 && ch < 0xE0) {
        // 2 bytes
        rune = ch - 0xC0;
        extra = 1;
        minVal = 0x80;
    } else if (ch >= 0xE0 && ch < 0xF0) {
        // 3 bytes
        rune = ch - 0xE0;
        extra = 2;
        minVal = 0x800;
    } else if (ch >= 0xF0 && ch < 0xF8) {
        // 4 bytes
        rune = ch - 0xF0;
        extra = 3;
        minVal = 0x10000;
    } else if (ch >= 0xF8 && ch < 0xFC) {
        // 5 bytes
        rune = ch - 0xF8;
        extra = 4;
        minVal = 0x200000;
    } else if (ch >= 0xFC && ch < 0xFE) {
        // 6 bytes
        rune = ch - 0xFC;
        extra = 5;
        minVal = 0x4000000;
    } else if (ch >= 0xFE && ch < 0xFF) {
        // 7 bytes
        rune = ch - 0xFE;
        extra = 6;
        minVal = 0x80000000;
    } else {
        // invalid, treat as error
        result.m_value = makeErrorCharacter(ch);
        result.m_size = 1;
        return;
    }

    // Parse extra bytes
    for (size_t i = 0; i < extra; ++i) {
        const uint8_t* pch = data.eat();

        // It is valid if
        // - there is a character
        // - it's a valid continuation
        // - merging it into our rune does not overflow
        if (pch != 0 && (*pch & 0xC0) == 0x80 && (rune & 0xFC000000) == 0) {
            rune = (rune << 6) | (*pch & 0x3F);
        } else {
            // failure
            result.m_value = makeErrorCharacter(ch);
            result.m_size = 1;
            return;
        }
    }

    // Check ranges.
    // - We only accept error characters when not forbidden
    // - We do not accept non-minimally encoded characters unless allowed.
    // - We do not accept non-characters unless allowed (surrogate halves are checked outside)
    if ((isErrorCharacter(rune)
         && (m_flags & Utf8::AllowErrorEscapes) != 0)
        || (rune < minVal
            && ((m_flags & Utf8::AllowNonMinimal) == 0)
            && !(rune == 0 && (m_flags & Utf8::AllowEncodedZero) != 0 && extra == 1))
        || ((rune >= UNICODE_MAX
             || (rune & 0xFFFF) >= 0xFFFE)
            && (m_flags & Utf8::AllowNonCharacters) == 0))
    {
        result.m_value = makeErrorCharacter(ch);
        result.m_size = 1;
        return;
    }

    // We have a character. Return it.
    result.m_value = rune;
    result.m_size = 1 + extra;
}
