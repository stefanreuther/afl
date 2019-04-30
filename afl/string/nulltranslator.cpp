/**
  *  \file afl/string/nulltranslator.cpp
  *  \brief Class afl::string::NullTranslator
  */

#include "afl/string/nulltranslator.hpp"

afl::string::NullTranslator::NullTranslator()
{ }

afl::string::NullTranslator::~NullTranslator()
{ }

String_t
afl::string::NullTranslator::translate(afl::string::ConstStringMemory_t in) const
{
    return fromMemory(in);
}
