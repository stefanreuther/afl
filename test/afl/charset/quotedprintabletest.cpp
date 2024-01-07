/**
  *  \file test/afl/charset/quotedprintabletest.cpp
  *  \brief Test for afl::charset::QuotedPrintable
  */

#include "afl/charset/quotedprintable.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.charset.QuotedPrintable:encode+decode", a)
{
    afl::charset::QuotedPrintable qp;

    // Empty
    a.checkEqual("01", afl::string::fromBytes(qp.encode(afl::string::toMemory(""))), "");
    a.checkEqual("02", qp.decode(afl::string::toBytes("")), "");

    // ASCII
    a.checkEqual("11", afl::string::fromBytes(qp.encode(afl::string::toMemory("abc"))), "abc");
    a.checkEqual("12", qp.decode(afl::string::toBytes("abc")), "abc");

    // UTF-8
    a.checkEqual("21", afl::string::fromBytes(qp.encode(afl::string::toMemory("a\xC2\x85"))), "a=C2=85");
    a.checkEqual("22", qp.decode(afl::string::toBytes("a=C2=85")), "a\xC2\x85");
    a.checkEqual("23", qp.decode(afl::string::toBytes("a=c2=85")), "a\xC2\x85");

    // ASCII with spaces
    a.checkEqual("31", afl::string::fromBytes(qp.encode(afl::string::toMemory("a b c"))), "a b c");
    a.checkEqual("32", qp.decode(afl::string::toBytes("a b c")), "a b c");

    // ASCII with equals
    a.checkEqual("41", afl::string::fromBytes(qp.encode(afl::string::toMemory("a=c"))), "a=3Dc");
    a.checkEqual("42", qp.decode(afl::string::toBytes("a=3Dc")), "a=c");
    a.checkEqual("43", qp.decode(afl::string::toBytes("a=3dc")), "a=c");

    // Non-UTF-8
    a.checkEqual("51", afl::string::fromBytes(qp.encode(afl::string::toMemory("a\xAA\xBBx"))), "a=AA=BBx");
    a.checkEqual("52", qp.decode(afl::string::toBytes("a=AA=BBx")), "a\xAA\xBBx");

    // Errors
    a.checkEqual("61", qp.decode(afl::string::toBytes("=")), "=");
    a.checkEqual("62", qp.decode(afl::string::toBytes("=1")), "=1");
    a.checkEqual("63", qp.decode(afl::string::toBytes("=XY")), "=XY");
    a.checkEqual("64", qp.decode(afl::string::toBytes("=3X")), "=3X");
    a.checkEqual("65", qp.decode(afl::string::toBytes("==33")), "==33");
    a.checkEqual("66", qp.decode(afl::string::toBytes("====33")), "===3");  // fourth "=" starts a character
}

/** Test clone. */
AFL_TEST("afl.charset.QuotedPrintable:clone", a)
{
    afl::charset::QuotedPrintable testee;
    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    a.checkNonNull("result", result.get());
    a.checkNonNull("type", dynamic_cast<afl::charset::QuotedPrintable*>(result.get()));
}
