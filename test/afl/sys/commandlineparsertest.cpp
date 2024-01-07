/**
  *  \file test/afl/sys/commandlineparsertest.cpp
  *  \brief Test for afl::sys::CommandLineParser
  */

#include "afl/sys/commandlineparser.hpp"

#include "afl/except/commandlineexception.hpp"
#include "afl/test/testrunner.hpp"

/** Simple interface test. */
AFL_TEST_NOARG("afl.sys.CommandLineParser:interface")
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
AFL_TEST("afl.sys.CommandLineParser:getRequiredParameter", a)
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
    AFL_CHECK_THROWS(a("missing parameter"), t.getRequiredParameter("foo"), afl::except::CommandLineException);

    // Give it a parameter
    t.set("blub");
    a.checkEqual("available parameter", t.getRequiredParameter("foo"), "blub");
}
