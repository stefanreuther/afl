/**
  *  \file u/t_io_xml_parser.cpp
  *  \brief Test for afl::io::xml::Parser
  */

#include <memory>
#include "afl/io/xml/parser.hpp"

#include "t_io_xml.hpp"
#include "afl/io/xml/reader.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/xml/defaultentityhandler.hpp"
#include "afl/charset/defaultcharsetfactory.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"

void
TestIoXmlParser::testIt()
{
    // Data to test
    afl::io::ConstMemoryStream in(afl::string::toBytes("<?xml version=\"1.0\"><root id=\"a\" name=\"x\">Hello, <b>World</b></root>"));

    // Parser infrastructure
    afl::charset::DefaultCharsetFactory csFactory;
    afl::io::xml::DefaultEntityHandler eh;
    afl::io::xml::Reader rdr(in, eh, csFactory);
    afl::io::xml::Parser testee(rdr);

    // Read first element (will be the processing instruction)
    {
        std::auto_ptr<afl::io::xml::Node> n(testee.parse());
        TS_ASSERT(n.get() != 0);

        afl::io::xml::PINode* pi = dynamic_cast<afl::io::xml::PINode*>(n.get());
        TS_ASSERT(pi != 0);
        TS_ASSERT_EQUALS(pi->getName(), "xml");
        TS_ASSERT_EQUALS(pi->getAttributeByName("version"), "1.0");
        TS_ASSERT_EQUALS(pi->getAttributeNames().getNumNames(), 1U);
    }

    // Read second element (will be the <root> node)
    {
        std::auto_ptr<afl::io::xml::Node> n(testee.parse());
        TS_ASSERT(n.get() != 0);

        afl::io::xml::TagNode* t = dynamic_cast<afl::io::xml::TagNode*>(n.get());
        TS_ASSERT(t != 0);
        TS_ASSERT_EQUALS(t->getName(), "root");
        TS_ASSERT_EQUALS(t->getAttributeByName("id"), "a");
        TS_ASSERT_EQUALS(t->getAttributeByName("name"), "x");
        TS_ASSERT_EQUALS(t->getAttributeNames().getNumNames(), 2U);
        TS_ASSERT_EQUALS(t->getChildren().size(), 2U);

        // Content
        afl::io::xml::TextNode* t1 = dynamic_cast<afl::io::xml::TextNode*>(t->getChildren()[0]);
        TS_ASSERT(t1 != 0);
        TS_ASSERT_EQUALS(t1->get(), "Hello, ");

        afl::io::xml::TagNode* t2 = dynamic_cast<afl::io::xml::TagNode*>(t->getChildren()[1]);
        TS_ASSERT(t2 != 0);
        TS_ASSERT_EQUALS(t2->getName(), "b");
        TS_ASSERT_EQUALS(t2->getAttributeNames().getNumNames(), 0U);
        TS_ASSERT_EQUALS(t2->getChildren().size(), 1U);

        afl::io::xml::TextNode* t21 = dynamic_cast<afl::io::xml::TextNode*>(t2->getChildren()[0]);
        TS_ASSERT(t21 != 0);
        TS_ASSERT_EQUALS(t21->get(), "World");
    }
}

