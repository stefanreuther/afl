/**
  *  \file test/afl/string/proxytranslatortest.cpp
  *  \brief Test for afl::string::ProxyTranslator
  */

#include "afl/string/proxytranslator.hpp"

#include "afl/string/translator.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.string.ProxyTranslator", a)
{
    class Parenizor : public afl::string::Translator {
     public:
        String_t translate(afl::string::ConstStringMemory_t mem) const
            { return "(" + afl::string::fromMemory(mem) + ")"; }
    };
    Parenizor p;
    a.checkEqual("01", p.translateString("hi"), "(hi)");

    afl::string::ProxyTranslator pp(p);
    a.checkEqual("02", pp.translateString("hi"), "(hi)");

    afl::string::ProxyTranslator ppp(pp);
    a.checkEqual("03", pp.translateString("hi"), "(hi)");
}
