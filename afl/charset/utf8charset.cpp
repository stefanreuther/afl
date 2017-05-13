/**
  *  \file afl/charset/utf8charset.cpp
  *  \brief Class afl::charset::Utf8Charset
  */

#include "afl/charset/utf8charset.hpp"

afl::charset::Utf8Charset::~Utf8Charset()
{ }

afl::base::GrowableBytes_t
afl::charset::Utf8Charset::encode(afl::string::ConstStringMemory_t in)
{
    afl::base::GrowableBytes_t result;
    result.append(in.toBytes());
    return result;
}

String_t
afl::charset::Utf8Charset::decode(afl::base::ConstBytes_t in)
{
    return afl::string::fromBytes(in);
}

afl::charset::Utf8Charset*
afl::charset::Utf8Charset::clone() const
{
    return new Utf8Charset();
}
