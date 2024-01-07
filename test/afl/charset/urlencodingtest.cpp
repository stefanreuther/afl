/**
  *  \file test/afl/charset/urlencodingtest.cpp
  *  \brief Test for afl::charset::UrlEncoding
  */

#include "afl/charset/urlencoding.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.charset.UrlEncoding:encode+decode", a)
{
    afl::charset::UrlEncoding ue;

    // Empty
    a.checkEqual("empty 1", afl::string::fromBytes(ue.encode(afl::string::toMemory(""))), "");
    a.checkEqual("empty 2", ue.decode(afl::string::toBytes("")), "");

    // ASCII
    a.checkEqual("ascii 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("abc"))), "abc");
    a.checkEqual("ascii 2", ue.decode(afl::string::toBytes("abc")), "abc");

    // UTF-8
    a.checkEqual("utf 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("a\xC2\x85"))), "a%C2%85");
    a.checkEqual("utf 2", ue.decode(afl::string::toBytes("a%C2%85")), "a\xC2\x85");
    a.checkEqual("utf 3", ue.decode(afl::string::toBytes("a%c2%85")), "a\xC2\x85");

    // ASCII with spaces
    a.checkEqual("space 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("a b c"))), "a+b+c");
    a.checkEqual("space 2", ue.decode(afl::string::toBytes("a+b+c")), "a b c");
    a.checkEqual("space 3", ue.decode(afl::string::toBytes("a%20b%20c")), "a b c");
    a.checkEqual("space 4", ue.decode(afl::string::toBytes("a b c")), "a b c");

    // ASCII with equals
    a.checkEqual("eq 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("a=c"))), "a%3Dc");
    a.checkEqual("eq 2", ue.decode(afl::string::toBytes("a%3Dc")), "a=c");
    a.checkEqual("eq 3", ue.decode(afl::string::toBytes("a%3dc")), "a=c");

    // ASCII with plus
    a.checkEqual("plus 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("a+=c"))), "a%2B%3Dc");
    a.checkEqual("plus 2", ue.decode(afl::string::toBytes("a%2B%3Dc")), "a+=c");

    // ASCII with ampersand
    a.checkEqual("amp 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("a&c"))), "a%26c");
    a.checkEqual("amp 2", ue.decode(afl::string::toBytes("a%26c")), "a&c");

    // Non-UTF-8
    a.checkEqual("bin 1", afl::string::fromBytes(ue.encode(afl::string::toMemory("a\xAA\xBBx"))), "a%AA%BBx");
    a.checkEqual("bin 2", ue.decode(afl::string::toBytes("a%AA%BBx")), "a\xAA\xBBx");
    a.checkEqual("bin 3", ue.decode(afl::string::toBytes("a%aa%bbx")), "a\xAA\xBBx");

    // Errors
    a.checkEqual("err 1", ue.decode(afl::string::toBytes("%")), "%");
    a.checkEqual("err 2", ue.decode(afl::string::toBytes("%1")), "%1");
    a.checkEqual("err 3", ue.decode(afl::string::toBytes("%XY")), "%XY");
    a.checkEqual("err 4", ue.decode(afl::string::toBytes("%3X")), "%3X");
    a.checkEqual("err 5", ue.decode(afl::string::toBytes("%%33")), "%%33");
    a.checkEqual("err 6", ue.decode(afl::string::toBytes("%%%%33")), "%%%3");  // fourth "%" starts a character
}

/** Test clone. */
AFL_TEST("afl.charset.UrlEncoding:clone", a)
{
    afl::charset::UrlEncoding testee;
    std::auto_ptr<afl::charset::Charset> result(testee.clone());
    a.checkNonNull("result", result.get());
    a.checkNonNull("type", dynamic_cast<afl::charset::UrlEncoding*>(result.get()));
}
