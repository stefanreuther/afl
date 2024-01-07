/**
  *  \file test/afl/io/xml/tagnodetest.cpp
  *  \brief Test for afl::io::xml::TagNode
  */

#include "afl/io/xml/tagnode.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.xml.TagNode", a)
{
    // Constructor
    afl::io::xml::TagNode t("b");
    a.checkEqual("01. getName", t.getName(), "b");

    // set/getName
    t.setName("a");
    a.checkEqual("02. getName", t.getName(), "a");

    // No attributes
    a.checkEqual("03. getNumNames", t.getAttributeNames().getNumNames(), 0U);
    a.checkEqual("04. getAttributeByName", t.getAttributeByName("x"), "");
    a.checkEqual("05. getAttributeByIndex", t.getAttributeByIndex(0), "");

    // Add some
    t.setAttribute("x", "y");
    t.setAttribute("q", "r");
    a.checkEqual("06. getAttributeByName", t.getAttributeByName("x"), "y");
    a.checkEqual("07. getAttributeByName", t.getAttributeByName("q"), "r");
    a.checkEqual("08. getNumNames", t.getAttributeNames().getNumNames(), 2U);
    a.checkEqual("09. getNameByIndex", t.getAttributeNames().getNameByIndex(0), "x");
    a.checkEqual("10. getNameByIndex", t.getAttributeNames().getNameByIndex(1), "q");
    a.checkEqual("11. getAttributeByIndex", t.getAttributeByIndex(0), "y");
    a.checkEqual("12. getAttributeByIndex", t.getAttributeByIndex(1), "r");

    // Replace some
    t.setAttribute("x", "z");
    a.checkEqual("13. getAttributeByName", t.getAttributeByName("x"), "z");
    a.checkEqual("14. getNumNames", t.getAttributeNames().getNumNames(), 2U);

    // Case sensitive
    t.setAttribute("x", "z");
    t.setAttribute("X", "11");
    a.checkEqual("15. getAttributeByName", t.getAttributeByName("x"), "z");
    a.checkEqual("16. getAttributeByName", t.getAttributeByName("X"), "11");
    a.checkEqual("17. getNumNames", t.getAttributeNames().getNumNames(), 3U);

    // No children
    a.checkEqual("18. getChildren", t.getChildren().size(), 0U);
}

AFL_TEST("afl.io.xml.TagNode:children", a)
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
        a.checkEqual("01. ref", ref, 1);
    }
    a.checkEqual("02. ref", ref, 0);

    // Verify TagNode
    {
        afl::io::xml::TagNode tag("x");
        TestChild* c1 = new TestChild(ref);
        tag.addNewChild(c1);

        TestChild* c2 = new TestChild(ref);
        tag.addNewChild(c2);

        TestChild* c3 = new TestChild(ref);
        tag.addNewChild(c3);

        a.checkEqual("11. ref", ref, 3);
        a.checkEqual("12. size", tag.getChildren().size(), 3U);
        a.checkEqual("13. index 0", tag.getChildren()[0], static_cast<afl::io::xml::Node*>(c1));
        a.checkEqual("14. index 1", tag.getChildren()[1], static_cast<afl::io::xml::Node*>(c2));
        a.checkEqual("15. index 2", tag.getChildren()[2], static_cast<afl::io::xml::Node*>(c3));
    }
    a.checkEqual("99. ref", ref, 0);
}
