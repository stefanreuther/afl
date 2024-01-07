/**
  *  \file test/afl/io/textfiletest.cpp
  *  \brief Test for afl::io::TextFile
  */

#include "afl/io/textfile.hpp"

#include "afl/charset/codepage.hpp"
#include "afl/charset/codepagecharset.hpp"
#include "afl/charset/utf8charset.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    /** Utility for testReadLine. */
    void doReadLineTest(afl::test::Assert a, const char* data)
    {
        afl::io::ConstMemoryStream ms(afl::string::toBytes(data));
        afl::io::TextFile tf(ms);

        String_t line;

        a.check("read first line", tf.readLine(line));
        a.checkEqual("first line content", line, "abc");
        a.checkEqual("first line number", tf.getLineNumber(), afl::io::TextFile::LineNumber_t(1));

        a.check("read second line", tf.readLine(line));
        a.checkEqual("second line content", line, "def");
        a.checkEqual("second line number", tf.getLineNumber(), afl::io::TextFile::LineNumber_t(2));

        a.check("read third line", !tf.readLine(line));
        a.checkEqual("third line content", line, "");
    }
}

/** Test readLine. */
AFL_TEST("afl.io.TextFile:readLine", a)
{
    doReadLineTest(a("Unix"),                  "abc\012def\012");
    doReadLineTest(a("DOS"),                   "abc\015\012def\015\012");
    doReadLineTest(a("Mac"),                   "abc\015def\015");
    doReadLineTest(a("Unix unterminated"),     "abc\012def");
    doReadLineTest(a("DOS unterminated"),      "abc\015\012def");
    doReadLineTest(a("Mac unterminated"),      "abc\015def");

    doReadLineTest(a("Unix C-z"),              "abc\012def\012\032");
    doReadLineTest(a("DOS C-z"),               "abc\015\012def\015\012\032");
    doReadLineTest(a("Mac C-z"),               "abc\015def\015\032");
    doReadLineTest(a("Unix unterminated C-z"), "abc\012def\032");
    doReadLineTest(a("DOS unterminated C-z"),  "abc\015\012def\032");
    doReadLineTest(a("Mac unterminated C-z"),  "abc\015def\032");
}

/*
 *  Various encodings
 */

AFL_TEST("afl.io.TextFile:encoding:l1:default", a)
{
    // Default encoding is latin1
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;

    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xC3\x82\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:l1:explicit", a)
{
    // Explicitly-set latin1
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageLatin1));

    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xC3\x82\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:koi8-r", a)
{
    // Explicitly-set KOI8-R
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageKOI8R));

    a.check("readLine", tf.readLine(line));

    // 0xC2 -> U+0431,   0xD0 0xB1
    // 0x80 -> U+2500,   0xE2 0x94 0x80
    a.checkEqual("content", line, "\xD0\xB1\xE2\x94\x80");
}

AFL_TEST("afl.io.TextFile:encoding:utf-8:explicit", a)
{
    // UTF-8
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    tf.setCharsetNew(new afl::charset::Utf8Charset());

    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:utf-8:implicit", a)
{
    // implicit UTF-8
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\xBF\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;

    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:not-utf8", a)
{
    // not UTF-8
    afl::io::ConstMemoryStream ms(afl::string::toBytes("a\xEF\xBB\xBF\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    a.check("readLine", tf.readLine(line));

    // 0xEF = 0xC3 0xAF
    // 0xBB = 0xC2 0xBB
    // 0xBF = 0xC2 0xBF
    a.checkEqual("", line,
                 "a"
                 "\xC3\xAF\xC2\xBB\xC2\xBF"
                 "\xC3\x82\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:not-utf-8-2", a)
{
    // also not UTF-8
    afl::io::ConstMemoryStream ms(afl::string::toBytes("a\n\xEF\xBB\xBF\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;

    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "a");

    a.check("readLine", tf.readLine(line));
    a.checkEqual("", line,
                 "\xC3\xAF\xC2\xBB\xC2\xBF"
                 "\xC3\x82\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:not-utf-8-3", a)
{
    // not UTF-8, too
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\n\xBF\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xC3\xAF\xC2\xBB");
    a.check("readLine", tf.readLine(line));
    a.checkEqual("", line,
                 "\xC2\xBF"
                 "\xC3\x82\xC2\x80");
}

AFL_TEST("afl.io.TextFile:encoding:snoop-implicitly-disabled", a)
{
    // character set overrides implicit UTF-8 by default....
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\xBF\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageKOI8R));

    // 0xEF -> U+041E,   0xD0 0x9E
    // 0xBB -> U+253B,   0xE2 0x94 0xBB     // nonstandard!
    // 0xBF -> U+00A9,   0xC2 0xA9
    // 0xC2 -> U+0431,   0xD0 0xB1
    // 0x80 -> U+2500,   0xE2 0x94 0x80
    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xD0\x9E\xE2\x94\xBB\xC2\xA9\xD0\xB1\xE2\x94\x80");
}

AFL_TEST("afl.io.TextFile:encoding:snoop-explicitly-enabled", a)
{
    // ....but we can still combine the two
    afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\xBF\xC2\x80"));
    afl::io::TextFile tf(ms);
    String_t line;
    tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageKOI8R));
    tf.setUtf8Snoop(true);

    a.check("readLine", tf.readLine(line));
    a.checkEqual("content", line, "\xC2\x80");
}

/*
 *  Writing
 */

AFL_TEST("afl.io.TextFile:write:l1:default", a)
{
    // Test writing with default Latin-1 charset
    afl::io::InternalStream ms;
    afl::io::TextFile tf(ms);
    tf.writeLine("hi");
    tf.writeLine("there\xC2\xA1");

    a.check("expected file size 1", tf.getSize() == 10 || tf.getSize() == 12);
    a.checkEqual("expected file position 1", tf.getPos(), tf.getSize());

    tf.flush();
    a.check("expected file size 2", tf.getSize() == 10 || tf.getSize() == 12);
    a.checkEqual("expected file position 2", tf.getPos(), tf.getSize());
    a.check("expected file size 3", ms.getSize() == 10 || ms.getSize() == 12);

    String_t str(afl::string::fromBytes(ms.getContent()));

    a.check("content", str == "hi\nthere\xA1\n" || str == "hi\rthere\xA1\r" || str == "hi\r\nthere\xA1\r\n");
}

AFL_TEST("afl.io.TextFile:write:cp437", a)
{
    // Test writing with CP437 charset
    afl::io::InternalStream ms;
    afl::io::TextFile tf(ms);
    tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepage437));
    tf.writeLine("hi");
    tf.writeLine("there\xC2\xA1");

    a.check("expected file size 1", tf.getSize() == 10 || tf.getSize() == 12);
    a.checkEqual("expected file position 1", tf.getPos(), tf.getSize());

    tf.flush();
    a.check("expected file size 2", tf.getSize() == 10 || tf.getSize() == 12);
    a.checkEqual("expected file position 2", tf.getPos(), tf.getSize());
    a.check("expected file size 3", ms.getSize() == 10 || ms.getSize() == 12);

    String_t str(afl::string::fromBytes(ms.getContent()));

    a.check("content", str == "hi\nthere\xAD\n" || str == "hi\rthere\xAD\r" || str == "hi\r\nthere\xAD\r\n");
}

AFL_TEST("afl.io.TextFile:write:utf-8", a)
{
    // Test writing with UTF-8 charset
    afl::io::InternalStream ms;
    afl::io::TextFile tf(ms);
    tf.setCharsetNew(new afl::charset::Utf8Charset());
    tf.writeLine("hi");
    tf.writeLine("there\xC2\x80");

    a.check("expected file size 1", tf.getSize() == 11 || tf.getSize() == 13);
    a.checkEqual("expected file position 1", tf.getPos(), tf.getSize());

    tf.flush();
    a.check("expected file size 2", tf.getSize() == 11 || tf.getSize() == 13);
    a.checkEqual("expected file position 2", tf.getPos(), tf.getSize());

    String_t str(afl::string::fromBytes(ms.getContent()));

    a.check("content", str == "hi\nthere\xC2\x80\n" || str == "hi\rthere\xC2\x80\r" || str == "hi\r\nthere\xC2\x80\r\n");
}

AFL_TEST("afl.io.TextFile:write:big", a)
{
    // Test writing and exceeding buffer size
    afl::io::InternalStream ms;
    afl::io::TextFile tf(ms);
    for (int i = 0; i < 10000; ++i) {
        tf.writeLine("hi");
    }
    a.check("expected file size", tf.getPos() == 30000 || tf.getPos() == 40000);
    a.check("expected file position", tf.getPos() == tf.getSize());
    tf.flush();

    String_t result(afl::string::fromBytes(ms.getContent()));

    a.check("expected content size", result.length() == 30000 || result.length() == 40000);
    a.checkEqual("content text", result.substr(0, 2), "hi");
    a.check("content newline", result[2] == '\r' || result[2] == '\n');
}
