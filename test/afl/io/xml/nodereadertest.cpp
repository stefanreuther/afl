/**
  *  \file test/afl/io/xml/nodereadertest.cpp
  *  \brief Test for afl::io::xml::NodeReader
  */

#include "afl/io/xml/nodereader.hpp"

#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/test/testrunner.hpp"

/** Test a simple tree.
    Build a tree, verify that the correct token sequence is produced. */
AFL_TEST("afl.io.xml.NodeReader:sequence", a)
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

    a.check("01. start", t.readNext() == t.TagStart);
    a.checkEqual("01. tag", t.getTag(), "a");

    a.check("02. text", t.readNext() == t.Text);
    a.checkEqual("02. value", t.getValue(), "x");

    a.check("03. start", t.readNext() == t.PIStart);
    a.checkEqual("03. tag", t.getTag(), "pi");

    a.check("04. attr", t.readNext() == t.PIAttribute);
    a.checkEqual("04. tag", t.getTag(), "pi");
    a.checkEqual("04. name", t.getName(), "pa");
    a.checkEqual("04. value", t.getValue(), "pav");

    a.check("05. start", t.readNext() == t.TagStart);
    a.checkEqual("05. tag", t.getTag(), "b");

    a.check("06. attr", t.readNext() == t.TagAttribute);
    a.checkEqual("06. tag", t.getTag(), "b");
    a.checkEqual("06. name", t.getName(), "e");
    a.checkEqual("06. value", t.getValue(), "1");

    a.check("07. attr", t.readNext() == t.TagAttribute);
    a.checkEqual("07. tag", t.getTag(), "b");
    a.checkEqual("07. name", t.getName(), "f");
    a.checkEqual("07. avlue", t.getValue(), "2");

    a.check("08. end", t.readNext() == t.TagEnd);
    a.checkEqual("08. tag", t.getTag(), "b");

    a.check("09. end", t.readNext() == t.TagEnd);
    a.checkEqual("09. tag", t.getTag(), "a");

    a.check("10. eof", t.readNext() == t.Eof);
}

/** Test empty reader.
    Initial state returns Eof.
    Adding a null child does not change that situation. */
AFL_TEST("afl.io.xml.NodeReader:empty", a)
{
    afl::io::xml::NodeReader testee;
    a.check("eof 1", testee.readNext() == testee.Eof);

    testee.addNode(0);
    a.check("eof 2", testee.readNext() == testee.Eof);
}

AFL_TEST("afl.io.xml.NodeReader:whitespace-mode", aa)
{
    afl::io::xml::TagNode tag("t");
    tag.addNewChild(new afl::io::xml::TextNode(" \t"));

    // Regular
    {
        afl::test::Assert a(aa("default"));
        afl::io::xml::NodeReader r;
        r.addNode(&tag);

        // Default is AllWS = tree is given out unmodified
        a.check("01. getWhitespaceMode", r.getWhitespaceMode() == r.AllWS);

        a.check("02. start", r.readNext() == r.TagStart);
        a.checkEqual("02. tag", r.getTag(), "t");

        a.check("03. text", r.readNext() == r.Text);
        a.checkEqual("03. value", r.getValue(), " \t");

        a.check("04. end", r.readNext() == r.TagEnd);
        a.checkEqual("04. tag", r.getTag(), "t");

        a.check("05. eof", r.readNext() == r.Eof);
    }

    // TrimWS
    {
        afl::test::Assert a(aa("TrimWS"));
        afl::io::xml::NodeReader r;
        r.addNode(&tag);
        r.setWhitespaceMode(r.TrimWS);

        a.check("01. start", r.readNext() == r.TagStart);
        a.checkEqual("01. tag", r.getTag(), "t");

        a.check("02. end", r.readNext() == r.TagEnd);
        a.checkEqual("02. tag", r.getTag(), "t");

        a.check("03. eof", r.readNext() == r.Eof);
    }

    // SingleWS
    {
        afl::test::Assert a(aa("SingleWS"));
        afl::io::xml::NodeReader r;
        r.addNode(&tag);
        r.setWhitespaceMode(r.SingleWS);

        a.check("01. start", r.readNext() == r.TagStart);
        a.checkEqual("01. tag", r.getTag(), "t");

        a.check("02. text", r.readNext() == r.Text);
        a.checkEqual("02. value", r.getValue(), " ");

        a.check("03. end", r.readNext() == r.TagEnd);
        a.checkEqual("03. tag", r.getTag(), "t");

        a.check("04. eof", r.readNext() == r.Eof);
    }
}
