/**
  *  \file u/t_io_textfile.cpp
  *  \brief Test for afl::io::TextFile
  */

#include "afl/io/textfile.hpp"

#include "u/t_io.hpp"
#include "afl/charset/codepage.hpp"
#include "afl/charset/codepagecharset.hpp"
#include "afl/charset/utf8.hpp"
#include "afl/charset/utf8charset.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/internalstream.hpp"
#include "afl/io/memorystream.hpp"

/** Utility for testReadLine. */
void
TestIoTextFile::doReadLineTest(const char* data, const char* name)
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes(data));
    afl::io::TextFile tf(ms);

    String_t line;

    TSM_ASSERT(name, tf.readLine(line));
    TSM_ASSERT_EQUALS(name, line, "abc");
    TSM_ASSERT_EQUALS(name, tf.getLineNumber(), afl::io::TextFile::LineNumber_t(1));

    TSM_ASSERT(name, tf.readLine(line));
    TSM_ASSERT_EQUALS(name, line, "def");
    TSM_ASSERT_EQUALS(name, tf.getLineNumber(), afl::io::TextFile::LineNumber_t(2));

    TSM_ASSERT(name, !tf.readLine(line));
    TSM_ASSERT_EQUALS(name, line, "");
}

/** Test readLine. */
void
TestIoTextFile::testReadLine()
{
    doReadLineTest("abc\012def\012",         "Unix");
    doReadLineTest("abc\015\012def\015\012", "DOS");
    doReadLineTest("abc\015def\015",         "Mac");
    doReadLineTest("abc\012def",             "Unix unterminated");
    doReadLineTest("abc\015\012def",         "DOS unterminated");
    doReadLineTest("abc\015def",             "Mac unterminated");

    doReadLineTest("abc\012def\012\032",         "Unix C-z");
    doReadLineTest("abc\015\012def\015\012\032", "DOS C-z");
    doReadLineTest("abc\015def\015\032",         "Mac C-z");
    doReadLineTest("abc\012def\032",             "Unix unterminated C-z");
    doReadLineTest("abc\015\012def\032",         "DOS unterminated C-z");
    doReadLineTest("abc\015def\032",             "Mac unterminated C-z");
}

/** Test various encodings. */
void
TestIoTextFile::testEncoding()
{
    {
        // Default encoding is latin1
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xC3\x82\xC2\x80");
    }

    {
        // Explicitly-set latin1
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;
        tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageLatin1));

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xC3\x82\xC2\x80");
    }

    {
        // Explicitly-set KOI8-R
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;
        tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageKOI8R));

        TS_ASSERT(tf.readLine(line));

        // 0xC2 -> U+0431,   0xD0 0xB1
        // 0x80 -> U+2500,   0xE2 0x94 0x80
        TS_ASSERT_EQUALS(line, "\xD0\xB1\xE2\x94\x80");
    }

    {
        // UTF-8
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;
        tf.setCharsetNew(new afl::charset::Utf8Charset());

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xC2\x80");
    }

    {
        // implicit UTF-8
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\xBF\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xC2\x80");
    }

    {
        // not UTF-8
        afl::io::ConstMemoryStream ms(afl::string::toBytes("a\xEF\xBB\xBF\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;
        TS_ASSERT(tf.readLine(line));

        // 0xEF = 0xC3 0xAF
        // 0xBB = 0xC2 0xBB
        // 0xBF = 0xC2 0xBF
        TS_ASSERT_EQUALS(line,
                         "a"
                         "\xC3\xAF\xC2\xBB\xC2\xBF"
                         "\xC3\x82\xC2\x80");
    }

    {
        // also not UTF-8
        afl::io::ConstMemoryStream ms(afl::string::toBytes("a\n\xEF\xBB\xBF\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "a");

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line,
                         "\xC3\xAF\xC2\xBB\xC2\xBF"
                         "\xC3\x82\xC2\x80");
    }

    {
        // not UTF-8, too
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\n\xBF\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;
        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xC3\xAF\xC2\xBB");
        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line,
                         "\xC2\xBF"
                         "\xC3\x82\xC2\x80");
    }

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
        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xD0\x9E\xE2\x94\xBB\xC2\xA9\xD0\xB1\xE2\x94\x80");
    }

    {
        // ....but we can still combine the two
        afl::io::ConstMemoryStream ms(afl::string::toBytes("\xEF\xBB\xBF\xC2\x80"));
        afl::io::TextFile tf(ms);
        String_t line;
        tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepageKOI8R));
        tf.setUtf8Snoop(true);

        TS_ASSERT(tf.readLine(line));
        TS_ASSERT_EQUALS(line, "\xC2\x80");
    }
}

/** Test writing. */
void
TestIoTextFile::testWrite()
{
    {
        // Test writing with default Latin-1 charset
        afl::io::InternalStream ms;
        afl::io::TextFile tf(ms);
        tf.writeLine("hi");
        tf.writeLine("there\xC2\xA1");

        TS_ASSERT(tf.getSize() == 10 || tf.getSize() == 12);
        TS_ASSERT_EQUALS(tf.getPos(), tf.getSize());

        tf.flush();
        TS_ASSERT(tf.getSize() == 10 || tf.getSize() == 12);
        TS_ASSERT_EQUALS(tf.getPos(), tf.getSize());
        TS_ASSERT(ms.getSize() == 10 || ms.getSize() == 12);

        String_t str(afl::string::fromBytes(ms.getContent()));

        TS_ASSERT(str == "hi\nthere\xA1\n" || str == "hi\rthere\xA1\r" || str == "hi\r\nthere\xA1\r\n");
    }

    {
        // Test writing with CP437 charset
        afl::io::InternalStream ms;
        afl::io::TextFile tf(ms);
        tf.setCharsetNew(new afl::charset::CodepageCharset(afl::charset::g_codepage437));
        tf.writeLine("hi");
        tf.writeLine("there\xC2\xA1");

        TS_ASSERT(tf.getSize() == 10 || tf.getSize() == 12);
        TS_ASSERT_EQUALS(tf.getPos(), tf.getSize());

        tf.flush();
        TS_ASSERT(tf.getSize() == 10 || tf.getSize() == 12);
        TS_ASSERT_EQUALS(tf.getPos(), tf.getSize());
        TS_ASSERT(ms.getSize() == 10 || ms.getSize() == 12);

        String_t str(afl::string::fromBytes(ms.getContent()));

        TS_ASSERT(str == "hi\nthere\xAD\n" || str == "hi\rthere\xAD\r" || str == "hi\r\nthere\xAD\r\n");
    }

    {
        // Test writing with UTF-8 charset
        afl::io::InternalStream ms;
        afl::io::TextFile tf(ms);
        tf.setCharsetNew(new afl::charset::Utf8Charset());
        tf.writeLine("hi");
        tf.writeLine("there\xC2\x80");

        TS_ASSERT(tf.getSize() == 11 || tf.getSize() == 13);
        TS_ASSERT_EQUALS(tf.getPos(), tf.getSize());

        tf.flush();
        TS_ASSERT(tf.getSize() == 11 || tf.getSize() == 13);
        TS_ASSERT_EQUALS(tf.getPos(), tf.getSize());

        String_t str(afl::string::fromBytes(ms.getContent()));

        TS_ASSERT(str == "hi\nthere\xC2\x80\n" || str == "hi\rthere\xC2\x80\r" || str == "hi\r\nthere\xC2\x80\r\n");
    }

    {
        // Test writing and exceeding buffer size
        afl::io::InternalStream ms;
        afl::io::TextFile tf(ms);
        for (int i = 0; i < 10000; ++i) {
            tf.writeLine("hi");
        }
        TS_ASSERT(tf.getPos() == 30000 || tf.getPos() == 40000);
        TS_ASSERT(tf.getPos() == tf.getSize());
        tf.flush();

        String_t result(afl::string::fromBytes(ms.getContent()));

        TS_ASSERT(result.length() == 30000 || result.length() == 40000);
        TS_ASSERT_EQUALS(result.substr(0, 2), "hi");
        TS_ASSERT(result[2] == '\r' || result[2] == '\n');
    }
}
