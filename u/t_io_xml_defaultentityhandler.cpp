/**
  *  \file u/t_io_xml_defaultentityhandler.cpp
  *  \brief Test for afl::io::xml::DefaultEntityHandler
  */

#include "afl/io/xml/defaultentityhandler.hpp"

#include "u/t_io_xml.hpp"

namespace {
    void testHandler(const char name[], afl::io::xml::DefaultEntityHandler& hdl)
    {
        // Border case
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference(String_t()), String_t());

        // XML standards
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("lt"), "<");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("gt"), ">");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("amp"), "&");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("quot"), "\"");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("apos"), "'");

        // Numeric
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("#160"), "\xC2\xA0");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("#xA0"), "\xC2\xA0");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("#xFF"), "\xC3\xBF");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("#2048"), "\xE0\xA0\x80");
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("#x800"), "\xE0\xA0\x80");

        // Invalid
        TSM_ASSERT_EQUALS(name, hdl.expandEntityReference("xyzzy"), "xyzzy");
    }
}

/** Simple test. Tests some entities. */
void
TestIoXmlDefaultEntityHandler::testIt()
{
    afl::io::xml::DefaultEntityHandler hdl;
    testHandler("local", hdl);
}

/** Test the default instance. */
void
TestIoXmlDefaultEntityHandler::testDefault()
{
    testHandler("default", afl::io::xml::DefaultEntityHandler::getInstance());
}
