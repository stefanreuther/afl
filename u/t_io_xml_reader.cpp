/**
  *  \file u/t_io_xml_reader.cpp
  *  \brief Test for afl::io::xml::Reader
  */

#include <vector>
#include "afl/io/xml/reader.hpp"

#include "u/t_io_xml.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/xml/defaultentityhandler.hpp"
#include "afl/charset/defaultcharsetfactory.hpp"

using afl::io::xml::DefaultEntityHandler;

namespace {
    static const char TEST_DOCUMENT[] =
        "<?xml version='1.0'?>\n"
        "\n"
        "<foo>\n"
        "  < bar zot >\n"
        "  < / bar >\n"
        "  <text id=1>\x80</text>\n"
        "</foo>\n";

    void testBasicDocument(afl::io::xml::Reader& xr, const char* name)
    {
        using afl::io::xml::Reader;
        Reader::Token t;

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::PIStart);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "xml");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::PIAttribute);
        TSM_ASSERT_EQUALS(name, xr.getName(), "version");
        TSM_ASSERT_EQUALS(name, xr.getValue(), "1.0");
        TSM_ASSERT_EQUALS(name, xr.getTag(), "xml");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagStart);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "foo");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagStart);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "bar");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagAttribute);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "bar");
        TSM_ASSERT_EQUALS(name, xr.getName(), "zot");
        TSM_ASSERT_EQUALS(name, xr.getValue(), "zot");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagEnd);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "bar");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagStart);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "text");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagAttribute);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "text");
        TSM_ASSERT_EQUALS(name, xr.getName(), "id");
        TSM_ASSERT_EQUALS(name, xr.getValue(), "1");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::Text);
        TSM_ASSERT_EQUALS(name, xr.getValue(), "\xc2\x80");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagEnd);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "text");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::TagEnd);
        TSM_ASSERT_EQUALS(name, xr.getTag(), "foo");

        t = xr.readNext();
        TSM_ASSERT_EQUALS(name, t, Reader::Eof);
    }
}

/** Test document in UTF-8. */
void
TestIoXmlReader::testUtf8()
{
    afl::charset::DefaultCharsetFactory cf;
    {
        afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0'?>\n"
                                                           "\n"
                                                           "<foo>\n"
                                                           "  < bar zot >\n"
                                                           "  < / bar >\n"
                                                           "  <text id=1>\xc2\x80</text>\n"
                                                           "</foo>\n"));
        afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
        testBasicDocument(xr, "testUtf8 (implicit)");
    }

    {
        using afl::io::xml::Reader;
        afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0' encoding=\"utf-8\"?>\n"
                                                           "\n"
                                                           "<text id=1>\xc2\x80</text>\n"));

        Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
        Reader::Token t;

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::PIStart);
        TS_ASSERT_EQUALS(xr.getTag(), "xml");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::PIAttribute);
        TS_ASSERT_EQUALS(xr.getName(), "version");
        TS_ASSERT_EQUALS(xr.getValue(), "1.0");
        TS_ASSERT_EQUALS(xr.getTag(), "xml");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::PIAttribute);
        TS_ASSERT_EQUALS(xr.getName(), "encoding");
        TS_ASSERT_EQUALS(xr.getValue(), "utf-8");
        TS_ASSERT_EQUALS(xr.getTag(), "xml");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::TagStart);
        TS_ASSERT_EQUALS(xr.getTag(), "text");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::TagAttribute);
        TS_ASSERT_EQUALS(xr.getTag(), "text");
        TS_ASSERT_EQUALS(xr.getName(), "id");
        TS_ASSERT_EQUALS(xr.getValue(), "1");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::Text);
        TS_ASSERT_EQUALS(xr.getValue(), "\xc2\x80");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::TagEnd);
        TS_ASSERT_EQUALS(xr.getTag(), "text");

        t = xr.readNext();
        TS_ASSERT_EQUALS(t, Reader::Eof);
    }
}

/** Test document in Latin-1. */
void
TestIoXmlReader::testLatin1()
{
    using afl::io::xml::Reader;

    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0' encoding='iso-8859-1'?>\n"
                                                       "\n"
                                                       "<foo>\n"
                                                       "  < bar zot >\n"
                                                       "  < / bar >\n"
                                                       "  <text id=1>\xc2\x80</text>\n"
                                                       "</foo>\n"));
    Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    Reader::Token t;

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::PIStart);
    TS_ASSERT_EQUALS(xr.getTag(), "xml");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::PIAttribute);
    TS_ASSERT_EQUALS(xr.getName(), "version");
    TS_ASSERT_EQUALS(xr.getValue(), "1.0");
    TS_ASSERT_EQUALS(xr.getTag(), "xml");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::PIAttribute);
    TS_ASSERT_EQUALS(xr.getName(), "encoding");
    TS_ASSERT_EQUALS(xr.getValue(), "iso-8859-1");
    TS_ASSERT_EQUALS(xr.getTag(), "xml");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "bar");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagAttribute);
    TS_ASSERT_EQUALS(xr.getTag(), "bar");
    TS_ASSERT_EQUALS(xr.getName(), "zot");
    TS_ASSERT_EQUALS(xr.getValue(), "zot");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "bar");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "text");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagAttribute);
    TS_ASSERT_EQUALS(xr.getTag(), "text");
    TS_ASSERT_EQUALS(xr.getName(), "id");
    TS_ASSERT_EQUALS(xr.getValue(), "1");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::Text);
    TS_ASSERT_EQUALS(xr.getValue(), "\xc3\x82\xc2\x80");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "text");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::Eof);
}

/** Test document in KOI8-R. */
void
TestIoXmlReader::testKOI8R()
{
    using afl::io::xml::Reader;

    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0' encoding='koi8r'?>\n"
                                                       "\n"
                                                       "<foo>\n"
                                                       "  < bar zot >\n"
                                                       "  < / bar >\n"
                                                       "  <text id=1>\xc2\x80</text>\n"
                                                       "</foo>\n"));
    Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    Reader::Token t;

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::PIStart);
    TS_ASSERT_EQUALS(xr.getTag(), "xml");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::PIAttribute);
    TS_ASSERT_EQUALS(xr.getName(), "version");
    TS_ASSERT_EQUALS(xr.getValue(), "1.0");
    TS_ASSERT_EQUALS(xr.getTag(), "xml");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::PIAttribute);
    TS_ASSERT_EQUALS(xr.getName(), "encoding");
    TS_ASSERT_EQUALS(xr.getValue(), "koi8r");
    TS_ASSERT_EQUALS(xr.getTag(), "xml");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "bar");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagAttribute);
    TS_ASSERT_EQUALS(xr.getTag(), "bar");
    TS_ASSERT_EQUALS(xr.getName(), "zot");
    TS_ASSERT_EQUALS(xr.getValue(), "zot");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "bar");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "text");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagAttribute);
    TS_ASSERT_EQUALS(xr.getTag(), "text");
    TS_ASSERT_EQUALS(xr.getName(), "id");
    TS_ASSERT_EQUALS(xr.getValue(), "1");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::Text);
    TS_ASSERT_EQUALS(xr.getValue(), "\xD0\xB1\xE2\x94\x80");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "text");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, Reader::Eof);
}

/** Test document in UTF16-LE. */
void
TestIoXmlReader::testUtf16LE()
{
    String_t b;
    for (size_t i = 0; TEST_DOCUMENT[i] != 0; ++i) {
        b += TEST_DOCUMENT[i];
        b += '\0';
    }
    afl::io::ConstMemoryStream ms(afl::string::toBytes(b));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    testBasicDocument(xr, "testUtf16LE");
}

/** Test document in UTF16-BE. */
void
TestIoXmlReader::testUtf16BE()
{
    String_t b;
    for (size_t i = 0; TEST_DOCUMENT[i] != 0; ++i) {
        b += '\0';
        b += TEST_DOCUMENT[i];
    }
    afl::io::ConstMemoryStream ms(afl::string::toBytes(b));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    testBasicDocument(xr, "testUtf16BE");
}

/** Test text handling (CDATA in particular). */
void
TestIoXmlReader::testText()
{
    /* Because the XML reader discards text elements containing whitespace only,
       and since CDATA blocks are treated as separate elements,
       the newline between two CDATA blocks would be eaten.
       We therefore add a 'x'. */
    afl::io::ConstMemoryStream ms(afl::string::toBytes("a\n"
                                                       "<![CDATA[]]>x\n"
                                                       "<![CDATA[[]]]>x\n"
                                                       "<![CDATA[<foo>]]>x\n"
                                                       "<![CDATA[<?bar?>]]>x\n"
                                                       "<![CDATA[ &amp; ]]>x\n"
                                                       "<![CDATA[a]b]]]>x\n"
                                                       " &amp;\n"
                                                       " &quot;\n"
                                                       "b\n"
                                                       "&#65;&#x61;\n"
                                                       "&#160;\n"
                                                       "&#xa0;\n"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    afl::io::xml::Reader::Token t;
    String_t result;
    while ((t = xr.readNext()) != xr.Eof) {
        TS_ASSERT_EQUALS(t, xr.Text);
        result += xr.getValue();
    }
    TS_ASSERT_EQUALS(result,
                     "a\n"
                     "x\n"
                     "[]x\n"
                     "<foo>x\n"
                     "<?bar?>x\n"
                     " &amp; x\n"
                     "a]b]x\n"
                     " &\n"
                     " \"\n"
                     "b\n"
                     "Aa\n"
                     "\xc2\xa0\n"
                     "\xc2\xa0\n");
}

/** Test quote handling. */
void
TestIoXmlReader::testQuote()
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<foo a=\"bla('x')>blub\" />"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    afl::io::xml::Reader::Token t;
    t = xr.readNext();
    TS_ASSERT_EQUALS(t, xr.TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, xr.TagAttribute);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");
    TS_ASSERT_EQUALS(xr.getName(), "a");
    TS_ASSERT_EQUALS(xr.getValue(), "bla('x')>blub");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, xr.TagEnd);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    t = xr.readNext();
    TS_ASSERT_EQUALS(t, xr.Eof);
}

/** Test seeking.
    Tests that parsing can properly be resumed. */
void
TestIoXmlReader::testSeek()
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<foo a=\"\xc2\x80\">\xc2\x90"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);

    // Read everything into vectors
    std::vector<afl::io::xml::Reader::Token> tokens;
    std::vector<std::string> tags;
    std::vector<std::string> names;
    std::vector<std::string> values;
    std::vector<afl::io::Stream::FileSize_t> positions;

    afl::io::xml::Reader::Token t;
    while ((t = xr.readNext()) != xr.Eof) {
        tokens.push_back(t);
        tags.push_back(xr.getTag());
        names.push_back(xr.getName());
        values.push_back(xr.getValue());
        positions.push_back(xr.getPos());
    }

    // Must be three elements: "<foo", "a=...", content.
    TS_ASSERT_EQUALS(tokens.size(), 3U);

    // Verify reading single tokens in turn
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] != xr.TagAttribute) {
            xr.setPos(positions[i]);
            t = xr.readNext();
            TS_ASSERT_EQUALS(t, tokens[i]);
            if (i > 0) {
                TS_ASSERT_EQUALS(xr.getTag(), tags[i]);
                TS_ASSERT_EQUALS(xr.getName(), names[i]);
                TS_ASSERT_EQUALS(xr.getValue(), values[i]);
            }
            TS_ASSERT_EQUALS(xr.getPos(), positions[i]);
        }
    }

    // Verify reading multiple tokens in turn
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] != xr.TagAttribute) {
            xr.setPos(positions[i]);
            for (size_t j = i; j < tokens.size(); ++j) {
                t = xr.readNext();
                TS_ASSERT_EQUALS(t, tokens[j]);
                if (j > 0) {
                    TS_ASSERT_EQUALS(xr.getTag(), tags[j]);
                    TS_ASSERT_EQUALS(xr.getName(), names[j]);
                    TS_ASSERT_EQUALS(xr.getValue(), values[j]);
                }
                TS_ASSERT_EQUALS(xr.getPos(), positions[j]);
            }
            TS_ASSERT_EQUALS(xr.readNext(), xr.Eof);
        }
    }
}

/** Test seeking.
    Tests that parsing can properly be resumed if parsing did not start at the beginning of a file. */
void
TestIoXmlReader::testSeek2()
{
    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(afl::string::toBytes("   <foo>"));
    ms.setPos(3);

    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    TS_ASSERT_EQUALS(xr.readNext(), xr.TagStart);
    TS_ASSERT_EQUALS(xr.getPos(), 3U);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");

    xr.setPos(3);
    TS_ASSERT_EQUALS(xr.readNext(), xr.TagStart);
    TS_ASSERT_EQUALS(xr.getPos(), 3U);
    TS_ASSERT_EQUALS(xr.getTag(), "foo");
}

/** Test position handling in UTF-16. */
void
TestIoXmlReader::testPosUtf16()
{
    static const uint8_t DOC[] = { '<',0,'a',0,'1',0,'>',0,'<',0,'b',0,'2',0,'>',0,0,4 };
    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(DOC);
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);

    TS_ASSERT_EQUALS(xr.readNext(), xr.TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "a1");
    TS_ASSERT_EQUALS(xr.getPos(), 0U);

    TS_ASSERT_EQUALS(xr.readNext(), xr.TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "b2");
    TS_ASSERT_EQUALS(xr.getPos(), 8U);

    TS_ASSERT_EQUALS(xr.readNext(), xr.Text);
    TS_ASSERT_EQUALS(xr.getValue(), "\xD0\x80");
    TS_ASSERT_EQUALS(xr.getPos(), 16U);

    TS_ASSERT_EQUALS(xr.readNext(), xr.Eof);

    // Seek and retry
    xr.setPos(8);
    TS_ASSERT_EQUALS(xr.readNext(), xr.TagStart);
    TS_ASSERT_EQUALS(xr.getTag(), "b2");
    TS_ASSERT_EQUALS(xr.getPos(), 8U);
}

