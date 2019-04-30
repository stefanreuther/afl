/**
  *  \file u/t_io_xml_pinode.cpp
  *  \brief Test for afl::io::xml::PINode
  */

#include "afl/io/xml/pinode.hpp"

#include "t_io_xml.hpp"

/** Basic tests. */
void
TestIoXmlPINode::testIt()
{
    // Constructor
    afl::io::xml::PINode t("b");
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
}

