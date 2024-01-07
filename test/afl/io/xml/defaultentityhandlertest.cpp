/**
  *  \file test/afl/io/xml/defaultentityhandlertest.cpp
  *  \brief Test for afl::io::xml::DefaultEntityHandler
  */

#include "afl/io/xml/defaultentityhandler.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    void testHandler(afl::test::Assert a, afl::io::xml::DefaultEntityHandler& hdl)
    {
        // Border case
        a.checkEqual("empty", hdl.expandEntityReference(String_t()), String_t());

        // XML standards
        a.checkEqual("lt",   hdl.expandEntityReference("lt"), "<");
        a.checkEqual("gt",   hdl.expandEntityReference("gt"), ">");
        a.checkEqual("amp",  hdl.expandEntityReference("amp"), "&");
        a.checkEqual("quot", hdl.expandEntityReference("quot"), "\"");
        a.checkEqual("apos", hdl.expandEntityReference("apos"), "'");

        // Numeric
        a.checkEqual("dec",       hdl.expandEntityReference("#160"), "\xC2\xA0");
        a.checkEqual("hex-a0",    hdl.expandEntityReference("#xA0"), "\xC2\xA0");
        a.checkEqual("hex-ff",    hdl.expandEntityReference("#xFF"), "\xC3\xBF");
        a.checkEqual("large-dec", hdl.expandEntityReference("#2048"), "\xE0\xA0\x80");
        a.checkEqual("large-hex", hdl.expandEntityReference("#x800"), "\xE0\xA0\x80");

        // Invalid
        a.checkEqual("invalid", hdl.expandEntityReference("xyzzy"), "xyzzy");
    }
}

/** Simple test. Tests some entities. */
AFL_TEST("afl.io.xml.DefaultEntityHandler:local", a)
{
    afl::io::xml::DefaultEntityHandler hdl;
    testHandler(a, hdl);
}

/** Test the default instance. */
AFL_TEST("afl.io.xml.DefaultEntityHandler:default", a)
{
    testHandler(a, afl::io::xml::DefaultEntityHandler::getInstance());
}
