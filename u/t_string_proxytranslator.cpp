/**
  *  \file u/t_string_proxytranslator.cpp
  *  \brief Test for afl::string::ProxyTranslator
  */

#include "afl/string/proxytranslator.hpp"

#include "u/t_string.hpp"
#include "afl/string/translator.hpp"

/** Test ProxyTranslator. */
void
TestStringProxyTranslator::testIt()
{
    class Parenizor : public afl::string::Translator {
     public:
        String_t translate(afl::string::ConstStringMemory_t mem)
            { return "(" + afl::string::fromMemory(mem) + ")"; }
    };
    Parenizor p;
    TS_ASSERT_EQUALS(p.translateString("hi"), "(hi)");

    afl::string::ProxyTranslator pp(p);
    TS_ASSERT_EQUALS(pp.translateString("hi"), "(hi)");

    afl::string::ProxyTranslator ppp(pp);
    TS_ASSERT_EQUALS(pp.translateString("hi"), "(hi)");
}
