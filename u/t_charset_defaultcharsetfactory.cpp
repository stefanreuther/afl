/**
  *  \file u/t_charset_defaultcharsetfactory.cpp
  *  \brief Test for afl::charset::DefaultCharsetFactory
  */

#include "afl/charset/defaultcharsetfactory.hpp"

#include "u/t_charset.hpp"
#include "afl/base/deleter.hpp"
#include "afl/charset/charset.hpp"

/** Simple tests. */
void
TestCharsetDefaultCharsetFactory::testIt()
{
    afl::base::Deleter del;
    afl::charset::DefaultCharsetFactory factory;

    // Latin 1 by different names
    afl::charset::Charset* p = factory.createCharset(del, "iso-8859-1");
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(p->encode(afl::string::toMemory("x\xC2\xA2")), "x\xA2");

    p = factory.createCharset(del, "iso88591");
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(p->encode(afl::string::toMemory("x\xC2\xA2")), "x\xA2");

    p = factory.createCharset(del, "latin1");
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(p->encode(afl::string::toMemory("x\xC2\xA2")), "x\xA2");

    // UTF-8
    p = factory.createCharset(del, "utf-8");
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(p->encode(afl::string::toMemory("x\xC2\xA2")), "x\xC2\xA2");
}
