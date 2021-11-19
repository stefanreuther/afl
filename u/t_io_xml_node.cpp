/**
  *  \file u/t_io_xml_node.cpp
  *  \brief Test for afl::io::xml::Node
  */

#include "afl/io/xml/node.hpp"

#include "t_io_xml.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/io/xml/tagnode.hpp"

/** Interface test. */
void
TestIoXmlNode::testInterface()
{
    class Tester : public afl::io::xml::Node {
     public:
        virtual void visit(afl::io::xml::Visitor& /*v*/) const
            { }
    };
    Tester t;
}

/** Test getTextContent. */
void
TestIoXmlNode::testGetTextContent()
{
    afl::io::xml::TagNode tag("p");
    tag.addNewChild(new afl::io::xml::TextNode("t"));
    tag.addNewChild(new afl::io::xml::TextNode("q"));

    TS_ASSERT_EQUALS(tag.getTextContent(), "tq");
}

