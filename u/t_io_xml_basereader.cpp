/**
  *  \file u/t_io_xml_basereader.cpp
  *  \brief Test for afl::io::xml::BaseReader
  */

#include "afl/io/xml/basereader.hpp"

#include "u/t_io_xml.hpp"

/** Simple test.
    Tests that we can instantiate the interface. */
void
TestIoXmlBaseReader::testIt()
{
    class MyReader : public afl::io::xml::BaseReader {
     public:
        virtual Token readNext()
            { return afl::io::xml::BaseReader::Error; }
        virtual String_t getTag() const
            { return String_t(); }
        virtual String_t getName() const
            { return String_t(); }
        virtual String_t getValue() const
            { return String_t(); }
        virtual void setWhitespaceMode(WhitespaceMode /*mode*/)
            { }
        virtual WhitespaceMode getWhitespaceMode() const
            { return afl::io::xml::BaseReader::TrimWS; }
    };
    MyReader rdr;
}
