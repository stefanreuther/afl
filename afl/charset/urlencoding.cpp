/**
  *  \file afl/charset/urlencoding.cpp
  *  \brief Class afl::charset::UrlEncoding
  */

#include "afl/charset/urlencoding.hpp"

namespace {
    const char g_hexAlphabet[] = "0123456789ABCDEF";

    int hexValue(char c)
    {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        } else {
            return -1;
        }
    }
}


afl::charset::UrlEncoding::~UrlEncoding()
{ }

String_t
afl::charset::UrlEncoding::encode(afl::string::ConstStringMemory_t in)
{
    String_t result;
    while (const char* pc = in.eat()) {
        uint8_t u = uint8_t(*pc);
        // We escape 8-bit and controls, '%' (quote), '&' (parameter separator),
        // '+' (space replacement), '=' (key/value separator)
        if (u >= 0x80 || u < 0x20 || u == '%' || u == '&' || u == '+' || u == '=') {
            result += '%';
            result += g_hexAlphabet[(u >> 4) & 15];
            result += g_hexAlphabet[u & 15];
        } else if (u == ' ') {
            result += '+';
        } else {
            result += char(u);
        }
    }
    return result;
}

String_t
afl::charset::UrlEncoding::decode(afl::string::ConstStringMemory_t in)
{
    String_t result;
    while (const char* pc = in.eat()) {
        typedef const char TwoBytes_t[2];
        TwoBytes_t* p2;
        if (*pc == '%' && (p2 = in.eatN<2>()) != 0) {
            int a = hexValue((*p2)[0]);
            int b = hexValue((*p2)[1]);
            if (a >= 0 && b >= 0) {
                result += char(16*a + b);
            } else {
                result += *pc;
                result += (*p2)[0];
                result += (*p2)[1];
            }
        } else if (*pc == '+') {
            result += ' ';
        } else {
            result += *pc;
        }
    }
    return result;
}

afl::charset::UrlEncoding*
afl::charset::UrlEncoding::clone() const
{
    return new UrlEncoding();
}
