/**
  *  \file afl/charset/urlencoding.cpp
  *  \brief Class afl::charset::UrlEncoding
  */

#include "afl/charset/urlencoding.hpp"
#include "afl/string/hex.hpp"

afl::charset::UrlEncoding::~UrlEncoding()
{ }

afl::base::GrowableBytes_t
afl::charset::UrlEncoding::encode(afl::string::ConstStringMemory_t in)
{
    afl::base::GrowableBytes_t result;
    while (const char* pc = in.eat()) {
        uint8_t u = uint8_t(*pc);
        // We escape 8-bit and controls, '%' (quote), '&' (parameter separator),
        // '+' (space replacement), '=' (key/value separator)
        if (u >= 0x80 || u < 0x20 || u == '%' || u == '&' || u == '+' || u == '=') {
            result.append('%');
            result.append(static_cast<uint8_t>(afl::string::HEX_DIGITS_UPPER[u >> 4]));
            result.append(static_cast<uint8_t>(afl::string::HEX_DIGITS_UPPER[u & 15]));
        } else if (u == ' ') {
            result.append('+');
        } else {
            result.append(u);
        }
    }
    return result;
}

String_t
afl::charset::UrlEncoding::decode(afl::base::ConstBytes_t in)
{
    String_t result;
    while (const uint8_t* pc = in.eat()) {
        typedef const uint8_t TwoBytes_t[2];
        TwoBytes_t* p2;
        if (*pc == '%' && (p2 = in.eatN<2>()) != 0) {
            int a = afl::string::getHexDigitValue(static_cast<char>((*p2)[0]));
            int b = afl::string::getHexDigitValue(static_cast<char>((*p2)[1]));
            if (a >= 0 && b >= 0) {
                result += char(16*a + b);
            } else {
                result += static_cast<char>(*pc);
                result += static_cast<char>((*p2)[0]);
                result += static_cast<char>((*p2)[1]);
            }
        } else if (*pc == '+') {
            result += ' ';
        } else {
            result += static_cast<char>(*pc);
        }
    }
    return result;
}

afl::charset::UrlEncoding*
afl::charset::UrlEncoding::clone() const
{
    return new UrlEncoding();
}
