/**
  *  \file u/t_charset_defaultcharsetfactory.cpp
  *  \brief Test for afl::charset::DefaultCharsetFactory
  */

#include <memory>
#include "afl/charset/defaultcharsetfactory.hpp"

#include "u/t_charset.hpp"
#include "afl/charset/charset.hpp"

/** Simple tests. */
void
TestCharsetDefaultCharsetFactory::testIt()
{
    afl::charset::DefaultCharsetFactory factory;

    // Latin 1 by different names
    std::auto_ptr<afl::charset::Charset> p(factory.createCharset("iso-8859-1"));
    TS_ASSERT(p.get() != 0);
    TS_ASSERT_EQUALS(afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xA2");

    p.reset(factory.createCharset("iso88591"));
    TS_ASSERT(p.get() != 0);
    TS_ASSERT_EQUALS(afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xA2");

    p.reset(factory.createCharset("latin1"));
    TS_ASSERT(p.get() != 0);
    TS_ASSERT_EQUALS(afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xA2");

    // UTF-8
    p.reset(factory.createCharset("utf-8"));
    TS_ASSERT(p.get() != 0);
    TS_ASSERT_EQUALS(afl::string::fromBytes(p->encode(afl::string::toMemory("x\xC2\xA2"))), "x\xC2\xA2");
}
