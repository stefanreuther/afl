/**
  *  \file u/t_io_xml_textnode.cpp
  *  \brief Test for afl::io::xml::TextNode
  */

#include "afl/io/xml/textnode.hpp"

#include "t_io_xml.hpp"
#include "afl/test/callreceiver.hpp"
#include "afl/io/xml/visitor.hpp"

/** Simple tests. */
void
TestIoXmlTextNode::testIt()
{
    using afl::io::xml::TextNode;

    // Constructor/Getter
    TS_ASSERT_EQUALS(TextNode().get(), "");
    TS_ASSERT_EQUALS(TextNode("foo").get(), "foo");

    // Set
    TextNode t("x");
    t.set("y");
    TS_ASSERT_EQUALS(t.get(), "y");

}

/** Test visit(). */
void
TestIoXmlTextNode::testVisit()
{
    using afl::io::xml::TextNode;

    // Visitor
    class TestVisitor : public afl::io::xml::Visitor, public afl::test::CallReceiver {
     public:
        TestVisitor()
            : CallReceiver("TestVisitor")
            { }
        virtual void visitPI(const afl::io::xml::PINode& /*node*/)
            { TS_ASSERT(0); }
        virtual void visitTag(const afl::io::xml::TagNode& /*node*/)
            { TS_ASSERT(0); }
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

