/**
  *  \file test/afl/io/xml/entityhandlertest.cpp
  *  \brief Test for afl::io::xml::EntityHandler
  */

#include "afl/io/xml/entityhandler.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test.
    Tests that we can instantiate the interface. */
AFL_TEST_NOARG("afl.io.xml.EntityHandler")
{
    class Handler : public afl::io::xml::EntityHandler {
     public:
        String_t expandEntityReference(String_t in)
            { return in; }
    };
    Handler hdl;
}
