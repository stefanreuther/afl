/**
  *  \file test/afl/io/xml/textnodetest.cpp
  *  \brief Test for afl::io::xml::TextNode
  */

#include "afl/io/xml/textnode.hpp"

#include "afl/io/xml/visitor.hpp"
#include "afl/test/callreceiver.hpp"
#include "afl/test/testrunner.hpp"

/** Simple tests. */
AFL_TEST("afl.io.xml.TextNode", a)
{
    using afl::io::xml::TextNode;

    // Constructor/Getter
    a.checkEqual("01. get", TextNode().get(), "");
    a.checkEqual("02. get", TextNode("foo").get(), "foo");

    // Set
    TextNode t("x");
    t.set("y");
    a.checkEqual("03. get", t.get(), "y");
}

/** Test visit(). */
AFL_TEST_NOARG("afl.io.xml.TextNode:visit")
{
    using afl::io::xml::TextNode;

    // Visitor
    class TestVisitor : public afl::io::xml::Visitor, public afl::test::CallReceiver {
     public:
        TestVisitor()
            : CallReceiver("TestVisitor")
            { }
        virtual void visitPI(const afl::io::xml::PINode& /*node*/)
            { checkCall("visitPI"); }
        virtual void visitTag(const afl::io::xml::TagNode& /*node*/)
            { checkCall("visitTag"); }
        virtual void visitText(const afl::io::xml::TextNode& node)
            { checkCall("visitText:" + node.get()); }
    };
    TestVisitor vis;
    vis.expectCall("visitText:q");
    TextNode("q").visit(vis);

    TextNode t("zz");
    vis.expectCall("visitText:zz");
    vis.visit(t);

    vis.checkFinish();
}
