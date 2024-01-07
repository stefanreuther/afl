/**
  *  \file test/afl/io/textsinktest.cpp
  *  \brief Test for afl::io::TextSink
  */

#include "afl/io/textsink.hpp"

#include "afl/io/internalsink.hpp"
#include "afl/string/format.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.io.TextSink", a)
{
    afl::io::InternalSink sink;
    afl::io::TextSink text(sink);

    static char mem[] = {'m','e','m'};
    static const char ntbs[] = "ntbs";

    // Test all the forms we want to accept:
    // - string literals (including named ones, which do NOT decay to ConstStringMemory_t, which would include the null byte)
    // - ConstStringMemory_t
    // - String_t
    // - Format (via String_t conversion)
    text << "ntbs"                                   // 4 chars
         << ntbs                                     // 4 chars
         << String_t("string")                       // 6 chars
         << afl::string::ConstStringMemory_t(mem)    // 3 chars
         << afl::string::StringMemory_t(mem)         // 3 chars
         << afl::string::Format("for%s", "mat");     // 6 chars

    afl::base::ConstBytes_t bytes = sink.getContent();
    a.checkEqual("size", bytes.size(), 26U);

    static const uint8_t expect[] = { 'n','t','b','s',
                                      'n','t','b','s',
                                      's','t','r','i','n','g',
                                      'm','e','m',
                                      'm','e','m',
                                      'f','o','r','m','a','t' };
    a.check("content", bytes.equalContent(expect));
}
