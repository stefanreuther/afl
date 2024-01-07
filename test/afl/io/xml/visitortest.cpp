/**
  *  \file test/afl/io/xml/visitortest.cpp
  *  \brief Test for afl::io::xml::Visitor
  */

#include "afl/io/xml/visitor.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST_NOARG("afl.io.xml.Visitor")
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
