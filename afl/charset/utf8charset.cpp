/**
  *  \file afl/charset/utf8charset.cpp
  *  \brief Class afl::charset::Utf8Charset
  */

#include "afl/charset/utf8charset.hpp"

afl::charset::Utf8Charset::~Utf8Charset()
{ }

String_t
afl::charset::Utf8Charset::encode(afl::string::ConstStringMemory_t in)
{
    return afl::string::fromMemory(in);
}

String_t
afl::charset::Utf8Charset::decode(afl::string::ConstStringMemory_t in)
{
    return afl::string::fromMemory(in);
}

afl::charset::Utf8Charset*
afl::charset::Utf8Charset::clone() const
{
    return new Utf8Charset();
}
