/**
  *  \file u/t_io_xml_writer.cpp
  *  \brief Test for afl::io::xml::Writer
  */

#include "afl/io/xml/writer.hpp"

#include "t_io_xml.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"

using afl::io::InternalSink;
using afl::io::xml::PINode;
using afl::io::xml::TagNode;
using afl::io::xml::TextNode;
using afl::io::xml::Writer;

/** Simple test (base case). */
void
TestIoXmlWriter::testSimple()
{
    TagNode node("t");

    InternalSink sink;
    Writer(sink).visit(node);

    TS_ASSERT_EQUALS(afl::string::fromBytes(sink.getContent()), "<t/>");
}

/** Complex test, using all node types. */
void
TestIoXmlWriter::testComplex()
{
    PINode node1("xml");
    node1.setAttribute("version", "1.0");

    TagNode node2("doc");
    node2.setAttribute("lang", "en");

    TagNode* node21 = new TagNode("title");
    node21->addNewChild(new TextNode("This & That"));
    node2.addNewChild(node21);
    node2.addNewChild(new TextNode("..."));
    TagNode* node22 = new TagNode("a");
    node22->setAttribute("href", "http://site/?x=1&y=2");
    node22->setAttribute("if", "x < 1");
    node2.addNewChild(node22);

    InternalSink sink;
    Writer w(sink);
    w.visit(node1);
    w.visit(node2);

    TS_ASSERT_EQUALS(afl::string::fromBytes(sink.getContent()),
                     "<?xml version=\"1.0\"?>"
                     "<doc lang=\"en\">"
                     "<title>This &amp; That</title>"
                     "..."
                     "<a href=\"http://site/?x=1&amp;y=2\" if=\"x &lt; 1\"/>"
                     "</doc>");
}

/** Test escape(); simple coverage test. */
void
TestIoXmlWriter::testEscape()
{
    TS_ASSERT_EQUALS(Writer::escape("a<b>c&d\"e"),
                     "a&lt;b&gt;c&amp;d&quot;e");
}

