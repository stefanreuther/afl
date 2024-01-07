/**
  *  \file test/afl/io/xml/nodetest.cpp
  *  \brief Test for afl::io::xml::Node
  */

#include "afl/io/xml/node.hpp"

#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/test/testrunner.hpp"

/** Interface test. */
AFL_TEST_NOARG("afl.io.xml.Node:interface")
{
    class Tester : public afl::io::xml::Node {
     public:
        virtual void visit(afl::io::xml::Visitor& /*v*/) const
            { }
    };
    Tester t;
}

/** Test getTextContent. */
AFL_TEST("afl.io.xml.Node:getTextContent", a)
{
    afl::io::xml::TagNode tag("p");
    tag.addNewChild(new afl::io::xml::TextNode("t"));
    tag.addNewChild(new afl::io::xml::TextNode("q"));

    a.checkEqual("", tag.getTextContent(), "tq");
}
