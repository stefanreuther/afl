/**
  *  \file test/afl/io/xml/parsertest.cpp
  *  \brief Test for afl::io::xml::Parser
  */

#include "afl/io/xml/parser.hpp"

#include <memory>
#include "afl/charset/defaultcharsetfactory.hpp"
#include "afl/io/constmemorystream.hpp"
#include "afl/io/xml/defaultentityhandler.hpp"
#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/reader.hpp"
#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/textnode.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.xml.Parser", a)
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
        a.checkNonNull("01. parse", n.get());

        afl::io::xml::PINode* pi = dynamic_cast<afl::io::xml::PINode*>(n.get());
        a.checkNonNull("02. type", pi);
        a.checkEqual("03. name", pi->getName(), "xml");
        a.checkEqual("04. attr", pi->getAttributeByName("version"), "1.0");
        a.checkEqual("05. getNumNames", pi->getAttributeNames().getNumNames(), 1U);
    }

    // Read second element (will be the <root> node)
    {
        std::auto_ptr<afl::io::xml::Node> n(testee.parse());
        a.checkNonNull("11. parse", n.get());

        afl::io::xml::TagNode* t = dynamic_cast<afl::io::xml::TagNode*>(n.get());
        a.checkNonNull("12. type", t);
        a.checkEqual("13. name", t->getName(), "root");
        a.checkEqual("14. attr", t->getAttributeByName("id"), "a");
        a.checkEqual("15. value", t->getAttributeByName("name"), "x");
        a.checkEqual("16. getNumNames", t->getAttributeNames().getNumNames(), 2U);
        a.checkEqual("17. children", t->getChildren().size(), 2U);

        // Content
        afl::io::xml::TextNode* t1 = dynamic_cast<afl::io::xml::TextNode*>(t->getChildren()[0]);
        a.checkNonNull("21. type", t1);
        a.checkEqual("22. text", t1->get(), "Hello, ");

        afl::io::xml::TagNode* t2 = dynamic_cast<afl::io::xml::TagNode*>(t->getChildren()[1]);
        a.checkNonNull("31. type", t2);
        a.checkEqual("32. name", t2->getName(), "b");
        a.checkEqual("33. attr", t2->getAttributeNames().getNumNames(), 0U);
        a.checkEqual("34. children", t2->getChildren().size(), 1U);

        afl::io::xml::TextNode* t21 = dynamic_cast<afl::io::xml::TextNode*>(t2->getChildren()[0]);
        a.checkNonNull("41. type", t21);
        a.checkEqual("42. text", t21->get(), "World");
    }
}
