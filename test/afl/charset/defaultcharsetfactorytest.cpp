/**
  *  \file test/afl/charset/defaultcharsetfactorytest.cpp
  *  \brief Test for afl::charset::DefaultCharsetFactory
  */

#include "afl/charset/defaultcharsetfactory.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/charset/charset.hpp"

AFL_TEST("afl.charset.DefaultCharsetFactory", a)
{
    afl::charset::DefaultCharsetFactory factory;

    // Latin 1 by different names
    std::auto_ptr<afl::charset::Charset> p(factory.createCharset("iso-8859-1"));
    a.checkNonNull("01", p.get());
    a.checkEqual("02", afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xA2");

    p.reset(factory.createCharset("iso88591"));
    a.checkNonNull("11", p.get());
    a.checkEqual("12", afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xA2");

    p.reset(factory.createCharset("latin1"));
    a.checkNonNull("21", p.get());
    a.checkEqual("22", afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xA2");

    // UTF-8
    p.reset(factory.createCharset("utf-8"));
    a.checkNonNull("31", p.get());
    a.checkEqual("32", afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xC2\xA2");
}
