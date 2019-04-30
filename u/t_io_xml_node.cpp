/**
  *  \file u/t_io_xml_node.cpp
  *  \brief Test for afl::io::xml::Node
  */

#include "afl/io/xml/node.hpp"

#include "t_io_xml.hpp"

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

