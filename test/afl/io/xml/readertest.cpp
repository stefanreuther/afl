/**
  *  \file test/afl/io/xml/readertest.cpp
  *  \brief Test for afl::io::xml::Reader
  */

#include "afl/io/xml/reader.hpp"

#include "afl/charset/defaultcharsetfactory.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/xml/defaultentityhandler.hpp"
#include "afl/test/testrunner.hpp"

using afl::io::xml::DefaultEntityHandler;
using afl::io::xml::Reader;

namespace {
    static const char TEST_DOCUMENT[] =
        "<?xml version='1.0'?>\n"
        "\n"
        "<foo>\n"
        "  < bar zot >\n"
        "  < / bar >\n"
        "  <text id=1>\x80</text>\n"
        "</foo>\n";

    void testBasicDocument(afl::test::Assert a, afl::io::xml::Reader& xr)
    {
        Reader::Token t;

        t = xr.readNext();
        a.check("01. start", t == Reader::PIStart);
        a.checkEqual("01. tag", xr.getTag(), "xml");

        t = xr.readNext();
        a.check("02. attr", t == Reader::PIAttribute);
        a.checkEqual("02. name", xr.getName(), "version");
        a.checkEqual("02. value", xr.getValue(), "1.0");
        a.checkEqual("02. tag", xr.getTag(), "xml");

        t = xr.readNext();
        a.check("03. start", t == Reader::TagStart);
        a.checkEqual("03. tag", xr.getTag(), "foo");

        t = xr.readNext();
        a.check("04. start", t == Reader::TagStart);
        a.checkEqual("04. tag", xr.getTag(), "bar");

        t = xr.readNext();
        a.check("05. attr", t == Reader::TagAttribute);
        a.checkEqual("05. tag", xr.getTag(), "bar");
        a.checkEqual("05. name", xr.getName(), "zot");
        a.checkEqual("05. value", xr.getValue(), "zot");

        t = xr.readNext();
        a.check("06. end", t == Reader::TagEnd);
        a.checkEqual("06. tag", xr.getTag(), "bar");

        t = xr.readNext();
        a.check("07. start", t == Reader::TagStart);
        a.checkEqual("07. tag", xr.getTag(), "text");

        t = xr.readNext();
        a.check("08. attr", t == Reader::TagAttribute);
        a.checkEqual("08. text", xr.getTag(), "text");
        a.checkEqual("08. name", xr.getName(), "id");
        a.checkEqual("08. value", xr.getValue(), "1");

        t = xr.readNext();
        a.check("09. text", t == Reader::Text);
        a.checkEqual("09. value", xr.getValue(), "\xc2\x80");

        t = xr.readNext();
        a.check("10. end", t == Reader::TagEnd);
        a.checkEqual("10. tag", xr.getTag(), "text");

        t = xr.readNext();
        a.check("11. end", t == Reader::TagEnd);
        a.checkEqual("11. tag", xr.getTag(), "foo");

        t = xr.readNext();
        a.check("12. eof", t == Reader::Eof);
    }
}

AFL_TEST("afl.io.xml.Reader:utf-8-implicit", a)
{
    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0'?>\n"
                                                       "\n"
                                                       "<foo>\n"
                                                       "  < bar zot >\n"
                                                       "  < / bar >\n"
                                                       "  <text id=1>\xc2\x80</text>\n"
                                                       "</foo>\n"));
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    testBasicDocument(a, xr);
}

AFL_TEST("afl.io.xml.Reader:utf-8-explicit", a)
{
    afl::charset::DefaultCharsetFactory cf;
    using afl::io::xml::Reader;
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<?xml version='1.0' encoding=\"utf-8\"?>\n"
                                                       "\n"
                                                       "<text id=1>\xc2\x80</text>\n"));

    Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    Reader::Token t;

    t = xr.readNext();
    a.check("01. start", t == Reader::PIStart);
    a.checkEqual("01. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("02. attr", t == Reader::PIAttribute);
    a.checkEqual("03. name", xr.getName(), "version");
    a.checkEqual("03. value", xr.getValue(), "1.0");
    a.checkEqual("03. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("04. attr", t == Reader::PIAttribute);
    a.checkEqual("04. name", xr.getName(), "encoding");
    a.checkEqual("04. value", xr.getValue(), "utf-8");
    a.checkEqual("04. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("05. start", t == Reader::TagStart);
    a.checkEqual("05. tag", xr.getTag(), "text");

    t = xr.readNext();
    a.check("06. attr", t == Reader::TagAttribute);
    a.checkEqual("06. tag", xr.getTag(), "text");
    a.checkEqual("06. name", xr.getName(), "id");
    a.checkEqual("06. value", xr.getValue(), "1");

    t = xr.readNext();
    a.check("07. text", t == Reader::Text);
    a.checkEqual("07. value", xr.getValue(), "\xc2\x80");

    t = xr.readNext();
    a.check("08. end", t == Reader::TagEnd);
    a.checkEqual("08. tag", xr.getTag(), "text");

    t = xr.readNext();
    a.check("09. eof", t == Reader::Eof);
}

AFL_TEST("afl.io.xml.Reader:latin1", a)
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
    a.check("01. start", t == Reader::PIStart);
    a.checkEqual("01. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("02. attr", t == Reader::PIAttribute);
    a.checkEqual("02. name", xr.getName(), "version");
    a.checkEqual("02. value", xr.getValue(), "1.0");
    a.checkEqual("02. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("03. attr", t == Reader::PIAttribute);
    a.checkEqual("03. name", xr.getName(), "encoding");
    a.checkEqual("03. value", xr.getValue(), "iso-8859-1");
    a.checkEqual("03. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("04. start", t == Reader::TagStart);
    a.checkEqual("04. tag", xr.getTag(), "foo");

    t = xr.readNext();
    a.check("05. start", t == Reader::TagStart);
    a.checkEqual("05. tag", xr.getTag(), "bar");

    t = xr.readNext();
    a.check("06. attr", t == Reader::TagAttribute);
    a.checkEqual("06. tag", xr.getTag(), "bar");
    a.checkEqual("06. name", xr.getName(), "zot");
    a.checkEqual("06. value", xr.getValue(), "zot");

    t = xr.readNext();
    a.check("07. end", t == Reader::TagEnd);
    a.checkEqual("07. tag", xr.getTag(), "bar");

    t = xr.readNext();
    a.check("08. start", t == Reader::TagStart);
    a.checkEqual("08. tag", xr.getTag(), "text");

    t = xr.readNext();
    a.check("09. attr", t == Reader::TagAttribute);
    a.checkEqual("09. tag", xr.getTag(), "text");
    a.checkEqual("09. name", xr.getName(), "id");
    a.checkEqual("09. value", xr.getValue(), "1");

    t = xr.readNext();
    a.check("10. text", t == Reader::Text);
    a.checkEqual("10. value", xr.getValue(), "\xc3\x82\xc2\x80");

    t = xr.readNext();
    a.check("11. end", t == Reader::TagEnd);
    a.checkEqual("11. tag", xr.getTag(), "text");

    t = xr.readNext();
    a.check("12. end", t == Reader::TagEnd);
    a.checkEqual("12. tag", xr.getTag(), "foo");

    t = xr.readNext();
    a.check("13. eof", t == Reader::Eof);
}

/** Test document in KOI8-R. */
AFL_TEST("afl.io.xml.Reader:koi8-r", a)
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
    a.check("01. start", t == Reader::PIStart);
    a.checkEqual("01. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("02. attr", t == Reader::PIAttribute);
    a.checkEqual("02. name", xr.getName(), "version");
    a.checkEqual("02. value", xr.getValue(), "1.0");
    a.checkEqual("02. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("03. attr", t == Reader::PIAttribute);
    a.checkEqual("03. name", xr.getName(), "encoding");
    a.checkEqual("03. value", xr.getValue(), "koi8r");
    a.checkEqual("03. tag", xr.getTag(), "xml");

    t = xr.readNext();
    a.check("04. start", t == Reader::TagStart);
    a.checkEqual("04. tag", xr.getTag(), "foo");

    t = xr.readNext();
    a.check("05. start", t == Reader::TagStart);
    a.checkEqual("05. tag", xr.getTag(), "bar");

    t = xr.readNext();
    a.check("06. attr", t == Reader::TagAttribute);
    a.checkEqual("06. tag", xr.getTag(), "bar");
    a.checkEqual("06. name", xr.getName(), "zot");
    a.checkEqual("06. value", xr.getValue(), "zot");

    t = xr.readNext();
    a.check("07. end", t == Reader::TagEnd);
    a.checkEqual("07. tag", xr.getTag(), "bar");

    t = xr.readNext();
    a.check("08. start", t == Reader::TagStart);
    a.checkEqual("08. tag", xr.getTag(), "text");

    t = xr.readNext();
    a.check("09. text", t == Reader::TagAttribute);
    a.checkEqual("09. text", xr.getTag(), "text");
    a.checkEqual("09. name", xr.getName(), "id");
    a.checkEqual("09. value", xr.getValue(), "1");

    t = xr.readNext();
    a.check("10. text", t == Reader::Text);
    a.checkEqual("10. value", xr.getValue(), "\xD0\xB1\xE2\x94\x80");

    t = xr.readNext();
    a.check("11. end", t == Reader::TagEnd);
    a.checkEqual("11. tag", xr.getTag(), "text");

    t = xr.readNext();
    a.check("12. end", t == Reader::TagEnd);
    a.checkEqual("12. tag", xr.getTag(), "foo");

    t = xr.readNext();
    a.check("13. eof", t == Reader::Eof);
}

/** Test document in UTF-16LE. */
AFL_TEST("afl.io.xml.Reader:utf-16LE", a)
{
    String_t b;
    for (size_t i = 0; TEST_DOCUMENT[i] != 0; ++i) {
        b += TEST_DOCUMENT[i];
        b += '\0';
    }
    afl::io::ConstMemoryStream ms(afl::string::toBytes(b));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    testBasicDocument(a, xr);
}

/** Test document in UTF-16BE. */
AFL_TEST("afl.io.xml.Reader:utf-16BE", a)
{
    String_t b;
    for (size_t i = 0; TEST_DOCUMENT[i] != 0; ++i) {
        b += '\0';
        b += TEST_DOCUMENT[i];
    }
    afl::io::ConstMemoryStream ms(afl::string::toBytes(b));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    testBasicDocument(a, xr);
}

/** Test text handling (CDATA in particular). */
AFL_TEST("afl.io.xml.Reader:text", a)
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
        a.check("type", t == xr.Text);
        result += xr.getValue();
    }
    a.checkEqual("all text", result,
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
AFL_TEST("afl.io.xml.Reader:quote", a)
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<foo a=\"bla('x')>blub\" />"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    afl::io::xml::Reader::Token t;
    t = xr.readNext();
    a.check("01. start", t == xr.TagStart);
    a.checkEqual("01. tag", xr.getTag(), "foo");

    t = xr.readNext();
    a.check("02. attr", t == xr.TagAttribute);
    a.checkEqual("02. tag", xr.getTag(), "foo");
    a.checkEqual("02. name", xr.getName(), "a");
    a.checkEqual("02. value", xr.getValue(), "bla('x')>blub");

    t = xr.readNext();
    a.check("03. end", t == xr.TagEnd);
    a.checkEqual("03. tag", xr.getTag(), "foo");

    t = xr.readNext();
    a.check("04. eof", t == xr.Eof);
}

/** Test seeking.
    Tests that parsing can properly be resumed. */
AFL_TEST("afl.io.xml.Reader:setPos", a)
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
    a.checkEqual("01. tokens.size()", tokens.size(), 3U);

    // Verify reading single tokens in turn
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] != xr.TagAttribute) {
            xr.setPos(positions[i]);
            t = xr.readNext();
            a.check("11. token type", t == tokens[i]);
            if (i > 0) {
                a.checkEqual("12. tag", xr.getTag(), tags[i]);
                a.checkEqual("13. name", xr.getName(), names[i]);
                a.checkEqual("14. value", xr.getValue(), values[i]);
            }
            a.checkEqual("15. pos", xr.getPos(), positions[i]);
        }
    }

    // Verify reading multiple tokens in turn
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] != xr.TagAttribute) {
            xr.setPos(positions[i]);
            for (size_t j = i; j < tokens.size(); ++j) {
                t = xr.readNext();
                a.check("21. token type", t == tokens[j]);
                if (j > 0) {
                    a.checkEqual("22. tag", xr.getTag(), tags[j]);
                    a.checkEqual("23. name", xr.getName(), names[j]);
                    a.checkEqual("24. value", xr.getValue(), values[j]);
                }
                a.checkEqual("25. pos", xr.getPos(), positions[j]);
            }
            a.check("26. eof", xr.readNext() == xr.Eof);
        }
    }
}

/** Test seeking.
    Tests that parsing can properly be resumed if parsing did not start at the beginning of a file. */
AFL_TEST("afl.io.xml.Reader:setPos:middle", a)
{
    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(afl::string::toBytes("   <foo>"));
    ms.setPos(3);

    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    a.check("01. start", xr.readNext() == xr.TagStart);
    a.checkEqual("01. pos", xr.getPos(), 3U);
    a.checkEqual("01. tag", xr.getTag(), "foo");

    xr.setPos(3);
    a.check("02. start", xr.readNext() == xr.TagStart);
    a.checkEqual("02. pos", xr.getPos(), 3U);
    a.checkEqual("02. tag", xr.getTag(), "foo");
}

/** Test position handling in UTF-16. */
AFL_TEST("afl.io.xml.Reader:setPos:utf-16", a)
{
    static const uint8_t DOC[] = { '<',0,'a',0,'1',0,'>',0,'<',0,'b',0,'2',0,'>',0,0,4 };
    afl::charset::DefaultCharsetFactory cf;
    afl::io::ConstMemoryStream ms(DOC);
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);

    a.check("01. start", xr.readNext() == xr.TagStart);
    a.checkEqual("01. tag", xr.getTag(), "a1");
    a.checkEqual("01. pos", xr.getPos(), 0U);

    a.check("02. start", xr.readNext() == xr.TagStart);
    a.checkEqual("02. tag", xr.getTag(), "b2");
    a.checkEqual("02. pos", xr.getPos(), 8U);

    a.check("03. text", xr.readNext() == xr.Text);
    a.checkEqual("03. value", xr.getValue(), "\xD0\x80");
    a.checkEqual("03. pos", xr.getPos(), 16U);

    a.check("04. eof", xr.readNext() == xr.Eof);

    // Seek and retry
    xr.setPos(8);
    a.check("05. start", xr.readNext() == xr.TagStart);
    a.checkEqual("05. tag", xr.getTag(), "b2");
    a.checkEqual("05. pos", xr.getPos(), 8U);
}

/** Test comment handling. */
AFL_TEST("afl.io.xml.Reader:comment", a)
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<!-- Hi --><foo>A<!-- ho-hu -->B</foo>"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    a.check("01. comment", xr.readNext() == xr.Comment);
    a.checkEqual("01. value", xr.getValue(), " Hi ");

    a.check("02. start", xr.readNext() == xr.TagStart);
    a.checkEqual("02. tag", xr.getTag(), "foo");

    a.check("03. text", xr.readNext() == xr.Text);
    a.checkEqual("03. value", xr.getValue(), "A");

    a.check("04. comment", xr.readNext() == xr.Comment);
    a.checkEqual("04. value", xr.getValue(), " ho-hu ");

    a.check("05. text", xr.readNext() == xr.Text);
    a.checkEqual("05. value", xr.getValue(), "B");

    a.check("07. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("07. tag", xr.getTag(), "foo");

    a.check("08. eof", xr.readNext() == xr.Eof);
}

/** Test whitespace mode handling: TrimWS. */
AFL_TEST("afl.io.xml.Reader:setWhitespaceMode:TrimWS-default", a)
{
    // Default configuration: trim whitespace
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<a>\n <b>X</b>\n</a>"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);

    a.check("01. getWhitespaceMode", xr.getWhitespaceMode() == xr.TrimWS);

    a.check("02. start", xr.readNext() == xr.TagStart);
    a.checkEqual("02. tag", xr.getTag(), "a");

    a.check("03. start", xr.readNext() == xr.TagStart);
    a.checkEqual("03. tag", xr.getTag(), "b");

    a.check("04. text", xr.readNext() == xr.Text);
    a.checkEqual("04. value", xr.getValue(), "X");

    a.check("05. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("05. tag", xr.getTag(), "b");

    a.check("06. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("06. tag", xr.getTag(), "a");

    a.check("07. eof", xr.readNext() == xr.Eof);
}

/** Test whitespace mode handling: SingleWS. */
AFL_TEST("afl.io.xml.Reader:setWhitespaceMode:SingleWS", a)
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<a>\n <b>X</b>\n</a>"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    xr.setWhitespaceMode(xr.SingleWS);

    a.check("01. start", xr.readNext() == xr.TagStart);
    a.checkEqual("01. tag", xr.getTag(), "a");

    a.check("02. text", xr.readNext() == xr.Text);
    a.checkEqual("02. value", xr.getValue(), " ");

    a.check("03. start", xr.readNext() == xr.TagStart);
    a.checkEqual("03. tag", xr.getTag(), "b");

    a.check("04. text", xr.readNext() == xr.Text);
    a.checkEqual("04. value", xr.getValue(), "X");

    a.check("05. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("05. tag", xr.getTag(), "b");

    a.check("06. text", xr.readNext() == xr.Text);
    a.checkEqual("06. value", xr.getValue(), " ");

    a.check("07. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("07. tag", xr.getTag(), "a");

    a.check("08. eof", xr.readNext() == xr.Eof);
}

/** Test whitespace mode handling: AllWS. */
AFL_TEST("afl.io.xml.Reader:setWhitespaceMode:AllWS", a)
{
    afl::io::ConstMemoryStream ms(afl::string::toBytes("<a>\n <b>X</b>\n</a>"));
    afl::charset::DefaultCharsetFactory cf;
    afl::io::xml::Reader xr(ms, DefaultEntityHandler::getInstance(), cf);
    xr.setWhitespaceMode(xr.AllWS);

    a.check("01. start", xr.readNext() == xr.TagStart);
    a.checkEqual("01. tag", xr.getTag(), "a");

    a.check("02. text", xr.readNext() == xr.Text);
    a.checkEqual("02. value", xr.getValue(), "\n ");

    a.check("03. start", xr.readNext() == xr.TagStart);
    a.checkEqual("03. tag", xr.getTag(), "b");

    a.check("04. text", xr.readNext() == xr.Text);
    a.checkEqual("04. value", xr.getValue(), "X");

    a.check("05. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("05. tag", xr.getTag(), "b");

    a.check("06. text", xr.readNext() == xr.Text);
    a.checkEqual("06. value", xr.getValue(), "\n");

    a.check("07. end", xr.readNext() == xr.TagEnd);
    a.checkEqual("07. tag", xr.getTag(), "a");

    a.check("08. eof", xr.readNext() == xr.Eof);
}

