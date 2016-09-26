/**
  *  \file afl/charset/utf8.cpp
  *  \brief Class afl::charset::Utf8
  */

#include "afl/charset/utf8.hpp"
#include "afl/charset/utf8reader.hpp"

namespace {
    void appendOne(String_t& out, afl::charset::Unichar_t ch)
    {
        size_t extraBytes;
        if (ch <= 0x7FF) {
            out.append(1, char(0xC0 + (ch >> 6)));
            extraBytes = 1;
        } else if (ch <= 0xFFFF) {
            out.append(1, char(0xE0 + (ch >> 12)));
            extraBytes = 2;
        } else if (ch <= 0x1FFFFF) {
            out.append(1, char(0xF0 + (ch >> 18)));
            extraBytes = 3;
        } else if (ch <= 0x3FFFFFF) {
            out.append(1, char(0xF8 + (ch >> 24)));
            extraBytes = 4;
        } else if (ch <= 0x7FFFFFFF) {
            out.append(1, char(0xFC + (ch >> 30)));
            extraBytes = 4;
        } else {
            out.append(1, char(0xFE));
            extraBytes = 5;
        }
        
        while (extraBytes > 0) {
            --extraBytes;
            out.append(1, char(0x80 + ((ch >> (6*extraBytes)) & 0x3F)));
        }
    }
}


// Constructor.
afl::charset::Utf8::Utf8(uint32_t flags)
    : m_flags(flags)
{ }

// Append UTF-8 character.
String_t&
afl::charset::Utf8::append(String_t& s, Unichar_t ch) const
{
    if ((m_flags & AllowErrorEscapes) != 0 && isErrorCharacter(ch)) {
        // Escape
        s.append(1, char(getErrorCharacterId(ch)));
    } else if (ch >= 0x10000 && ch <= UNICODE_MAX && (m_flags & AllowSurrogates) != 0) {
        // Surrogate pair
        ch -= 0x10000;
        appendOne(s, UNICODE_HIGH_SURROGATE_MIN + ((ch >> 10) & 0x3FF));
        appendOne(s, UNICODE_LOW_SURROGATE_MIN  + (ch & 0x3FF));
    } else if (ch == 0 && (m_flags & AllowEncodedZero) != 0) {
        // Zero, but not minimally encoded
        appendOne(s, 0);
    } else if (ch >= 0x80) {
        // Character to be encoded
        appendOne(s, ch);
    } else {
        // Single character
        s.append(1, char(ch));
    }
    return s;
}

// Compute number of characters.
size_t
afl::charset::Utf8::length(const String_t& text) const
{
    return length(afl::string::toBytes(text));
}

// Compute number of characters.
size_t
afl::charset::Utf8::length(Bytes_t text) const
{
    return Utf8Reader(text, m_flags).count();
}

// Get substring.
String_t
afl::charset::Utf8::substr(const String_t& text, size_t index, size_t count) const
{
    return substr(afl::string::toBytes(text), index, count);
}

// Get substring.
String_t
afl::charset::Utf8::substr(Bytes_t text, size_t index, size_t count) const
{
    return afl::string::fromBytes(substrMemory(text, index, count));
}

// Get substring.
afl::charset::Utf8::Bytes_t
afl::charset::Utf8::substrMemory(Bytes_t text, size_t index, size_t count) const
{
    // Skip over initial portion
    Utf8Reader rdr(text, m_flags);
    while (rdr.hasMore() && index != 0) {
        rdr.eat();
        --index;
    }

    // Produce result
    Bytes_t tail(rdr.getRemainder());
    if (count >= tail.size()) {
        // User wants the whole remaining string
        return tail;
    } else {
        // User wants possibly less than the whole string
        while (rdr.hasMore() && count != 0) {
            rdr.eat();
            --count;
        }
        return tail.split(tail.size() - rdr.getRemainder().size());
    }
}

// Get character by position.
afl::charset::Unichar_t
afl::charset::Utf8::charAt(const String_t& text, size_t index) const
{
    return charAt(afl::string::toBytes(text), index);
}

// Get character by position.
afl::charset::Unichar_t
afl::charset::Utf8::charAt(Bytes_t text, size_t index) const
{
    // Skip over initial portion
    Utf8Reader rdr(text, m_flags);
    while (rdr.hasMore() && index != 0) {
        rdr.eat();
        --index;
    }

    // Check bounds
    if (index == 0 && rdr.hasMore()) {
        return rdr.eat();
    } else {
        return 0;
    }
}

// Convert byte position to character position.
size_t
afl::charset::Utf8::byteToCharPos(const String_t& text, size_t bytePos) const
{
    return byteToCharPos(afl::string::toBytes(text), bytePos);
}

// Convert byte position to character position.
size_t
afl::charset::Utf8::byteToCharPos(Bytes_t text, size_t bytePos) const
{
    if (bytePos >= text.size()) {
        // We cannot possibly reach this position. Just count the characters.
        return Utf8Reader(text, m_flags).count();
    } else {
        // Count characters until we have consumed bytePos bytes.
        Utf8Reader rdr(text, m_flags);
        size_t total = text.size();
        size_t result = 0;
        while (total - rdr.getRemainder().size() < bytePos) {
            rdr.eat();
            ++result;
        }
        return result;
    }
}

// Convert character position to byte position.
size_t
afl::charset::Utf8::charToBytePos(const String_t& text, size_t charPos) const
{
    return charToBytePos(afl::string::toBytes(text), charPos);
}

// Convert character position to byte position.
size_t
afl::charset::Utf8::charToBytePos(Bytes_t text, size_t charPos) const
{
    if (charPos >= text.size()) {
        // We cannot possibly have more characters in this string than it has bytes.
        return text.size();
    } else {
        // Skip charPos characters.
        Utf8Reader rdr(text, m_flags);
        size_t total = text.size();
        while (charPos != 0) {
            rdr.eat();
            --charPos;
        }
        return total - rdr.getRemainder().size();
    }
}

// Check for continuation byte.
bool
afl::charset::Utf8::isContinuationByte(uint8_t byte)
{
    return (byte & 0xC0) == 0x80;
}
