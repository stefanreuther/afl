/**
  *  \file afl/charset/hexencoding.cpp
  *  \brief Class afl::charset::HexEncoding
  */

#include "afl/charset/hexencoding.hpp"
#include "afl/string/hex.hpp"

afl::charset::HexEncoding::HexEncoding()
    : m_digits(afl::string::HEX_DIGITS_UPPER)
{ }

afl::charset::HexEncoding::HexEncoding(const char (&digits)[16])
    : m_digits(digits)
{ }

afl::charset::HexEncoding::~HexEncoding()
{ }

afl::base::GrowableBytes_t
afl::charset::HexEncoding::encode(afl::string::ConstStringMemory_t in)
{
    afl::base::GrowableBytes_t result;
    result.reserve(in.size()*2);
    while (const char* p = in.eat()) {
        uint8_t i = *p;
        result.append(m_digits[i >> 4]);
        result.append(m_digits[i & 15]);
    }
    return result;
}

String_t
afl::charset::HexEncoding::decode(afl::base::ConstBytes_t in)
{
    String_t result;
    while (const uint8_t* p = in.eat()) {
        int a = afl::string::getHexDigitValue(*p);
        if (a >= 0) {
            if (const uint8_t* q = in.eat()) {
                int b = afl::string::getHexDigitValue(*q);
                if (b >= 0) {
                    result += char(16*a + b);
                }
            }
        }
    }
    return result;
}

afl::charset::HexEncoding*
afl::charset::HexEncoding::clone() const
{
    return new HexEncoding(m_digits);
}
