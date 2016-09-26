/**
  *  \file u/t_sys_commandlineparser.cpp
  *  \brief Test for afl::sys::CommandLineParser
  */

#include "afl/sys/commandlineparser.hpp"

#include "u/t_sys.hpp"

/** Simple interface test. */
void
TestSysCommandLineParser::testIt()
{
    class Tester : public afl::sys::CommandLineParser {
     public:
        virtual bool getNext(bool&, String_t&)
            { return false; }
        virtual bool getParameter(String_t&)
            { return false; }
        virtual Flags_t getFlags()
            { return Flags_t(); }
    };
    Tester t;
}
