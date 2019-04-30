/**
  *  \file u/t_io_xml_visitor.cpp
  *  \brief Test for afl::io::xml::Visitor
  */

#include "afl/io/xml/visitor.hpp"

#include "t_io_xml.hpp"

/** Interface test. */
void
TestIoXmlVisitor::testInterface()
{
    class Tester : public afl::io::xml::Visitor {
     public:
        virtual void visitPI(const afl::io::xml::PINode& /*node*/)
            { }
        virtual void visitTag(const afl::io::xml::TagNode& /*node*/)
            { }
        virtual void visitText(const afl::io::xml::TextNode& /*node*/)
            { }
    };
    Tester t;
}

