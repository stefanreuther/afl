/**
  *  \file afl/test/translator.cpp
  *  \brief Class afl::test::Translator
  */

#include "afl/test/translator.hpp"

afl::test::Translator::Translator()
    : m_open("<"), m_close(">")
{ }

afl::test::Translator::Translator(String_t open, String_t close)
    : m_open(open), m_close(close)
{ }

afl::test::Translator::~Translator()
{ }

String_t
afl::test::Translator::translate(afl::string::ConstStringMemory_t in) const
{
    return m_open + afl::string::fromMemory(in) + m_close;
}
