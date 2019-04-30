/**
  *  \file u/t_io_xml_tagnode.cpp
  *  \brief Test for afl::io::xml::TagNode
  */

#include "afl/io/xml/tagnode.hpp"

#include "t_io_xml.hpp"

void
TestIoXmlTagNode::testIt()
{
    // Constructor
    afl::io::xml::TagNode t("b");
    TS_ASSERT_EQUALS(t.getName(), "b");

    // set/getName
    t.setName("a");
    TS_ASSERT_EQUALS(t.getName(), "a");

    // No attributes
    TS_ASSERT_EQUALS(t.getAttributeNames().getNumNames(), 0U);
    TS_ASSERT_EQUALS(t.getAttributeByName("x"), "");
    TS_ASSERT_EQUALS(t.getAttributeByIndex(0), "");

    // Add some
    t.setAttribute("x", "y");
    t.setAttribute("q", "r");
    TS_ASSERT_EQUALS(t.getAttributeByName("x"), "y");
    TS_ASSERT_EQUALS(t.getAttributeByName("q"), "r");
    TS_ASSERT_EQUALS(t.getAttributeNames().getNumNames(), 2U);
    TS_ASSERT_EQUALS(t.getAttributeNames().getNameByIndex(0), "x");
    TS_ASSERT_EQUALS(t.getAttributeNames().getNameByIndex(1), "q");
    TS_ASSERT_EQUALS(t.getAttributeByIndex(0), "y");
    TS_ASSERT_EQUALS(t.getAttributeByIndex(1), "r");

    // Replace some
    t.setAttribute("x", "z");
    TS_ASSERT_EQUALS(t.getAttributeByName("x"), "z");
    TS_ASSERT_EQUALS(t.getAttributeNames().getNumNames(), 2U);

    // Case sensitive
    t.setAttribute("x", "z");
    t.setAttribute("X", "11");
    TS_ASSERT_EQUALS(t.getAttributeByName("x"), "z");
    TS_ASSERT_EQUALS(t.getAttributeByName("X"), "11");
    TS_ASSERT_EQUALS(t.getAttributeNames().getNumNames(), 3U);

    // No children
    TS_ASSERT_EQUALS(t.getChildren().size(), 0U);
}

void
TestIoXmlTagNode::testChildren()
{
    class TestChild : public afl::io::xml::Node {
     public:
        TestChild(int& ref)
            : m_ref(ref)
            { ++m_ref; }
        ~TestChild()
            { --m_ref; }
              
        virtual void visit(afl::io::xml::Visitor&) const
            { }
     private:
        int& m_ref;
    };

    int ref = 0;

    // Verify TestChild
    {
        TestChild tc(ref);
        TS_ASSERT_EQUALS(ref, 1);
    }
    TS_ASSERT_EQUALS(ref, 0);

    // Verify TagNode
    {
        afl::io::xml::TagNode tag("x");
        TestChild* c1 = new TestChild(ref);
        tag.addNewChild(c1);

        TestChild* c2 = new TestChild(ref);
        tag.addNewChild(c2);

        TestChild* c3 = new TestChild(ref);
        tag.addNewChild(c3);

        TS_ASSERT_EQUALS(ref, 3);
        TS_ASSERT_EQUALS(tag.getChildren().size(), 3U);
        TS_ASSERT_EQUALS(tag.getChildren()[0], static_cast<afl::io::xml::Node*>(c1));
        TS_ASSERT_EQUALS(tag.getChildren()[1], static_cast<afl::io::xml::Node*>(c2));
        TS_ASSERT_EQUALS(tag.getChildren()[2], static_cast<afl::io::xml::Node*>(c3));
    }
    TS_ASSERT_EQUALS(ref, 0);
}

