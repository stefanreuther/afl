/**
  *  \file afl/string/translator.cpp
  *  \brief Base class afl::string::Translator
  */

#include "afl/string/translator.hpp"
#include "afl/string/nulltranslator.hpp"

String_t
afl::string::Translator::translateString(const char* ntbs) const
{
    return translate(toMemory(ntbs));
}

String_t
afl::string::Translator::translateString(const String_t& str) const
{
    return translate(toMemory(str));
}

String_t
afl::string::Translator::get(String_t a) const
{
    return translate(toMemory(a));
}

String_t
afl::string::Translator::operator()(const char* str) const
{
    return translate(toMemory(str));
}

/**************************** System Instance ****************************/

namespace {
    std::auto_ptr<afl::string::Translator> g_systemTranslator;
}

afl::string::Translator&
afl::string::Translator::getSystemInstance()
{
    if (g_systemTranslator.get() == 0) {
        g_systemTranslator.reset(new NullTranslator());
    }
    return *g_systemTranslator;
}

void
afl::string::Translator::setSystemInstance(std::auto_ptr<Translator> inst)
{
    g_systemTranslator = inst;
}
