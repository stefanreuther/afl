/**
  *  \file afl/string/proxytranslator.cpp
  *  \brief Class afl::string::ProxyTranslator
  */

#include "afl/string/proxytranslator.hpp"

afl::string::ProxyTranslator::ProxyTranslator(Translator& other)
    : m_other(other)
{ }

String_t
afl::string::ProxyTranslator::translate(afl::string::ConstStringMemory_t in) const
{
    return m_other.translate(in);
}
