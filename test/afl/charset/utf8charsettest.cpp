/**
  *  \file test/afl/charset/utf8charsettest.cpp
  *  \brief Test for afl::charset::Utf8Charset
  */

#include "afl/charset/utf8charset.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.charset.Utf8Charset:decode+encode", a)
{
    afl::charset::Utf8Charset u8cs;

    a.checkEqual("decode 1", u8cs.decode(afl::string::toBytes("foo")), "foo");
    a.checkEqual("decode 2", u8cs.decode(afl::string::toBytes("f\xc2\x90")), "f\xc2\x90");

    a.checkEqual("encode 1", afl::string::fromBytes(u8cs.encode(afl::string::toMemory("foo"))), "foo");
    a.checkEqual("encode 2", afl::string::fromBytes(u8cs.encode(afl::string::toMemory("f\xc2\x90"))), "f\xc2\x90");
}

AFL_TEST("afl.charset.Utf8Charset:clone", a)
{
    afl::charset::Utf8Charset testee;
    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    a.checkNonNull("result", result.get());
    a.checkNonNull("type", dynamic_cast<afl::charset::Utf8Charset*>(result.get()));
}
