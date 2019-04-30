/**
  *  \file u/t_io_xml_nodereader.cpp
  *  \brief Test for afl::io::xml::NodeReader
  */

#include "afl/io/xml/nodereader.hpp"

#include "t_io_xml.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"

/** Test a simple tree.
    Build a tree, verify that the correct token sequence is produced. */
void
TestIoXmlNodeReader::testSimple()
{
    // Build a simple tree
    afl::io::xml::TagNode tag("a");
    tag.addNewChild(new afl::io::xml::TextNode("x"));

    afl::io::xml::PINode* pi = new afl::io::xml::PINode("pi");
    pi->setAttribute("pa", "pav");
    tag.addNewChild(pi);

    afl::io::xml::TagNode* innerTag = new afl::io::xml::TagNode("b");
    innerTag->setAttribute("e", "1");
    innerTag->setAttribute("f", "2");
    tag.addNewChild(innerTag);

    // Read it
    afl::io::xml::NodeReader t;
    t.addNode(&tag);

    TS_ASSERT_EQUALS(t.readNext(), t.TagStart);
    TS_ASSERT_EQUALS(t.getTag(), "a");

    TS_ASSERT_EQUALS(t.readNext(), t.Text);
    TS_ASSERT_EQUALS(t.getValue(), "x");

    TS_ASSERT_EQUALS(t.readNext(), t.PIStart);
    TS_ASSERT_EQUALS(t.getTag(), "pi");

    TS_ASSERT_EQUALS(t.readNext(), t.PIAttribute);
    TS_ASSERT_EQUALS(t.getTag(), "pi");
    TS_ASSERT_EQUALS(t.getName(), "pa");
    TS_ASSERT_EQUALS(t.getValue(), "pav");

    TS_ASSERT_EQUALS(t.readNext(), t.TagStart);
    TS_ASSERT_EQUALS(t.getTag(), "b");

    TS_ASSERT_EQUALS(t.readNext(), t.TagAttribute);
    TS_ASSERT_EQUALS(t.getTag(), "b");
    TS_ASSERT_EQUALS(t.getName(), "e");
    TS_ASSERT_EQUALS(t.getValue(), "1");

    TS_ASSERT_EQUALS(t.readNext(), t.TagAttribute);
    TS_ASSERT_EQUALS(t.getTag(), "b");
    TS_ASSERT_EQUALS(t.getName(), "f");
    TS_ASSERT_EQUALS(t.getValue(), "2");

    TS_ASSERT_EQUALS(t.readNext(), t.TagEnd);
    TS_ASSERT_EQUALS(t.getTag(), "b");

    TS_ASSERT_EQUALS(t.readNext(), t.TagEnd);
    TS_ASSERT_EQUALS(t.getTag(), "a");

    TS_ASSERT_EQUALS(t.readNext(), t.Eof);
}

/** Test empty reader.
    Initial state returns Eof.
    Adding a null child does not change that situation. */
void
TestIoXmlNodeReader::testEmpty()
{
    afl::io::xml::NodeReader testee;
    TS_ASSERT_EQUALS(testee.readNext(), testee.Eof);

    testee.addNode(0);
    TS_ASSERT_EQUALS(testee.readNext(), testee.Eof);
}

void
TestIoXmlNodeReader::testWhitespaceMode()
{
    afl::io::xml::TagNode tag("t");
    tag.addNewChild(new afl::io::xml::TextNode(" \t"));

    // Regular
    {
        afl::io::xml::NodeReader r;
        r.addNode(&tag);

        // Default is AllWS = tree is given out unmodified
        TS_ASSERT_EQUALS(r.getWhitespaceMode(), r.AllWS);

        TS_ASSERT_EQUALS(r.readNext(), r.TagStart);
        TS_ASSERT_EQUALS(r.getTag(), "t");

        TS_ASSERT_EQUALS(r.readNext(), r.Text);
        TS_ASSERT_EQUALS(r.getValue(), " \t");

        TS_ASSERT_EQUALS(r.readNext(), r.TagEnd);
        TS_ASSERT_EQUALS(r.getTag(), "t");

        TS_ASSERT_EQUALS(r.readNext(), r.Eof);
    }

    // TrimWS
    {
        afl::io::xml::NodeReader r;
        r.addNode(&tag);
        r.setWhitespaceMode(r.TrimWS);

        TS_ASSERT_EQUALS(r.readNext(), r.TagStart);
        TS_ASSERT_EQUALS(r.getTag(), "t");

        TS_ASSERT_EQUALS(r.readNext(), r.TagEnd);
        TS_ASSERT_EQUALS(r.getTag(), "t");

        TS_ASSERT_EQUALS(r.readNext(), r.Eof);
    }

    // SingleWS
    {
        afl::io::xml::NodeReader r;
        r.addNode(&tag);
        r.setWhitespaceMode(r.SingleWS);

        TS_ASSERT_EQUALS(r.readNext(), r.TagStart);
        TS_ASSERT_EQUALS(r.getTag(), "t");

        TS_ASSERT_EQUALS(r.readNext(), r.Text);
        TS_ASSERT_EQUALS(r.getValue(), " ");

        TS_ASSERT_EQUALS(r.readNext(), r.TagEnd);
        TS_ASSERT_EQUALS(r.getTag(), "t");

        TS_ASSERT_EQUALS(r.readNext(), r.Eof);
    }
}

