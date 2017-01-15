/**
  *  \file u/t_sys_commandlineparser.cpp
  *  \brief Test for afl::sys::CommandLineParser
  */

#include "afl/sys/commandlineparser.hpp"

#include "u/t_sys.hpp"
#include "afl/except/commandlineexception.hpp"

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

/** Test getRequiredParameter. */
void
TestSysCommandLineParser::testRequired()
{
    class Tester : public afl::sys::CommandLineParser {
     public:
        Tester()
            : m_parameter(),
              m_hasParameter(false)
            { }

        virtual bool getNext(bool& /*option*/, String_t& /*text*/)
            { return false; }
        virtual bool getParameter(String_t& value)
            {
                if (m_hasParameter) {
                    value = m_parameter;
                    m_hasParameter = false;
                    return true;
                } else {
                    return false;
                }
            }
        virtual Flags_t getFlags()
            { return Flags_t(); }

        void set(String_t param)
            {
                m_parameter = param;
                m_hasParameter = true;
            }
     private:
        String_t m_parameter;
        bool m_hasParameter;
    };
    Tester t;

    // Nothing available -> getRequiredParameter() will throw
    TS_ASSERT_THROWS(t.getRequiredParameter("foo"), afl::except::CommandLineException);

    // Give it a parameter
    t.set("blub");
    TS_ASSERT_EQUALS(t.getRequiredParameter("foo"), "blub");
}
