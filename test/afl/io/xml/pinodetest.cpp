/**
  *  \file test/afl/io/xml/pinodetest.cpp
  *  \brief Test for afl::io::xml::PINode
  */

#include "afl/io/xml/pinode.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.xml.PINode", a)
{
    // Constructor
    afl::io::xml::PINode t("b");
    a.checkEqual("01. getName", t.getName(), "b");

    // set/getName
    t.setName("a");
    a.checkEqual("02. getName", t.getName(), "a");

    // No attributes
    a.checkEqual("11. getNumNames", t.getAttributeNames().getNumNames(), 0U);
    a.checkEqual("12. getAttributeByName", t.getAttributeByName("x"), "");
    a.checkEqual("13. getAttributeByIndex", t.getAttributeByIndex(0), "");

    // Add some
    t.setAttribute("x", "y");
    t.setAttribute("q", "r");
    a.checkEqual("21. getAttributeByName", t.getAttributeByName("x"), "y");
    a.checkEqual("22. getAttributeByName", t.getAttributeByName("q"), "r");
    a.checkEqual("23. getNumNames", t.getAttributeNames().getNumNames(), 2U);
    a.checkEqual("24. getNameByIndex", t.getAttributeNames().getNameByIndex(0), "x");
    a.checkEqual("25. getNameByIndex", t.getAttributeNames().getNameByIndex(1), "q");
    a.checkEqual("26. getAttributeByIndex", t.getAttributeByIndex(0), "y");
    a.checkEqual("27. getAttributeByIndex", t.getAttributeByIndex(1), "r");

    // Replace some
    t.setAttribute("x", "z");
    a.checkEqual("31. getAttributeByName", t.getAttributeByName("x"), "z");
    a.checkEqual("32. getNumNames", t.getAttributeNames().getNumNames(), 2U);

    // Case sensitive
    t.setAttribute("x", "z");
    t.setAttribute("X", "11");
    a.checkEqual("41. getAttributeByName", t.getAttributeByName("x"), "z");
    a.checkEqual("42. getAttributeByName", t.getAttributeByName("X"), "11");
    a.checkEqual("43. getNumNames", t.getAttributeNames().getNumNames(), 3U);
}
