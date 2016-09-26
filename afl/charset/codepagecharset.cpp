/**
  *  \file afl/charset/codepagecharset.cpp
  *  \brief Class afl::charset::CodepageCharset
  */

#include "afl/charset/codepagecharset.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/charset/codepage.hpp"
#include "afl/charset/utf8reader.hpp"
#include "afl/base/countof.hpp"

afl::charset::CodepageCharset::~CodepageCharset()
{ }

// Charset:
String_t
afl::charset::CodepageCharset::encode(afl::string::ConstStringMemory_t in)
{
    // Result
    String_t result;

    Utf8Reader rdr(in.toBytes(), 0);
    while (rdr.hasMore()) {
        Unichar_t ch = rdr.eat();
        if (ch < 0x80) {
            // ASCII. Just append.
            result.append(1, char(ch));
        } else {
            // Try to find the character in the codepage.
            // If we don't find any, drop it.
            for (size_t i = 0; i < countof(m_codepage.m_characters); ++i) {
                if (m_codepage.m_characters[i] == ch) {
                    result.append(1, char(0x80 + i));
                    break;
                }
            }
        }
    }

    return result;
}

String_t
afl::charset::CodepageCharset::decode(afl::string::ConstStringMemory_t in)
{
    // Temporaries
    String_t result;
    Utf8 u8(0);

    // Parse as bytes
    Utf8::Bytes_t inMem(in.toBytes());

    while (const uint8_t* pch = inMem.eat()) {
        const uint8_t ch = *pch;
        if (ch < 0x80) {
            u8.append(result, ch);
        } else {
            u8.append(result, m_codepage.m_characters[ch - 0x80]);
        }
    }

    return result;
}

afl::charset::CodepageCharset*
afl::charset::CodepageCharset::clone() const
{
    return new CodepageCharset(m_codepage);
}
