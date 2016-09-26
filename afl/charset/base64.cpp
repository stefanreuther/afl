/**
  *  \file afl/charset/base64.cpp
  *  \brief Class afl::charset::Base64
  */

#include <cassert>
#include <cstring>
#include "afl/charset/base64.hpp"

namespace {
    const char g_base64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    int b64Value(char c)
    {
        const char* p = std::strchr(g_base64Alphabet, c);
        if (p != 0) {
            return int(p - g_base64Alphabet);
        } else {
            return 0;
        }
    }
}

afl::charset::Base64::~Base64()
{ }

String_t
afl::charset::Base64::encode(afl::string::ConstStringMemory_t in)
{
    // Create result
    String_t result;
    result.reserve((in.size() + 2) / 3 * 4);

    // Convert triplets
    typedef const char Triplet_t[3];
    while (Triplet_t* p3 = in.eatN<3>()) {
        uint32_t value = 65536 * uint8_t((*p3)[0]) + 256 * uint8_t((*p3)[1]) + uint8_t((*p3)[2]);
        result += g_base64Alphabet[(value >> 18) & 63];
        result += g_base64Alphabet[(value >> 12) & 63];
        result += g_base64Alphabet[(value >> 6) & 63];
        result += g_base64Alphabet[(value) & 63];
    }

    // Convert remainder
    if (const char* p1 = in.eat()) {
        if (const char* p2 = in.eat()) {
            // Two characters
            uint32_t value = 65536 * uint8_t(*p1) + 256 * uint8_t(*p2);
            result += g_base64Alphabet[(value >> 18) & 63];
            result += g_base64Alphabet[(value >> 12) & 63];
            result += g_base64Alphabet[(value >> 6) & 63];
            result += '=';
        } else {
            // One character
            uint32_t value = 65536 * uint8_t(*p1);
            result += g_base64Alphabet[(value >> 18) & 63];
            result += g_base64Alphabet[(value >> 12) & 63];
            result += '=';
            result += '=';
        }
    }
    assert(in.empty());
    return result;
}

String_t
afl::charset::Base64::decode(afl::string::ConstStringMemory_t in)
{
    // Trim trailing equal signs
    while (in.size() > 0 && *in.at(in.size()-1) == '=') {
        in.trim(in.size()-1);
    }

    // Build result
    String_t result;
    result.reserve(in.size() * 3 / 4 + 1);
    while (!in.empty()) {
        uint32_t value = 0;
        if (const char* pch = in.eat()) {
            // First character (6 bits)
            value += b64Value(*pch) << 18;
        }
        if (const char* pch = in.eat()) {
            // Second character (12 bits, enough for one result character)
            value += b64Value(*pch) << 12;
            result += char((value >> 16) & 255);
        }
        if (const char* pch = in.eat()) {
            // Third character (18 bits, enough for two result characters)
            value += b64Value(*pch) << 6;
            result += char((value >> 8) & 255);
        }
        if (const char* pch = in.eat()) {
            // Fourth character (24 bits, enough for three result characters)
            value += b64Value(*pch);
            result += char(value & 255);
        }
    }
    return result;
}

afl::charset::Base64*
afl::charset::Base64::clone() const
{
    return new Base64();
}
