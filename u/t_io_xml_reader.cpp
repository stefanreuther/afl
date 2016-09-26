/**
  *  \file u/t_io_xml_reader.cpp
  *  \brief Test for afl::io::xml::Reader
  */

#include <vector>
#include "afl/io/xml/reader.hpp"

#include "u/t_io_xml.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/xml/defaultentityhandler.hpp"

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
    {
        afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0'?>\n"
                                                           "\n"
                                                           "<foo>\n"
                                                           "  < bar zot >\n"
                                                           "  < / bar >\n"
                                                           "  <text id=1>\xc2\x80</text>\n"
                                                           "</foo>\n"));
        afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance());
        testBasicDocument(xr, "testUtf8 (implicit)");
    }

    {
        using afl::io::xml::Reader;
        afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0' encoding=\"utf-8\"?>\n"
                                                           "\n"
                                                           "<text id=1>\xc2\x80</text>\n"));

        Reader xr(ms, DefaultEntityHandler::getInstance());
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
//     MemoryStream ms("<?xml version='1.0' encoding='iso-8859-1'?>\n"
//                     "\n"
//                     "<foo>\n"
//                     "  < bar zot >\n"
//                     "  < / bar >\n"
//                     "  <text id=1>\xc2\x80</text>\n"
//                     "</foo>\n");
//     XmlReader xr(ms);
//     XmlReader::Token t;

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::PIStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "xml");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::PIAttribute);
//     TS_ASSERT_EQUALS(xr.getName(), "version");
//     TS_ASSERT_EQUALS(xr.getValue(), "1.0");
//     TS_ASSERT_EQUALS(xr.getTag(), "xml");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::PIAttribute);
//     TS_ASSERT_EQUALS(xr.getName(), "encoding");
//     TS_ASSERT_EQUALS(xr.getValue(), "iso-8859-1");
//     TS_ASSERT_EQUALS(xr.getTag(), "xml");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "foo");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "bar");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagAttribute);
//     TS_ASSERT_EQUALS(xr.getTag(), "bar");
//     TS_ASSERT_EQUALS(xr.getName(), "zot");
//     TS_ASSERT_EQUALS(xr.getValue(), "zot");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagEnd);
//     TS_ASSERT_EQUALS(xr.getTag(), "bar");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "text");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagAttribute);
//     TS_ASSERT_EQUALS(xr.getTag(), "text");
//     TS_ASSERT_EQUALS(xr.getName(), "id");
//     TS_ASSERT_EQUALS(xr.getValue(), "1");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::Text);
//     TS_ASSERT_EQUALS(xr.getValue(), "\xc3\x82\xc2\x80");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagEnd);
//     TS_ASSERT_EQUALS(xr.getTag(), "text");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagEnd);
//     TS_ASSERT_EQUALS(xr.getTag(), "foo");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::Eof);
}

/** Test document in KOI8-R. */
void
TestIoXmlReader::testKOI8R()
{
//     MemoryStream ms("<?xml version='1.0' encoding='koi8r'?>\n"
//                     "\n"
//                     "<foo>\n"
//                     "  < bar zot >\n"
//                     "  < / bar >\n"
//                     "  <text id=1>\xc2\x80</text>\n"
//                     "</foo>\n");
//     XmlReader xr(ms);
//     XmlReader::Token t;

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::PIStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "xml");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::PIAttribute);
//     TS_ASSERT_EQUALS(xr.getName(), "version");
//     TS_ASSERT_EQUALS(xr.getValue(), "1.0");
//     TS_ASSERT_EQUALS(xr.getTag(), "xml");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::PIAttribute);
//     TS_ASSERT_EQUALS(xr.getName(), "encoding");
//     TS_ASSERT_EQUALS(xr.getValue(), "koi8r");
//     TS_ASSERT_EQUALS(xr.getTag(), "xml");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "foo");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "bar");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagAttribute);
//     TS_ASSERT_EQUALS(xr.getTag(), "bar");
//     TS_ASSERT_EQUALS(xr.getName(), "zot");
//     TS_ASSERT_EQUALS(xr.getValue(), "zot");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagEnd);
//     TS_ASSERT_EQUALS(xr.getTag(), "bar");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagStart);
//     TS_ASSERT_EQUALS(xr.getTag(), "text");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagAttribute);
//     TS_ASSERT_EQUALS(xr.getTag(), "text");
//     TS_ASSERT_EQUALS(xr.getName(), "id");
//     TS_ASSERT_EQUALS(xr.getValue(), "1");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::Text);
//     TS_ASSERT_EQUALS(xr.getValue(), "\xD0\xB1\xE2\x94\x80");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagEnd);
//     TS_ASSERT_EQUALS(xr.getTag(), "text");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::TagEnd);
//     TS_ASSERT_EQUALS(xr.getTag(), "foo");

//     t = xr.readNext();
//     TS_ASSERT_EQUALS(t, XmlReader::Eof);
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
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance());
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
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance());
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
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance());
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
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance());
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
