/**
  *  \file u/t_io_xml_entityhandler.cpp
  *  \brief Test for afl::io::xml::EntityHandler
  */

#include "afl/io/xml/entityhandler.hpp"

#include "u/t_io_xml.hpp"

/** Simple test.
    Tests that we can instantiate the interface. */
void
TestIoXmlEntityHandler::testIt()
{
    class Handler : public afl::io::xml::EntityHandler {
     public:
        String_t expandEntityReference(String_t in)
            { return in; }
    };
    Handler hdl;
}
